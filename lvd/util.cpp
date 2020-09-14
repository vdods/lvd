// 2020.03.31 - Victor Dods

#include "lvd/util.hpp"

#include <algorithm>
#include "lvd/fmt.hpp"
#include <regex>

namespace lvd {

std::string const WHITESPACE{" \f\n\r\t\v"}; // As defined by std::isspace in default locale

std::string_view string_view_trimmed (std::string_view const &str_view) {
    auto begin_index = str_view.find_first_not_of(WHITESPACE);
    if (begin_index == std::string::npos)
        return std::string_view(); // Empty
    auto end_index = str_view.find_last_not_of(WHITESPACE);
    if (end_index == std::string::npos)
        end_index = str_view.size();
    else
        ++end_index; // end_index includes the last non-whitespace char, so increment the index.
    assert(begin_index <= end_index);
    assert(end_index <= str_view.size());
    return std::string_view(&str_view[0]+begin_index, end_index-begin_index);
}

//
// Radix
//

std::string const &as_string (Radix radix);

RadixFlags as_radix_flags (Radix radix) {
    if (uint32_t(radix) > uint32_t(Radix::__HIGHEST__))
        throw std::runtime_error("radix out of range");
//     return RADIX_FLAGS[uint16_t(radix)];
    return RadixFlags(1 << uint32_t(radix));
}

std::string const &as_string (Radix radix) {
    static std::string const STRINGS[size_t(Radix::__HIGHEST__)+1] = {
        "<invalid-radix>", // 0
        "<invalid-radix>",
        "bin", // 2
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "oct", // 8
        "<invalid-radix>",
        "dec", // 10
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "hex", // 16
    };
    if (uint16_t(radix) > uint16_t(Radix::__HIGHEST__))
        throw std::runtime_error("radix out of range");
    return STRINGS[uint16_t(radix)];
}

std::string const &radix_name (Radix radix) {
    static std::string const NAMES[size_t(Radix::__HIGHEST__)+1] = {
        "<invalid-radix>", // 0
        "<invalid-radix>",
        "binary", // 2
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "octal", // 8
        "<invalid-radix>",
        "decimal", // 10
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "hexadecimal", // 16
    };
    if (uint16_t(radix) > uint16_t(Radix::__HIGHEST__))
        throw std::runtime_error("radix out of range");
    return NAMES[uint16_t(radix)];
}

std::string const &radix_prefix (Radix radix) {
    static std::string const PREFIXES[size_t(Radix::__HIGHEST__)+1] = {
        "<invalid-radix>", // 0
        "<invalid-radix>",
        "0b", // 2
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "0o", // 8
        "<invalid-radix>",
        "", // 10
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "0x", // 16
    };
    if (uint16_t(radix) > uint16_t(Radix::__HIGHEST__))
        throw std::runtime_error("radix out of range");
    return PREFIXES[uint16_t(radix)];
}

Radix cycled_radix (Radix radix) {
    switch (radix) {
        case Radix::BIN: return Radix::OCT;
        case Radix::OCT: return Radix::DEC;
        case Radix::DEC: return Radix::HEX;
        case Radix::HEX: return Radix::BIN;
        default: throw std::runtime_error("invalid Radix");
    }
}

//
// RadixFlags
//

std::vector<Radix> const RADIX_VECTOR{Radix::BIN, Radix::OCT, Radix::DEC, Radix::HEX};

std::string as_string (RadixFlags radix_flags) {
    if (radix_flags == RadixFlags::NONE)
        return "NONE";
    if ((radix_flags & RadixFlags::ANY) == RadixFlags::NONE)
        throw std::runtime_error("invalid RadixFlags (out of range)");

    auto radix_vector = as_radix_vector(radix_flags);
    return string_join("|", radix_vector);
}

Radix as_radix (RadixFlags single_radix_flag) {
    auto n = uint32_t(single_radix_flag);
    if (single_radix_flag == RadixFlags::NONE)
        throw std::runtime_error("single_radix_flag had no bits set");
    if ((n & (n-1)) != 0) // More than one bit was set.
        throw std::runtime_error("single_radix_flag had more than one bit set");
    if (n > uint32_t(RadixFlags::__HIGHEST__))
        throw std::runtime_error("invalid single_radix_flag -- it was greater than RadixFlags::__HIGHEST__");

    // This implementation depends on RadixFlags being (1 << uint32_t(radix)) for each radix value.
    double b = std::log2(n);
    assert(b == std::round(b) && "n was not an exact power of 2");
    return Radix(uint32_t(b));
}

std::vector<Radix> as_radix_vector (RadixFlags radix_flags) {
    std::vector<Radix> retval;
    for (auto radix : RADIX_VECTOR)
        if ((as_radix_flags(radix) & radix_flags) != RadixFlags::NONE)
            retval.push_back(radix);
    return retval;
}

//
// Parsing and rendering for uint64_t and int64_t
//

uint64_t string_view_to_uint64_raw (std::string_view const &str_view, Radix radix, uint64_t upper_bound) {
    static uint64_t const m = std::numeric_limits<uint64_t>::max();
    static uint64_t const DIGIT_VALUES[128] = {
        m,   m,   m,   m,     m,   m,   m, m,   m, m, m, m,   m, m, m, m,
        m,   m,   m,   m,     m,   m,   m, m,   m, m, m, m,   m, m, m, m,
        m,   m,   m,   m,     m,   m,   m, m,   m, m, m, m,   m, m, m, m,
        0,   1,   2,   3,     4,   5,   6, 7,   8, 9, m, m,   m, m, m, m,
        m, 0xA, 0xB, 0xC,   0xD, 0xE, 0xF, m,   m, m, m, m,   m, m, m, m,
        m,   m,   m,   m,     m,   m,   m, m,   m, m, m, m,   m, m, m, m,
        m, 0xa, 0xb, 0xc,   0xd, 0xe, 0xf, m,   m, m, m, m,   m, m, m, m,
        m,   m,   m,   m,     m,   m,   m, m,   m, m, m, m,   m, m, m, m,
    };

    auto trimmed_str = string_view_trimmed(str_view);
    if (trimmed_str.empty())
        throw std::runtime_error(LVD_FMT("ill-formed " << radix_name(radix) << " string (must not be empty)"));
    uint64_t accumulator = 0;
    for (auto c : trimmed_str) {
        if ((accumulator*uint64_t(radix))/uint64_t(radix) != accumulator)
            throw std::runtime_error(LVD_FMT("overflow in " << radix_name(radix) << " string"));
        if (uint8_t(c) >= 128)
            throw std::runtime_error(LVD_FMT("invalid digit in " << radix_name(radix) << " string (ascii code of digit was " << uint8_t(c) << ')'));
        uint64_t digit_value = DIGIT_VALUES[uint8_t(c)];
        if (digit_value == m)
            throw std::runtime_error(LVD_FMT("invalid digit in " << radix_name(radix) << " string (ascii code of digit was " << uint8_t(c) << ')'));
        if (digit_value >= uint64_t(radix))
            throw std::runtime_error(LVD_FMT("invalid digit '" << c << "' in " << radix_name(radix) << " string"));
        accumulator *= uint64_t(radix);
        if (accumulator+digit_value < accumulator)
            throw std::runtime_error(LVD_FMT("overflow in " << radix_name(radix) << " string"));
        accumulator += digit_value;
    }
    return accumulator;
}

std::string const &int64_min_string (Radix radix, UseRadixPrefix use_radix_prefix) {
    static std::string const MIN_STRINGS_WITH_PREFIX[17] = {
        "<invalid-radix>", // 0
        "<invalid-radix>",
        "-0b1"+std::string(63, '0'), // 2
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "-0o1"+std::string(21, '0'), // 8
        "<invalid-radix>",
        "-0d9223372036854775808", // 10
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "-0x8"+std::string(15, '0'), // 16
    };
    static std::string const MIN_STRINGS_WITHOUT_PREFIX[17] = {
        "<invalid-radix>", // 0
        "<invalid-radix>",
        "-1"+std::string(63, '0'), // 2
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "-1"+std::string(21, '0'), // 8
        "<invalid-radix>",
        "-9223372036854775808", // 10
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "<invalid-radix>",
        "-8"+std::string(15, '0'), // 16
    };
    if (uint16_t(radix) > uint16_t(Radix::__HIGHEST__))
        throw std::runtime_error("radix out of range");
    if (use_radix_prefix == UseRadixPrefix::YES)
        return MIN_STRINGS_WITH_PREFIX[uint16_t(radix)];
    else
        return MIN_STRINGS_WITHOUT_PREFIX[uint16_t(radix)];
}

// TODO: Think about making this into a class which has an operator<< override for ostream
std::string int64_to_string (int64_t value, Radix radix, UseRadixPrefix use_radix_prefix, SignOption sign_option) {
    if (value == std::numeric_limits<int64_t>::min())
        return int64_min_string(radix, use_radix_prefix);

    bool is_negative = value < 0;
    bool is_positive = value > 0;
    std::ostringstream out;
    if (is_negative) {
        // This works because the minimum value, whose negative is not representable in this
        // type, was handled above.
        value = -value;
    }
    static std::string const DIGITS{"0123456789ABCDEF"};
    // This produces the digits from least significant to most, which then have to be reversed.
    do {
        out << DIGITS[value % int64_t(radix)];
        value /= int64_t(radix);
    } while (value > 0);
    // If requested, stick on the radix indicator, in reverse.
    if (use_radix_prefix == UseRadixPrefix::YES)
        switch (radix) {
            case Radix::BIN: out << "b0"; break;
            case Radix::OCT: out << "o0"; break;
            case Radix::DEC: out << "d0"; break;
            case Radix::HEX: out << "x0"; break;
        }
    // Stick on the sign if needed.
    if (is_negative)
        out << '-';
    if (is_positive && sign_option == SignOption::ALWAYS_PRINT_SIGN)
        out << '+';
    // Reverse the string.
    std::string retval(out.str());
    std::reverse(retval.begin(), retval.end());
    return retval;
}

int64_t string_to_int64 (std::string const &str, RadixFlags allowed) {
    if (str.empty())
        throw std::runtime_error("can't parse int64 out of empty string");
    if (allowed == RadixFlags::NONE)
        throw std::runtime_error("must specify nonempty RadixFlags for allowed");

//     std::cerr << "string_to_int64; str = \"" << str << "\"\n";

    // TODO: make this initialize once as a static var
    std::regex regex("[[:space:]]*([+\\-]?)(0b[01]+|0o[0-7]+|0d[0-9]+|0x[0-9A-Fa-f]+|[1-9A-Fa-f][0-9A-Fa-f]*)[[:space:]]*", std::regex_constants::ECMAScript);
    std::smatch matches;
    if (!std::regex_match(str, matches, regex))
        throw std::runtime_error("ill-formed int64");
    // NOTE: For some reason, the first match is always the whole string (std::regex_match
    // docs say the number of matches is 1 plus the number of matched subexpressions).

//     std::cerr << "    " << REFLECT(matches.size()) << '\n';
//     for (auto const &match : matches) {
//         std::cerr << "        \"" << match.str() << "\"\n";
//     }
    assert(matches.size() == 3);
    assert(matches[0].str() == str); // The first match is always the whole string
    assert(matches[1].str().empty() || matches[1].str() == "+" || matches[1].str() == "-");
    assert(!matches[2].str().empty());

    bool is_negative = matches[1].str() == "-";
    size_t pos = 0;
    RadixFlags encountered = RadixFlags::NONE;
    Radix radix = Radix::BIN; // Arbitrary, will be set later.
    UseRadixPrefix radix_prefix_was_used = UseRadixPrefix::YES; // Arbitrary, will be set later.
    if (matches[2].str()[0] == '0' && matches[2].str().size() >= 2) {
        assert(matches[2].str().size() >= 2); // Should be true because the case of "0" was handled already.
        switch (matches[2].str()[1]) {
            case 'b': encountered = RadixFlags::BIN; radix = Radix::BIN; pos += 2; break;
            case 'o': encountered = RadixFlags::OCT; radix = Radix::OCT; pos += 2; break;
            case 'd': encountered = RadixFlags::DEC; radix = Radix::DEC; pos += 2; break;
            case 'x': encountered = RadixFlags::HEX; radix = Radix::HEX; pos += 2; break;
            default: throw std::runtime_error("ill-formed integer (expected radix specifier prefix '0b', '0o', or '0x')");
        }
        radix_prefix_was_used = UseRadixPrefix::YES;
    } else {
        // No prefix, which is only allowed if the allowed flags correponds to a single radix.
        if (!is_single_radix_flag(allowed))
            throw std::runtime_error("no radix prefix was present in the string, and more than one RadixFlags is present, so the parse is ambiguous");
        encountered = allowed;
        radix = as_radix(allowed);
        radix_prefix_was_used = UseRadixPrefix::NO;
        pos += 0;
    }
    assert(pos <= matches[2].str().size());

    if ((encountered & allowed) == RadixFlags::NONE)
        throw std::runtime_error(radix_name(radix) + " string encountered but was not an allowed radix");
    if (str == int64_min_string(radix, radix_prefix_was_used))
        return std::numeric_limits<int64_t>::min();

    // The upper bound enforces that it
    uint64_t upper_bound = uint64_t(std::numeric_limits<int64_t>::max());
    uint64_t raw = string_view_to_uint64_raw(std::string_view(&matches[2].str()[pos], matches[2].str().size()-pos), radix, upper_bound);
    assert(raw <= upper_bound);

    int64_t value = int64_t(raw);
    if (is_negative)
        value = -value;
    return value;
}

std::ostream &operator << (std::ostream &out, Tabs const &tabs)
{
    static uint32_t const TAB_SIZE_IN_SPACES = 4;
    return out << std::string(TAB_SIZE_IN_SPACES*tabs.m_count, ' ');
}

uint32_t newline_count (std::string const &text)
{
    uint32_t newline_count = 0;

    uint32_t pos = 0;
    while (pos < text.length())
        if (text[pos++] == '\n')
            ++newline_count;

    return newline_count;
}

///////////////////////////////////////////////////////////////////////////////

void emit_execution_message (std::ostream *out, std::string const &message)
{
    if (out == nullptr)
        return;

    *out << "message: " << message << std::endl;
    
//    if (GetOptions().IsVerbose(OptionsBase::V_EXECUTION))
//        *out << GetOptions().ProgramName() << ": " << message << std::endl;
}

void emit_warning (std::ostream *out, std::string const &message, FiLoc const &filoc)
{
    if (out == nullptr)
        return;

    if (filoc.is_valid())
        *out << "warning: " << filoc << ": " << message << std::endl;
    else
        *out << "warning: " << message << std::endl;
//    if (OptionsAreInitialized() && GetOptions().TreatWarningsAsErrors())
//        emit_error(out, message, filoc);
//    else if (filoc.is_valid())
//        *out << GetOptions().ProgramName() << ": " << filoc << ": warning: " << message << std::endl;
//    else
//        *out << GetOptions().ProgramName() << ": " << "warning: " << message << std::endl;
}

void emit_error (std::ostream *out, std::string const &message, FiLoc const &filoc)
{
    if (out == nullptr)
        return;

    if (filoc.is_valid())
        *out << "error: " << filoc << ": " << message << std::endl;
    else
        *out << "error: " << message << std::endl;
//    g_errors_encountered = true;
//    if (OptionsAreInitialized() && GetOptions().HaltOnFirstError())
//        EmitFatalError(message, filoc);
//    else
//    {
//        if (filoc.is_valid())
//            *out << GetOptions().ProgramName() << ": " << filoc << ": error: " << message << std::endl;
//        else
//            *out << GetOptions().ProgramName() << ": " << "error: " << message << std::endl;
//
//#if DEBUG
//        if (OptionsAreInitialized() && GetOptions().AssertOnError())
//            assert(false && "you have requested to assert on error, human, and here it is");
//#endif
//    }
}
} // end namespace lvd
