// 2020.03.31 - Victor Dods

#include "lvd/literal.hpp"

#include <cassert>

namespace lvd {

std::string literal_of (bool b) {
    return b ? "true" : "false";
}

std::string literal_of (char c) {
    return char_literal_of(c, true);
}

std::string literal_of (char const *s) {
    return string_literal_of(s, true);
}

std::string literal_of (std::string const &s) {
    return string_literal_of(s, true);
}

// this anonymous namespace is for local-file-scoping of certain functions
namespace {

bool IsOctalDigit (uint8_t c)
{
    return '0' <= c && c <= '7';
}

bool IsHexDigit (uint8_t c)
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

} // end of anonymous namespace

bool char_needs_hex_escaping (uint8_t c)
{
    // normal escaping will suffice for these (a b t n v f r are contiguous).
    if (c == '\0' || (c >= '\a' && c <= '\r'))
        return false;
    // otherwise, only use hex escaping for anything outside the printables
    else
        return c < ' ' || c > '~';
}

bool char_literal_char_needs_normal_escaping (uint8_t c)
{
    // normal escaping will suffice for these (a b t n v f r are contiguous).
    return c == '\0' || (c >= '\a' && c <= '\r') || c == '\\' || c == '\'';
}

bool StringLiteralCharNeedsNormalEscaping (uint8_t c)
{
    // normal escaping will suffice for these (a b t n v f r are contiguous).
    return c == '\0' || (c >= '\a' && c <= '\r') || c == '\\' || c == '\"';
}

uint8_t hex_char (uint8_t hex_digit)
{
    assert(hex_digit < 0x10);
    if (hex_digit < 0xA)
        return hex_digit + '0';
    hex_digit -= 0xA;
    return hex_digit + 'A';
}

std::string hex_char_literal (uint8_t c, bool with_quotes)
{
    std::string retval;
    if (with_quotes)
        retval += '\'';
    retval += '\\';
    retval += 'x';
    retval += char(hex_char(c >> 4));
    retval += char(hex_char(c & 0xF));
    if (with_quotes)
        retval += '\'';
    return retval;
}

uint8_t escape_code (uint8_t c)
{
    switch (c)
    {
        case '\0': return '0';
        case '\a': return 'a';
        case '\b': return 'b';
        case '\t': return 't';
        case '\n': return 'n';
        case '\v': return 'v';
        case '\f': return 'f';
        case '\r': return 'r';
        default  : return c;
    }
}

std::string char_literal_of (uint8_t c, bool with_quotes)
{
    std::string retval;
    if (with_quotes)
        retval += '\'';

    if (char_literal_char_needs_normal_escaping(c))
        retval += '\\', retval += escape_code(c);
    else if (char_needs_hex_escaping(c))
        retval += hex_char_literal(c, false);
    else
        retval += c;

    if (with_quotes)
        retval += '\'';
    return retval;
}

void escape_char (uint8_t &c)
{
    switch (c)
    {
        case '0': c = '\0'; break;
        case 'a': c = '\a'; break;
        case 'b': c = '\b'; break;
        case 't': c = '\t'; break;
        case 'n': c = '\n'; break;
        case 'v': c = '\v'; break;
        case 'f': c = '\f'; break;
        case 'r': c = '\r'; break;
        default : /* do nothing */ break;
    }
}

uint8_t escaped_char (uint8_t c)
{
    uint8_t retval = c;
    escape_char(retval);
    return retval;
}

// this function relies on the fact that an escaped string (i.e. a string with
// escape codes such as \n or \xA7 converted into single chars) is not longer
// than the original string.
EscapeStringStatus escape_string (std::string &text)
{
    auto return_code = EscapeStringReturnCode::SUCCESS;
    std::string::size_type read_cursor = 0;
    std::string::size_type write_cursor = 0;
    uint32_t line_number_offset = 0;
    while (read_cursor < text.length())
    {
        assert(write_cursor <= read_cursor);
        if (text[read_cursor] == '\\')
        {
            ++read_cursor;
            // if we're at the end of the string, this is an error.
            if (read_cursor == text.length())
            {
                return_code = EscapeStringReturnCode::UNEXPECTED_EOI;
                break;
            }
            // if there's an x next, we'll expect hex digits after it.
            // if there's an octal digit next, read octal.
            else if (text[read_cursor] == 'x' || IsOctalDigit(text[read_cursor]))
            {
                int radix = 8;
                std::string::size_type end_of_digits;

                if (text[read_cursor] == 'x')
                {
                    radix = 16;

                    ++read_cursor;
                    // if we're at the end of the string, or if there are
                    // no hex digits after the x, this is an error.
                    if (read_cursor == text.length() || !IsHexDigit(text[read_cursor]))
                    {
                        return_code = EscapeStringReturnCode::MALFORMED_HEX_CHAR;
                        break;
                    }

                    // otherwise find the range of hex digits and read them.
                    end_of_digits = read_cursor;
                    while (end_of_digits < text.length() && IsHexDigit(text[end_of_digits]))
                        ++end_of_digits;
                }
                else
                {
                    // find the range of octal digits and read them.
                    end_of_digits = read_cursor;
                    while (end_of_digits < text.length() && IsOctalDigit(text[end_of_digits]))
                        ++end_of_digits;
                }

                assert(end_of_digits > read_cursor);

                // decode the digits into a value
                uint32_t value = strtol(text.c_str()+read_cursor, NULL, 16);
                // handle out-of-range error
                if (value >= 256)
                {
                    if (radix == 16)
                        return_code = EscapeStringReturnCode::HEX_ESCAPE_SEQUENCE_OUT_OF_RANGE;
                    else
                        return_code = EscapeStringReturnCode::OCTAL_ESCAPE_SEQUENCE_OUT_OF_RANGE;
                    break;
                }

                // otherwise everything was ok, so write the hex char
                text[write_cursor] = uint8_t(value);
                read_cursor = end_of_digits;
            }
            // otherwise it's a normal escape char
            else
            {
                if (text[read_cursor] == '\n')
                    ++line_number_offset;
                text[write_cursor] = escaped_char(text[read_cursor]);
                ++read_cursor;
            }
        }
        else
        {
            if (text[read_cursor] == '\n')
                ++line_number_offset;
            text[write_cursor] = text[read_cursor];
            ++read_cursor;
        }
        ++write_cursor;
    }
    // chop off the unused portion of the string, and return the return code.
    text.resize(write_cursor);
    return EscapeStringStatus(return_code, line_number_offset);
}

std::string string_literal_of (std::string const &text, bool with_quotes)
{
    std::string retval;
    if (with_quotes)
        retval += '"';

    for (std::string::const_iterator it = text.begin(),
                                     it_end = text.end();
         it != it_end;
         ++it)
    {
        uint8_t c = *it;
        if (StringLiteralCharNeedsNormalEscaping(c))
        {
            retval += '\\';
            retval += escape_code(c);
        }
        else if (char_needs_hex_escaping(c))
            retval += hex_char_literal(c, false);
        else
            retval += char(c);
    }

    if (with_quotes)
        retval += '"';
    return retval;
}

} // end namespace lvd
