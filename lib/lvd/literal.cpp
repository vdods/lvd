// 2020.03.31 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/literal.hpp"

#include <cassert>
#include "lvd/abort.hpp"

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
// TODO: This has to be fixed to reflect the fixes to string_literal_of.
// See https://stackoverflow.com/questions/10220401/rules-for-c-string-literals-escape-character
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

// Semantic subtype of char which prints as \XYZ for octal digits X, Y, Z.  Always 3 digits,
// so as to give unambiguous and context-free string literal behavior.
// See https://stackoverflow.com/questions/10220401/rules-for-c-string-literals-escape-character
class BackslashOctalChar {
public:

    BackslashOctalChar () = delete;
    BackslashOctalChar (char value) : m_value(value) { }

    char value () const { return m_value; }
    char &value () { return m_value; }

    template <uint32_t DIGIT_>
    uint32_t digit () const {
        static_assert(DIGIT_ <= 2, "DIGIT_ must be 0, 1, or 2");
        return (uint32_t(m_value) >> (3*DIGIT_)) & 7;
    }

private:

    char m_value;
};

inline std::ostream &operator<< (std::ostream &out, BackslashOctalChar const &oc) {
    return out << '\\' << oc.digit<2>() << oc.digit<1>() << oc.digit<0>();
}

// Semantic subtype of char.  Anything that isn't a normal printable char or have a single-char escape code,
// e.g. \n or \a, should be printed as a full, 3-digit octal code, because that's the only way to get
// unambiguous and context-free behavior out of string literals.
// See https://stackoverflow.com/questions/10220401/rules-for-c-string-literals-escape-character
class StringLiteralChar {
public:

    StringLiteralChar () = delete;
    StringLiteralChar (char value) : m_value(value) { }

    char value () const { return m_value; }
    char &value () { return m_value; }

    std::ostream &print (std::ostream &out) const {
        switch (type_of(m_value)) {
            case Type::DIRECTLY_PRINTABLE: return out << m_value;
            case Type::BACKSLASH_CHAR: return out << '\\' << backslash_char_of(m_value);
            case Type::BACKSLASH_OCTAL: return out << BackslashOctalChar(m_value);
            default: LVD_ABORT("invalid StringLiteralChar::Type");
        }
    }

private:

    enum class Type {
        DIRECTLY_PRINTABLE = 0,
        BACKSLASH_CHAR,
        BACKSLASH_OCTAL
    };

    static Type type_of (char c) {
        if (('\a' <= c && c <= '\r') || c == '"' || c == '\\')
            return Type::BACKSLASH_CHAR;
        else if (' ' <= c && c <= '~') // Note that this range depends on '"' and '\\' having already been handled.
            return Type::DIRECTLY_PRINTABLE;
        else
            return Type::BACKSLASH_OCTAL;
    }
    static char backslash_char_of (char c) {
        assert(type_of(c) == Type::BACKSLASH_CHAR);
        switch (c) {
            case '\a': return 'a';
            case '\b': return 'b';
            case '\t': return 't';
            case '\n': return 'n';
            case '\v': return 'v';
            case '\f': return 'f';
            case '\r': return 'r';
            case '\"': return '\"';
            case '\\': return '\\';
            default: LVD_ABORT("expected type_of(c) == Type::BACKSLASH_CHAR");
        }
    }

    char m_value;
};

inline std::ostream &operator<< (std::ostream &out, StringLiteralChar const &c) {
    return c.print(out);
}

std::string string_literal_of (std::string const &text, bool with_quotes) {
    std::ostringstream out;
    if (with_quotes)
        out << '"';
    for (auto c : text)
        out << StringLiteralChar(c);
    if (with_quotes)
        out << '"';
    return out.str(); // Hopefully this uses RVO to not deep-copy the string.
}

} // end namespace lvd
