// 2021.01.27 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/literal.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
// #include "print.hpp"
#include <sstream>
#include <string>

using namespace std::string_literals;

namespace lvd {

LVD_TEST_BEGIN(001__literal__00__literal_of__00__string)
    LVD_TEST_REQ_EQ(literal_of(""s), "\"\""s);
    LVD_TEST_REQ_EQ(literal_of("abc"s), "\"abc\""s);

    LVD_TEST_REQ_EQ(literal_of("\a"s), "\"\\a\""s);
    LVD_TEST_REQ_EQ(literal_of("\b"s), "\"\\b\""s);
    LVD_TEST_REQ_EQ(literal_of("\t"s), "\"\\t\""s);
    LVD_TEST_REQ_EQ(literal_of("\n"s), "\"\\n\""s);
    LVD_TEST_REQ_EQ(literal_of("\v"s), "\"\\v\""s);
    LVD_TEST_REQ_EQ(literal_of("\f"s), "\"\\f\""s);
    LVD_TEST_REQ_EQ(literal_of("\r"s), "\"\\r\""s);
    LVD_TEST_REQ_EQ(literal_of("\""s), "\"\\\"\""s);
    LVD_TEST_REQ_EQ(literal_of("\\"s), "\"\\\\\""s);

    LVD_TEST_REQ_EQ(literal_of("\ax"s), "\"\\ax\""s);
    LVD_TEST_REQ_EQ(literal_of("\bx"s), "\"\\bx\""s);
    LVD_TEST_REQ_EQ(literal_of("\tx"s), "\"\\tx\""s);
    LVD_TEST_REQ_EQ(literal_of("\nx"s), "\"\\nx\""s);
    LVD_TEST_REQ_EQ(literal_of("\vx"s), "\"\\vx\""s);
    LVD_TEST_REQ_EQ(literal_of("\fx"s), "\"\\fx\""s);
    LVD_TEST_REQ_EQ(literal_of("\rx"s), "\"\\rx\""s);
    LVD_TEST_REQ_EQ(literal_of("\"x"s), "\"\\\"x\""s);
    LVD_TEST_REQ_EQ(literal_of("\\x"s), "\"\\\\x\""s);

    LVD_TEST_REQ_EQ(literal_of("\000"s), "\"\\000\""s);
    LVD_TEST_REQ_EQ(literal_of("\000\001"s), "\"\\000\\001\""s);
    LVD_TEST_REQ_EQ(literal_of("\000\001002"s), "\"\\000\\001002\""s);
    LVD_TEST_REQ_EQ(literal_of("abc\n"s), "\"abc\\n\""s);
    LVD_TEST_REQ_EQ(literal_of("abc\000xyz"s), "\"abc\\000xyz\""s);
    LVD_TEST_REQ_EQ(literal_of("abc\0001yz"s), "\"abc\\0001yz\""s);
    LVD_TEST_REQ_EQ(literal_of("abc\0321yz"s), "\"abc\\0321yz\""s);
LVD_TEST_END

} // end namespace lvd
