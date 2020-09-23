// 2020.03.31 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <cmath>
#include "lvd/FiLoc.hpp"
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace lvd {

// Returns the actual remainder of an integer division, not this bastardized, signed version
// of it as specified by C++11.  For example, it will give -1 for (-7) % 3, instead of
// the correct mathematical result which is 2 (since the remainder of a / b is defined to
// be a value r in the range [0, b) such that q*b + r = a (here, q is the quotient).
// To simplify things a bit, this function requires that b, the divisor, is positive.
template <typename Int_>
std::make_unsigned_t<Int_> remainder (Int_ a, std::make_unsigned_t<Int_> b) {
    assert(b > 0 && "divisor must be positive");
    if (std::make_unsigned_t<Int_>(std::abs(a)) < b) {
        // No need to do the division (this also handles the case where casting b to
        // the [possibly signed] Int_ type would overflow Int_.
        return a >= 0 ? a : b + a;
    } else {
        Int_ m = a % Int_(b);
        return m >= 0 ? m : b + m;
    }
}

// Returns a std::string_view with the whitespace trimmed off the beginning and end.
std::string_view string_view_trimmed (std::string_view const &str);

// Returns a std::string_view with the whitespace trimmed off the beginning and end of the std::string.
inline std::string_view string_trimmed (std::string const &str) {
    return string_view_trimmed(std::string_view(&str[0], str.size()));
}

// Joins the given items together as strings (via use std::ostringstream), using the given separator.
template <typename Iterator_>
std::string string_join (std::string const &separator, Iterator_ begin, Iterator_ end) {
    std::ostringstream out;
    for (auto it = begin; it != end; ++it) {
        out << *it;
        auto next_it = std::next(it);
        if (next_it != end)
            out << separator;
    }
    return out.str();
}

// Joins the items of the given range together as strings (via use std::ostringstream), using the given separator.
template <typename Range_>
std::string string_join (std::string const &separator, Range_ const &r) {
    std::ostringstream out;
    for (auto it = r.begin(); it != r.end(); ++it) {
        out << *it;
        auto next_it = std::next(it);
        if (next_it != r.end())
            out << separator;
    }
    return out.str();
}

//
// Radix
//

enum class Radix : uint8_t {
    BIN = 2,
    OCT = 8,
    DEC = 10,
    HEX = 16,

    __HIGHEST__ = HEX
};

// A std::vector containing all values of Radix, in ascending order.
extern std::vector<Radix> const RADIX_VECTOR;

std::string const &as_string (Radix radix);

// Returns "binary", "octal", "decimal", or "hexadecimal".
std::string const &radix_name (Radix radix);

// Returns "0b" for BIN, "0o" for OCT, "" for DEC, and "0x" for HEX.
std::string const &radix_prefix (Radix radix);

inline std::ostream &operator << (std::ostream &out, Radix radix) {
    return out << as_string(radix);
}

Radix cycled_radix (Radix radix);

//
// RadixFlags
//

enum class RadixFlags : uint32_t {
    NONE = 0,
    BIN = (1 << 2),
    OCT = (1 << 8),
    DEC = (1 << 10),
    HEX = (1 << 16),
    ANY = BIN|OCT|DEC|HEX,

    __HIGHEST__ = HEX
};

std::string as_string (RadixFlags radix_flags);

inline std::ostream &operator << (std::ostream &out, RadixFlags radix_flags) {
    return out << as_string(radix_flags);
}

// Returns the RadixFlags value corresponding to the given radix.
RadixFlags as_radix_flags (Radix radix);
// Returns true iff exactly one flag is set.
inline bool is_single_radix_flag (RadixFlags radix_flags) {
    return (uint32_t(radix_flags) & (uint32_t(radix_flags)-1)) == 0;
}
// If single_radix_flag corresponds to exactly one of the RadixFlags elements (i.e. one bit is set),
// then this returns the corresponding Radix value.  Otherwise throws std::runtime_error.  See also
// is_single_radix_flag.
Radix as_radix (RadixFlags single_radix_flag);

std::vector<Radix> as_radix_vector (RadixFlags radix_flags);

// Bitwise AND operation for RadixFlags.
inline RadixFlags operator & (RadixFlags lhs, RadixFlags rhs) { return RadixFlags(uint32_t(lhs) & uint32_t(rhs)); }
// Bitwise OR operation for RadixFlags.
inline RadixFlags operator | (RadixFlags lhs, RadixFlags rhs) { return RadixFlags(uint32_t(lhs) | uint32_t(rhs)); }

//
// Parsing and rendering for uint64_t and int64_t
//

// Parses uint64_t out of the given std::string_view.  No radix prefix, no sign.
uint64_t string_view_to_uint64_raw (std::string_view const &str_view, Radix radix, uint64_t upper_bound = std::numeric_limits<uint64_t>::max());

// Parses uint64_t out of the given std::string.  No radix prefix, no sign.
inline uint64_t string_to_uint64_raw (std::string const &str, Radix radix, uint64_t upper_bound = std::numeric_limits<uint64_t>::max()) {
    return string_view_to_uint64_raw(std::string_view(&str[0], str.size()), radix, upper_bound);
}

enum class UseRadixPrefix : uint8_t {
    NO = 0,
    YES
};

enum class SignOption : uint8_t {
    ALWAYS_PRINT_SIGN = 0,
    ONLY_PRINT_NEGATIVE_SIGN
};

// Renders the given int64_t as a std::string in the given radix.  Optional radix prefix and plus sign
// are configurable via use_radix_prefix and sign_option.
std::string int64_to_string (int64_t value, Radix radix, UseRadixPrefix use_radix_prefix, SignOption sign_option);
// Parses an int64_t out of the given std::string, throwing std::runtime_error upon error (including
// but not limited to, ill-formed string, value overflow, non-allowed radix).
int64_t string_to_int64 (std::string const &str, RadixFlags allowed = RadixFlags::ANY);

struct Tabs
{
    uint32_t m_count;

    Tabs (uint32_t count) : m_count(count) { }
};

std::ostream &operator << (std::ostream &out, Tabs const &tabs);

uint32_t newline_count (std::string const &text);

// If out == nullptr, this won't do anything.
void emit_execution_message (std::ostream *out, std::string const &message);
// If out == nullptr, this won't do anything.
void emit_warning (std::ostream *out, std::string const &message, FiLoc const &filoc);
// If out == nullptr, this won't do anything.
void emit_error (std::ostream *out, std::string const &message, FiLoc const &filoc);

} // end namespace lvd
