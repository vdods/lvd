// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/literal.hpp"
#include "lvd/random.hpp"
#include "lvd/repr.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include "print.hpp"
#include <sstream>
#include <string>

namespace lvd {

template <typename T_>
void hr_test_case (req::Context &req_context, T_ const &value, std::string const &expected_output) {
    std::ostringstream out;
    out << hr(value);
    req_context.log() << Log::trc() << out.str() << '\n';
    LVD_TEST_REQ_EQ(out.str(), expected_output);
}

LVD_TEST_BEGIN(230__From_t__00__hr)
    hr_test_case(req_context, std::string(""), "\"\"");
    hr_test_case(req_context, std::string("blahhhh"), "\"blahhhh\"");
    hr_test_case(req_context, true, "true");
    hr_test_case(req_context, false, "false");
    hr_test_case(req_context, int64_t(12345), "12345");
    hr_test_case(req_context, int32_t(12345), "12345");
    hr_test_case(req_context, 'a', "'a'");
    hr_test_case(req_context, std::byte(0xA7), "0xA7");
    hr_test_case(req_context, float(1.25), "1.250000000e+00");
    hr_test_case(req_context, std::pair('x',true), "('x', true)");
    hr_test_case(req_context, std::array<std::byte,3>{std::byte(0x12), std::byte(0x34), std::byte(0x56)}, "(0x12, 0x34, 0x56)");
    hr_test_case(req_context, std::vector<char>{'h', 'i', 'p', 'p', 'o'}, "('h', 'i', 'p', 'p', 'o')");
    hr_test_case(req_context, std::map<char,bool>{{'h',false}, {'i',true}, {'p',false}, {'p',false}, {'o',true}}, "(('h', false), ('i', true), ('o', true), ('p', false))");
    hr_test_case(req_context, std::set<char>{'h', 'i', 'p', 'p', 'o'}, "('h', 'i', 'o', 'p')");
LVD_TEST_END

template <typename T_>
void lit_test_case (req::Context &req_context, T_ const &value, std::string const &expected_output) {
    std::ostringstream out;
    out << lit(value);
    req_context.log() << Log::trc() << out.str() << '\n';
    LVD_TEST_REQ_EQ(out.str(), expected_output);
}

LVD_TEST_BEGIN(230__From_t__01__lit)
    lit_test_case(req_context, std::string(""), "string(\"\")");
    lit_test_case(req_context, std::string("blahhhh"), "string(\"blahhhh\")");
    lit_test_case(req_context, true, "bool(true)");
    lit_test_case(req_context, false, "bool(false)");
    lit_test_case(req_context, int64_t(12345), "int64_t(12345)");
    lit_test_case(req_context, int32_t(12345), "int32_t(12345)");
    lit_test_case(req_context, 'a', "char('a')");
    lit_test_case(req_context, std::byte(0xA7), "byte(0xA7)");
    lit_test_case(req_context, float(1.25), "float(1.250000000e+00)");
    lit_test_case(req_context, std::pair('x',true), "pair<char,bool>('x', true)");
    lit_test_case(req_context, std::array<std::byte,3>{std::byte(0x12), std::byte(0x34), std::byte(0x56)}, "array<byte,3>(0x12, 0x34, 0x56)");
    lit_test_case(req_context, std::vector<char>{'h', 'i', 'p', 'p', 'o'}, "vector<char>('h', 'i', 'p', 'p', 'o')");
    lit_test_case(req_context, std::map<char,bool>{{'h',false}, {'i',true}, {'p',false}, {'p',false}, {'o',true}}, "map<char,bool>(('h', false), ('i', true), ('o', true), ('p', false))");
    lit_test_case(req_context, std::set<char>{'h', 'i', 'p', 'p', 'o'}, "set<char>('h', 'i', 'o', 'p')");
LVD_TEST_END

template <typename T_>
void bin_test_case (req::Context &req_context, T_ const &value, std::string const &expected_output) {
    std::ostringstream out;
    out << bin(value);
    req_context.log() << Log::trc() << LVD_REFLECT(literal_of(expected_output)) << ", actual output = " << literal_of(out.str()) << '\n';
    LVD_TEST_REQ_EQ(out.str(), expected_output);
}

// TEMP TESTING STUFF
using ByteString = std::basic_string<std::byte>;

inline std::ostream &operator<< (std::ostream &out, ByteString const &bs) {
    out << "ByteString(";
    bool has_printed_element = false;
    for (auto c : bs) {
        if (has_printed_element)
            out << ' ';
        else
            has_printed_element = true;
        out << c;
    }
    return out << ')';
}

LVD_TEST_BEGIN(230__From_t__02__bin)
    // TEMP TESTING STUFF
    auto x = ByteString(reinterpret_cast<std::byte const *>("\xF0\xDE\xBC\x9A\x78\x56\x34\x12"), 8);
    test_log << Log::trc() << LVD_FMT(x) << '\n';

    // The weird constructions for expected_output std::string is because:
    // -    Constructing a string containing null chars requires also specifying the length of the string,
    // -    Apparently "\x00blah" parses as "\vlah", hence the concatenated string literals.
    //      See https://stackoverflow.com/questions/45612822/how-to-properly-add-hex-escapes-into-a-string-literal
    bin_test_case(req_context, std::string(""), std::string("\0\0\0\0\0\0\0\0", 8));
    bin_test_case(req_context, std::string("blahhhh"), std::string("\x07\0\0\0\0\0\0\0" "blahhhh", 15));
    bin_test_case(req_context, true, std::string("\x01", 1));
    bin_test_case(req_context, false, std::string("\0", 1));
    bin_test_case(req_context, 'a', "a");
    bin_test_case(req_context, std::byte(0xA7), "\xA7");
    bin_test_case(req_context, int8_t(0x12), std::string("\x12", 1));
    bin_test_case(req_context, uint8_t(0x12), std::string("\x12", 1));
    bin_test_case(req_context, int16_t(0x1234), std::string("\x34\x12", 2));
    bin_test_case(req_context, uint16_t(0x1234), std::string("\x34\x12", 2));
    bin_test_case(req_context, int32_t(0x12345678), std::string("\x78\x56\x34\x12", 4));
    bin_test_case(req_context, uint32_t(0x12345678), std::string("\x78\x56\x34\x12", 4));
    bin_test_case(req_context, int64_t(0x123456789ABCDEF0), std::string("\xF0\xDE\xBC\x9A\x78\x56\x34\x12", 8));
    bin_test_case(req_context, uint64_t(0x123456789ABCDEF0), std::string("\xF0\xDE\xBC\x9A\x78\x56\x34\x12", 8));
    bin_test_case(req_context, float(0.0f), std::string("\0\0\0\0", 4));
    bin_test_case(req_context, float(1.0f), std::string("\0\0\x80\x3F", 4));
    bin_test_case(req_context, float(-1.0f), std::string("\0\0\x80\xBF", 4));
    bin_test_case(req_context, double(0.0f), std::string("\0\0\0\0\0\0\0\0", 8));
    bin_test_case(req_context, double(1.0f), std::string("\0\0\0\0\0\0\xF0\x3F", 8));
    bin_test_case(req_context, double(-1.0f), std::string("\0\0\0\0\0\0\xF0\xBF", 8));
LVD_TEST_END

template <typename T_>
void bin_roundtrip_test_case (req::Context &req_context, T_ const &expected_value) {
    std::ostringstream out;
    out << bin(expected_value);
    req_context.log() << Log::trc() << "actual output = " << literal_of(out.str()) << '\n';

    T_ actual_value;
    std::istringstream in(out.str());
    in >> into_bin(actual_value);

    LVD_TEST_REQ_EQ(actual_value, expected_value);
}

LVD_TEST_BEGIN(230__From_t__03__bin_roundtrip)
    bin_roundtrip_test_case(req_context, std::string(""));
    bin_roundtrip_test_case(req_context, std::string("blahhhh"));
    bin_roundtrip_test_case(req_context, true);
    bin_roundtrip_test_case(req_context, false);
    bin_roundtrip_test_case(req_context, 'a');
    bin_roundtrip_test_case(req_context, std::byte(0xA7));
    bin_roundtrip_test_case(req_context, int8_t(0x12));
    bin_roundtrip_test_case(req_context, uint8_t(0x12));
    bin_roundtrip_test_case(req_context, int16_t(0x1234));
    bin_roundtrip_test_case(req_context, uint16_t(0x1234));
    bin_roundtrip_test_case(req_context, int32_t(0x12345678));
    bin_roundtrip_test_case(req_context, uint32_t(0x12345678));
    bin_roundtrip_test_case(req_context, int64_t(0x123456789ABCDEF0));
    bin_roundtrip_test_case(req_context, uint64_t(0x123456789ABCDEF0));
    bin_roundtrip_test_case(req_context, float(0.0f));
    bin_roundtrip_test_case(req_context, float(1.0f));
    bin_roundtrip_test_case(req_context, float(-1.0f));
    bin_roundtrip_test_case(req_context, double(0.0f));
    bin_roundtrip_test_case(req_context, double(1.0f));
    bin_roundtrip_test_case(req_context, double(-1.0f));
    bin_roundtrip_test_case(req_context, std::pair(-1.0f, 'a'));
    bin_roundtrip_test_case(req_context, std::vector<uint32_t>{});
    bin_roundtrip_test_case(req_context, std::vector<uint32_t>{1, 10, 100, 99});
    bin_roundtrip_test_case(req_context, std::array<char,3>{'a', 'x', 'z'});
LVD_TEST_END

template <typename T_>
void bin_roundtrip_test_case_random (req::Context &req_context) {
    auto rng = std::mt19937{42};
    for (int i = 0; i < 100; ++i) {
        auto expected_value = make_random<T_>(rng);

        {
            std::ostringstream out;
            out << bin(expected_value);
            req_context.log() << Log::trc() << "actual output = " << literal_of(out.str()) << '\n';

            T_ actual_value;
            std::istringstream in(out.str());
            in >> into_bin(actual_value);

            static_assert(std::is_same_v<decltype(expected_value),decltype(actual_value)>);
            req_context.log() << Log::trc() << LVD_REFLECT(type_v<decltype(expected_value)>) << ", " << LVD_REFLECT(type_v<decltype(actual_value)>) << '\n';
            req_context.log() << Log::trc() << LVD_REFLECT(expected_value) << ", " << LVD_REFLECT(actual_value) << '\n';

            LVD_TEST_REQ_EQ(actual_value, expected_value);
            req_context.log() << Log::trc() << "first test passed\n";
        }

        // Also test read_value.
        {
            std::ostringstream out;
            out << bin(expected_value);
            req_context.log() << Log::trc() << "actual output = " << literal_of(out.str()) << '\n';

            std::istringstream in(out.str());
            auto actual_value = read_bin<T_>(in, BinRepr::SINGLETON_LITTLE_ENDIAN);

            LVD_TEST_REQ_EQ(actual_value, expected_value);
            req_context.log() << Log::trc() << "second test passed\n";
        }
    }
}

LVD_TEST_BEGIN(230__From_t__04__bin_roundtrip_random)
    bin_roundtrip_test_case_random<bool>(req_context);
    bin_roundtrip_test_case_random<std::byte>(req_context);
    bin_roundtrip_test_case_random<char>(req_context);
    bin_roundtrip_test_case_random<int8_t>(req_context);
    bin_roundtrip_test_case_random<uint8_t>(req_context);
    bin_roundtrip_test_case_random<int16_t>(req_context);
    bin_roundtrip_test_case_random<uint16_t>(req_context);
    bin_roundtrip_test_case_random<int32_t>(req_context);
    bin_roundtrip_test_case_random<uint32_t>(req_context);
    bin_roundtrip_test_case_random<int64_t>(req_context);
    bin_roundtrip_test_case_random<uint64_t>(req_context);
    bin_roundtrip_test_case_random<float>(req_context);
    bin_roundtrip_test_case_random<double>(req_context);
    bin_roundtrip_test_case_random<std::string>(req_context);
    bin_roundtrip_test_case_random<std::pair<char,uint8_t>>(req_context);
    bin_roundtrip_test_case_random<std::array<float,4>>(req_context);
    bin_roundtrip_test_case_random<std::vector<std::byte>>(req_context);
    bin_roundtrip_test_case_random<std::map<char,double>>(req_context);
    bin_roundtrip_test_case_random<std::set<double>>(req_context);
    bin_roundtrip_test_case_random<std::unordered_map<std::string,int8_t>>(req_context);
    bin_roundtrip_test_case_random<std::unordered_set<int8_t>>(req_context);
LVD_TEST_END

template <typename T_, TypeEncoding TYPE_ENCODING_>
void repr_nice_syntax_roundtrip_random_bin_test_case (req::Context &req_context, BinRepr_t<TYPE_ENCODING_> const &bin) {
    auto rng = std::mt19937{42};
    for (int i = 0; i < 100; ++i) {
        auto expected_value = make_random<T_>(rng);

        {
            std::ostringstream out;
            out << bin.out(expected_value);
            req_context.log() << Log::trc() << "actual output = " << literal_of(out.str()) << '\n';

            T_ actual_value;
            std::istringstream in(out.str());
            in >> bin.in(actual_value);

            static_assert(std::is_same_v<decltype(expected_value),decltype(actual_value)>);
            req_context.log() << Log::trc() << LVD_REFLECT(type_v<decltype(expected_value)>) << ", " << LVD_REFLECT(type_v<decltype(actual_value)>) << '\n';
            req_context.log() << Log::trc() << LVD_REFLECT(expected_value) << ", " << LVD_REFLECT(actual_value) << '\n';

            LVD_TEST_REQ_EQ(actual_value, expected_value);
            req_context.log() << Log::trc() << "first test passed\n";
        }

        // Also test bin.read.
        {
            std::ostringstream out;
            out << bin.out(expected_value);
            req_context.log() << Log::trc() << "actual output = " << literal_of(out.str()) << '\n';

            std::istringstream in(out.str());
            auto actual_value = bin.template read<T_>(in);

            LVD_TEST_REQ_EQ(actual_value, expected_value);
            req_context.log() << Log::trc() << "second test passed\n";
        }
    }
}

// template <typename T_>
// void repr_nice_syntax_roundtrip_random_test_case (req::Context &req_context) {
//     repr_nice_syntax_roundtrip_random_bin_test_case<T_>(req_context, BinRepr(Endianness::BIG));
//     repr_nice_syntax_roundtrip_random_bin_test_case<T_>(req_context, BinRepr(Endianness::LIL));
// }
//
// LVD_TEST_BEGIN(230__From_t__05__repr_nice_syntax_roundtrip_random)
//     repr_nice_syntax_roundtrip_random_test_case<bool>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<std::byte>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<char>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<int8_t>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<uint8_t>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<int16_t>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<uint16_t>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<int32_t>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<uint32_t>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<int64_t>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<uint64_t>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<float>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<double>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<std::string>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<std::pair<char,uint8_t>>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<std::array<float,4>>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<std::vector<std::byte>>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<std::map<char,double>>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<std::set<double>>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<std::unordered_map<std::string,int8_t>>(req_context);
//     repr_nice_syntax_roundtrip_random_test_case<std::unordered_set<int8_t>>(req_context);
// LVD_TEST_END

} // end namespace lvd
