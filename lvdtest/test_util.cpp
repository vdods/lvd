// 2020.04.01 - Victor Dods

#include "lvd/req.hpp"
#include "lvd/Test.hpp"
#include "lvd/util.hpp"

LVD_REGISTER_TEST(100__util__remainder, ([](){
    LVD_REQ_EQ(lvd::remainder(0, 1), size_t(0));
    LVD_REQ_EQ(lvd::remainder(0, 2), size_t(0));
    LVD_REQ_EQ(lvd::remainder(0, 3), size_t(0));

    LVD_REQ_EQ(lvd::remainder(-5, 5), size_t(0));
    LVD_REQ_EQ(lvd::remainder(-4, 5), size_t(1));
    LVD_REQ_EQ(lvd::remainder(-3, 5), size_t(2));
    LVD_REQ_EQ(lvd::remainder(-2, 5), size_t(3));
    LVD_REQ_EQ(lvd::remainder(-1, 5), size_t(4));
    LVD_REQ_EQ(lvd::remainder(0, 5),  size_t(0));
    LVD_REQ_EQ(lvd::remainder(1, 5),  size_t(1));
    LVD_REQ_EQ(lvd::remainder(2, 5),  size_t(2));
    LVD_REQ_EQ(lvd::remainder(3, 5),  size_t(3));
    LVD_REQ_EQ(lvd::remainder(4, 5),  size_t(4));
    LVD_REQ_EQ(lvd::remainder(5, 5),  size_t(0));

    LVD_REQ_EQ(lvd::remainder(7, 1),  size_t(0));
    LVD_REQ_EQ(lvd::remainder(7, 2),  size_t(1));
    LVD_REQ_EQ(lvd::remainder(7, 3),  size_t(1));
    LVD_REQ_EQ(lvd::remainder(7, 4),  size_t(3));
    LVD_REQ_EQ(lvd::remainder(7, 5),  size_t(2));
    LVD_REQ_EQ(lvd::remainder(7, 6),  size_t(1));
    LVD_REQ_EQ(lvd::remainder(7, 7),  size_t(0));
    LVD_REQ_EQ(lvd::remainder(7, 8),  size_t(7));
    LVD_REQ_EQ(lvd::remainder(7, 9),  size_t(7));

    LVD_REQ_EQ(lvd::remainder(-7, 1), size_t(0));
    LVD_REQ_EQ(lvd::remainder(-7, 2), size_t(1));
    LVD_REQ_EQ(lvd::remainder(-7, 3), size_t(2));
    LVD_REQ_EQ(lvd::remainder(-7, 4), size_t(1));
    LVD_REQ_EQ(lvd::remainder(-7, 5), size_t(3));
    LVD_REQ_EQ(lvd::remainder(-7, 6), size_t(5));
    LVD_REQ_EQ(lvd::remainder(-7, 7), size_t(0));
    LVD_REQ_EQ(lvd::remainder(-7, 8), size_t(1));
    LVD_REQ_EQ(lvd::remainder(-7, 9), size_t(2));

    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 1), size_t(0));
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 2), size_t(1));
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 3), size_t(1));
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 4), size_t(3));
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 5), size_t(2));
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 6), size_t(1));
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 7), size_t(0));
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 8), size_t(7));
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 9), size_t(7));
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 9223372036854775805ull), size_t(2));
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 9223372036854775806ull), size_t(1));
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 9223372036854775807ull), size_t(0));
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 9223372036854775808ull), 9223372036854775807ull);
    LVD_REQ_EQ(lvd::remainder(9223372036854775807ll, 9223372036854775809ull), 9223372036854775807ull);

    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 1), size_t(0));
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 2), size_t(0));
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 3), size_t(1));
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 4), size_t(0));
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 5), size_t(2));
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 6), size_t(4));
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 7), size_t(6));
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 8), size_t(0));
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 9), size_t(1));
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 9223372036854775805ull), 9223372036854775802ull);
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 9223372036854775806ull), 9223372036854775804ull);
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 9223372036854775807ull), 9223372036854775806ull);
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 9223372036854775808ull), size_t(0));
    LVD_REQ_EQ(lvd::remainder(std::numeric_limits<int64_t>::min(), 9223372036854775809ull), size_t(1));
}));

LVD_REGISTER_TEST(100__util__as_radix_flags, ([](){
    LVD_REQ_EQ(lvd::as_radix_flags(lvd::Radix::BIN), lvd::RadixFlags::BIN);
    LVD_REQ_EQ(lvd::as_radix_flags(lvd::Radix::OCT), lvd::RadixFlags::OCT);
    LVD_REQ_EQ(lvd::as_radix_flags(lvd::Radix::DEC), lvd::RadixFlags::DEC);
    LVD_REQ_EQ(lvd::as_radix_flags(lvd::Radix::HEX), lvd::RadixFlags::HEX);
}));

LVD_REGISTER_TEST(100__util__as_radix, ([](){
    LVD_REQ_EQ(lvd::as_radix(lvd::RadixFlags::BIN), lvd::Radix::BIN);
    LVD_REQ_EQ(lvd::as_radix(lvd::RadixFlags::OCT), lvd::Radix::OCT);
    LVD_REQ_EQ(lvd::as_radix(lvd::RadixFlags::DEC), lvd::Radix::DEC);
    LVD_REQ_EQ(lvd::as_radix(lvd::RadixFlags::HEX), lvd::Radix::HEX);
}));

LVD_REGISTER_TEST(100__util__string_to_uint64_raw__bin, ([](){
    LVD_REQ_EQ(lvd::string_to_uint64_raw("0", lvd::Radix::BIN),  uint64_t(0));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("1", lvd::Radix::BIN),  uint64_t(1));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("10", lvd::Radix::BIN), uint64_t(2));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("11", lvd::Radix::BIN), uint64_t(3));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("1111111111111111111111111111111111111111111111111111111111111111", lvd::Radix::BIN), std::numeric_limits<uint64_t>::max());

    LVD_REQ_EQ(lvd::string_to_uint64_raw(" 0", lvd::Radix::BIN),        uint64_t(0));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("\t1", lvd::Radix::BIN),       uint64_t(1));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("\n10", lvd::Radix::BIN),      uint64_t(2));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("11 ", lvd::Radix::BIN),       uint64_t(3));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("11\t", lvd::Radix::BIN),      uint64_t(3));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("11\n", lvd::Radix::BIN),      uint64_t(3));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("    11   ", lvd::Radix::BIN), uint64_t(3));

    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("", lvd::Radix::BIN); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw(" ", lvd::Radix::BIN); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("\t", lvd::Radix::BIN); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("\t\n   ", lvd::Radix::BIN); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("2", lvd::Radix::BIN); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("3", lvd::Radix::BIN); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("11000101011100x", lvd::Radix::BIN); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("10000000000000000000000000000000000000000000000000000000000000000", lvd::Radix::BIN); // Overflow -- this is 2^64
    });
}));

LVD_REGISTER_TEST(100__util__string_to_uint64_raw__oct, ([](){
    LVD_REQ_EQ(lvd::string_to_uint64_raw("0", lvd::Radix::OCT),   uint64_t(0));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("1", lvd::Radix::OCT),   uint64_t(1));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("7", lvd::Radix::OCT),   uint64_t(7));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("10", lvd::Radix::OCT),  uint64_t(8));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("11", lvd::Radix::OCT),  uint64_t(9));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("100", lvd::Radix::OCT), uint64_t(64));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("1777777777777777777777", lvd::Radix::OCT), std::numeric_limits<uint64_t>::max());

    LVD_REQ_EQ(lvd::string_to_uint64_raw(" 0", lvd::Radix::OCT),         uint64_t(0));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("\t1", lvd::Radix::OCT),        uint64_t(1));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("\n7", lvd::Radix::OCT),        uint64_t(7));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("10 ", lvd::Radix::OCT),        uint64_t(8));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("11\t", lvd::Radix::OCT),       uint64_t(9));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("100\n", lvd::Radix::OCT),      uint64_t(64));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("    100   ", lvd::Radix::OCT), uint64_t(64));

    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("", lvd::Radix::OCT); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw(" ", lvd::Radix::OCT); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("\t", lvd::Radix::OCT); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("\t\n   ", lvd::Radix::OCT); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("8", lvd::Radix::OCT); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("9", lvd::Radix::OCT); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("1234567x", lvd::Radix::OCT); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("2000000000000000000000", lvd::Radix::OCT); // Overflow -- this is 2^64
    });
}));

LVD_REGISTER_TEST(100__util__string_to_uint64_raw__dec, ([](){
    LVD_REQ_EQ(lvd::string_to_uint64_raw("0", lvd::Radix::DEC),   uint64_t(0));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("1", lvd::Radix::DEC),   uint64_t(1));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("7", lvd::Radix::DEC),   uint64_t(7));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("8", lvd::Radix::DEC),   uint64_t(8));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("9", lvd::Radix::DEC),   uint64_t(9));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("10", lvd::Radix::DEC),  uint64_t(10));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("11", lvd::Radix::DEC),  uint64_t(11));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("100", lvd::Radix::DEC), uint64_t(100));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("18446744073709551615", lvd::Radix::DEC), std::numeric_limits<uint64_t>::max());

    LVD_REQ_EQ(lvd::string_to_uint64_raw(" 0", lvd::Radix::DEC),         uint64_t(0));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("\t1", lvd::Radix::DEC),        uint64_t(1));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("\n7", lvd::Radix::DEC),        uint64_t(7));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("10 ", lvd::Radix::DEC),        uint64_t(10));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("11\t", lvd::Radix::DEC),       uint64_t(11));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("100\n", lvd::Radix::DEC),      uint64_t(100));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("    100   ", lvd::Radix::DEC), uint64_t(100));

    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("", lvd::Radix::DEC); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw(" ", lvd::Radix::DEC); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("\t", lvd::Radix::DEC); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("\t\n   ", lvd::Radix::DEC); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("A", lvd::Radix::DEC); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("a", lvd::Radix::DEC); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        uint64_t v = lvd::string_to_uint64_raw("1234567890x", lvd::Radix::DEC); // Invalid digit
        std::cerr << "v = " << v << '\n';
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        uint64_t w = lvd::string_to_uint64_raw("18446744073709551616", lvd::Radix::DEC); // Overflow -- this is 2^64
        std::cerr << "w = " << w << '\n';
    });
}));

LVD_REGISTER_TEST(100__util__string_to_uint64_raw__hex, ([](){
    LVD_REQ_EQ(lvd::string_to_uint64_raw("0", lvd::Radix::HEX),   uint64_t(0));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("1", lvd::Radix::HEX),   uint64_t(1));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("7", lvd::Radix::HEX),   uint64_t(7));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("8", lvd::Radix::HEX),   uint64_t(8));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("9", lvd::Radix::HEX),   uint64_t(9));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("A", lvd::Radix::HEX),   uint64_t(0xA));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("a", lvd::Radix::HEX),   uint64_t(0xA));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("B", lvd::Radix::HEX),   uint64_t(0xB));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("b", lvd::Radix::HEX),   uint64_t(0xB));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("C", lvd::Radix::HEX),   uint64_t(0xC));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("c", lvd::Radix::HEX),   uint64_t(0xC));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("D", lvd::Radix::HEX),   uint64_t(0xD));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("d", lvd::Radix::HEX),   uint64_t(0xD));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("E", lvd::Radix::HEX),   uint64_t(0xE));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("e", lvd::Radix::HEX),   uint64_t(0xE));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("F", lvd::Radix::HEX),   uint64_t(0xF));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("f", lvd::Radix::HEX),   uint64_t(0xF));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("10", lvd::Radix::HEX),  uint64_t(16));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("11", lvd::Radix::HEX),  uint64_t(17));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("100", lvd::Radix::HEX), uint64_t(256));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("FFFFFFFFFFFFFFFF", lvd::Radix::HEX), std::numeric_limits<uint64_t>::max());

    LVD_REQ_EQ(lvd::string_to_uint64_raw(" 0", lvd::Radix::HEX),         uint64_t(0));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("\t1", lvd::Radix::HEX),        uint64_t(1));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("\n7", lvd::Radix::HEX),        uint64_t(7));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("10 ", lvd::Radix::HEX),        uint64_t(16));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("11\t", lvd::Radix::HEX),       uint64_t(17));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("100\n", lvd::Radix::HEX),      uint64_t(256));
    LVD_REQ_EQ(lvd::string_to_uint64_raw("    100   ", lvd::Radix::HEX), uint64_t(256));

    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("", lvd::Radix::HEX); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw(" ", lvd::Radix::HEX); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("\t", lvd::Radix::HEX); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("\t\n   ", lvd::Radix::HEX); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("G", lvd::Radix::HEX); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("g", lvd::Radix::HEX); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("1234567890ABCDEFx", lvd::Radix::HEX); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_uint64_raw("0x10000000000000000", lvd::Radix::HEX); // Overflow -- this is 2^64
    });
}));

LVD_REGISTER_TEST(100__util__int64_to_string, ([](){
    LVD_REQ_EQ(lvd::int64_to_string(0, lvd::Radix::BIN, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "0b0");
    LVD_REQ_EQ(lvd::int64_to_string(0, lvd::Radix::OCT, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "0o0");
    LVD_REQ_EQ(lvd::int64_to_string(0, lvd::Radix::DEC, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "0d0");
    LVD_REQ_EQ(lvd::int64_to_string(0, lvd::Radix::HEX, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "0x0");

    LVD_REQ_EQ(lvd::int64_to_string(2, lvd::Radix::BIN, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "+0b10");
    LVD_REQ_EQ(lvd::int64_to_string(2, lvd::Radix::OCT, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "+0o2");
    LVD_REQ_EQ(lvd::int64_to_string(2, lvd::Radix::DEC, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "+0d2");
    LVD_REQ_EQ(lvd::int64_to_string(2, lvd::Radix::HEX, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "+0x2");

    LVD_REQ_EQ(lvd::int64_to_string(-2, lvd::Radix::BIN, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "-0b10");
    LVD_REQ_EQ(lvd::int64_to_string(-2, lvd::Radix::OCT, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "-0o2");
    LVD_REQ_EQ(lvd::int64_to_string(-2, lvd::Radix::DEC, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "-0d2");
    LVD_REQ_EQ(lvd::int64_to_string(-2, lvd::Radix::HEX, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "-0x2");

    LVD_REQ_EQ(lvd::int64_to_string(31, lvd::Radix::BIN, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "+0b11111");
    LVD_REQ_EQ(lvd::int64_to_string(31, lvd::Radix::OCT, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "+0o37");
    LVD_REQ_EQ(lvd::int64_to_string(31, lvd::Radix::DEC, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "+0d31");
    LVD_REQ_EQ(lvd::int64_to_string(31, lvd::Radix::HEX, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "+0x1F");

    LVD_REQ_EQ(lvd::int64_to_string(-31, lvd::Radix::BIN, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "-0b11111");
    LVD_REQ_EQ(lvd::int64_to_string(-31, lvd::Radix::OCT, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "-0o37");
    LVD_REQ_EQ(lvd::int64_to_string(-31, lvd::Radix::DEC, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "-0d31");
    LVD_REQ_EQ(lvd::int64_to_string(-31, lvd::Radix::HEX, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "-0x1F");

    LVD_REQ_EQ(lvd::int64_to_string(std::numeric_limits<int64_t>::min(), lvd::Radix::BIN, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "-0b1000000000000000000000000000000000000000000000000000000000000000");
    LVD_REQ_EQ(lvd::int64_to_string(std::numeric_limits<int64_t>::min(), lvd::Radix::OCT, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "-0o1000000000000000000000");
    LVD_REQ_EQ(lvd::int64_to_string(std::numeric_limits<int64_t>::min(), lvd::Radix::DEC, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "-0d9223372036854775808");
    LVD_REQ_EQ(lvd::int64_to_string(std::numeric_limits<int64_t>::min(), lvd::Radix::HEX, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "-0x8000000000000000");

    LVD_REQ_EQ(lvd::int64_to_string(std::numeric_limits<int64_t>::max(), lvd::Radix::BIN, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "+0b111111111111111111111111111111111111111111111111111111111111111");
    LVD_REQ_EQ(lvd::int64_to_string(std::numeric_limits<int64_t>::max(), lvd::Radix::OCT, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "+0o777777777777777777777");
    LVD_REQ_EQ(lvd::int64_to_string(std::numeric_limits<int64_t>::max(), lvd::Radix::DEC, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "+0d9223372036854775807");
    LVD_REQ_EQ(lvd::int64_to_string(std::numeric_limits<int64_t>::max(), lvd::Radix::HEX, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), "+0x7FFFFFFFFFFFFFFF");
}));

LVD_REGISTER_TEST(100__util__string_to_int64, ([](){
    LVD_REQ_EQ(lvd::string_to_int64("0b0", lvd::RadixFlags::BIN), 0);
    LVD_REQ_EQ(lvd::string_to_int64("+0b1", lvd::RadixFlags::BIN), 1);
    LVD_REQ_EQ(lvd::string_to_int64("-0b1", lvd::RadixFlags::BIN), -1);
    LVD_REQ_EQ(lvd::string_to_int64("+0b10", lvd::RadixFlags::BIN), 2);
    LVD_REQ_EQ(lvd::string_to_int64("-0b10", lvd::RadixFlags::BIN), -2);
    LVD_REQ_EQ(lvd::string_to_int64("+0b11", lvd::RadixFlags::BIN), 3);
    LVD_REQ_EQ(lvd::string_to_int64("-0b11", lvd::RadixFlags::BIN), -3);
    // This is zero and then 63 ones
    LVD_REQ_EQ(lvd::string_to_int64("+0b0111111111111111111111111111111111111111111111111111111111111111", lvd::RadixFlags::BIN), std::numeric_limits<int64_t>::max());
    LVD_REQ_EQ(lvd::string_to_int64("-0b0111111111111111111111111111111111111111111111111111111111111111", lvd::RadixFlags::BIN), -std::numeric_limits<int64_t>::max());
    // This is one and then 63 zeros
    LVD_REQ_EQ(lvd::string_to_int64("-0b1000000000000000000000000000000000000000000000000000000000000000", lvd::RadixFlags::BIN), std::numeric_limits<int64_t>::min());

    LVD_REQ_EQ(lvd::string_to_int64(" 0b0", lvd::RadixFlags::BIN), 0);
    LVD_REQ_EQ(lvd::string_to_int64(" +0b1", lvd::RadixFlags::BIN), 1);
    LVD_REQ_EQ(lvd::string_to_int64(" -0b1", lvd::RadixFlags::BIN), -1);
    LVD_REQ_EQ(lvd::string_to_int64("\t+0b1", lvd::RadixFlags::BIN), 1);
    LVD_REQ_EQ(lvd::string_to_int64("\t-0b1", lvd::RadixFlags::BIN), -1);
    LVD_REQ_EQ(lvd::string_to_int64("\n+0b10", lvd::RadixFlags::BIN), 2);
    LVD_REQ_EQ(lvd::string_to_int64("\n-0b10", lvd::RadixFlags::BIN), -2);
    LVD_REQ_EQ(lvd::string_to_int64("+0b11 ", lvd::RadixFlags::BIN), 3);
    LVD_REQ_EQ(lvd::string_to_int64("-0b11 ", lvd::RadixFlags::BIN), -3);
    LVD_REQ_EQ(lvd::string_to_int64("+0b11\t", lvd::RadixFlags::BIN), 3);
    LVD_REQ_EQ(lvd::string_to_int64("-0b11\t", lvd::RadixFlags::BIN), -3);
    LVD_REQ_EQ(lvd::string_to_int64("+0b11\n", lvd::RadixFlags::BIN), 3);
    LVD_REQ_EQ(lvd::string_to_int64("-0b11\n", lvd::RadixFlags::BIN), -3);
    LVD_REQ_EQ(lvd::string_to_int64("    +0b11   ", lvd::RadixFlags::BIN), 3);
    LVD_REQ_EQ(lvd::string_to_int64("    -0b11   ", lvd::RadixFlags::BIN), -3);

    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_int64("", lvd::RadixFlags::BIN); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_int64(" ", lvd::RadixFlags::BIN); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_int64("\t", lvd::RadixFlags::BIN); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_int64("\t\n   ", lvd::RadixFlags::BIN); // Empty string
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_int64("+0b2", lvd::RadixFlags::BIN); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_int64("-0b2", lvd::RadixFlags::BIN); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_int64("+0b3", lvd::RadixFlags::BIN); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_int64("-0b3", lvd::RadixFlags::BIN); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_int64("+0b11000101011100x", lvd::RadixFlags::BIN); // Invalid digit
    });
    lvd::call_function_and_expect_exception<std::runtime_error>([](){
        lvd::string_to_int64("+0b10000000000000000000000000000000000000000000000000000000000000000", lvd::RadixFlags::BIN); // Overflow -- this is 2^64
    });
}));

LVD_REGISTER_TEST(100__util__int64_to_string_to_int64, ([](){
    std::vector<lvd::Radix> radices{lvd::Radix::BIN, lvd::Radix::OCT, lvd::Radix::DEC, lvd::Radix::HEX};

    for (auto radix : radices) {
        for (int64_t n = -33; n <= 33; ++n)
            LVD_REQ_EQ(lvd::string_to_int64(lvd::int64_to_string(n, radix, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), lvd::as_radix_flags(radix)), n);

        for (int64_t n = std::numeric_limits<int64_t>::min(); n <= std::numeric_limits<int64_t>::min()+33; ++n)
            LVD_REQ_EQ(lvd::string_to_int64(lvd::int64_to_string(n, radix, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), lvd::as_radix_flags(radix)), n);

        for (int64_t n = std::numeric_limits<int64_t>::max(); n >= std::numeric_limits<int64_t>::max()-33; --n)
            LVD_REQ_EQ(lvd::string_to_int64(lvd::int64_to_string(n, radix, lvd::UseRadixPrefix::YES, lvd::SignOption::ALWAYS_PRINT_SIGN), lvd::as_radix_flags(radix)), n);
    }
}));
