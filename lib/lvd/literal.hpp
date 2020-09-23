// 2020.03.31 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/fmt.hpp"
#include <string>

namespace lvd {

//
// The main point of this header is the family of `literal` function overloads.
//

std::string literal_of (bool b);
std::string literal_of (char c);
std::string literal_of (char const *s);
std::string literal_of (std::string const &s);

// Fallback
template <typename T_>
std::string literal_of (T_ const &x) {
    return LVD_FMT('`' << x << '`');
}

//
// Supporting code
//

enum class EscapeStringReturnCode : uint8_t
{
    SUCCESS = 0,
    // if there's a backslash immediately before the end of the string
    UNEXPECTED_EOI,
    // if there's \x without a hex digit after it
    MALFORMED_HEX_CHAR,
    // if the hex code's value exceeded 255
    HEX_ESCAPE_SEQUENCE_OUT_OF_RANGE,
    // if the octal code's value exceeded 255
    OCTAL_ESCAPE_SEQUENCE_OUT_OF_RANGE
}; // end of enum EscapeStringReturnCode

struct EscapeStringStatus
{
    EscapeStringReturnCode m_return_code;
    uint32_t m_line_number_offset;

    EscapeStringStatus (EscapeStringReturnCode return_code, uint32_t line_number_offset)
        :
        m_return_code(return_code),
        m_line_number_offset(line_number_offset)
    { }
}; // end of struct EscapeStringStatus

bool char_needs_hex_escaping (uint8_t c);
bool char_literal_char_needs_normal_escaping (uint8_t c);
uint8_t hex_char (uint8_t hex_digit);
std::string hex_char_literal (uint8_t c, bool with_quotes);
uint8_t escape_code (uint8_t c);
std::string char_literal_of (uint8_t c, bool with_quotes = true);

// in-place char-escaping
void escape_char (uint8_t &c);
uint8_t escaped_char (uint8_t c);
// in-place string-escaping
EscapeStringStatus escape_string (std::string &text);
std::string string_literal_of (std::string const &text, bool with_quotes = true);

} // end namespace lvd
