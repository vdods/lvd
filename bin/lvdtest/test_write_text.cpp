// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/literal.hpp"
#include "lvd/random.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include "lvd/write_text_array.hpp"
#include "lvd/write_text_map.hpp"
#include "lvd/write_text_pair.hpp"
#include "lvd/write_text_set.hpp"
#include "lvd/write_text_string.hpp"
#include "lvd/write_text_tuple.hpp"
#include "lvd/write_text_type.hpp"
#include "lvd/write_text_unordered_map.hpp"
#include "lvd/write_text_unordered_set.hpp"
#include "lvd/write_text_vector.hpp"
#include "lvd/type.hpp"
#include "print.hpp"
#include <sstream>
#include <string>

namespace lvd {

template <typename T_>
void txt_test_case (req::Context &req_context, T_ const &value, std::string const &expected_output) {
    std::ostringstream out;
    out << txt.out(value);
    req_context.log() << Log::trc() << out.str() << '\n';
    LVD_TEST_REQ_EQ(out.str(), expected_output);
}

LVD_TEST_BEGIN(232__write_text__00__txt)
    txt_test_case(req_context, true, "true");
    txt_test_case(req_context, false, "false");
    txt_test_case(req_context, int64_t(12345), "12345");
    txt_test_case(req_context, int32_t(12345), "12345");
    txt_test_case(req_context, 'a', "'a'");
    txt_test_case(req_context, std::byte(0xA7), "0xA7");
    txt_test_case(req_context, float(1.25), "1.250000000e+00");
    txt_test_case(req_context, std::string(""), "\"\"");
    txt_test_case(req_context, std::string("blahhhh"), "\"blahhhh\"");
    txt_test_case(req_context, ty<int32_t>, "int32_t");
    txt_test_case(req_context, ty<std::string>, "string");
    txt_test_case(req_context, ty<std::vector<std::map<std::byte,bool>>>, "vector<map<byte,bool>>");
    txt_test_case(req_context, std::pair('x',true), "('x', true)");
    txt_test_case(req_context, std::tuple<>(), "()");
    txt_test_case(req_context, std::tuple(int8_t(123)), "(123,)");
    txt_test_case(req_context, std::tuple(true,int8_t(123)), "(true, 123)");
    txt_test_case(req_context, std::tuple('x',true,int8_t(123)), "('x', true, 123)");
    txt_test_case(req_context, std::array<std::byte,3>{std::byte(0x12), std::byte(0x34), std::byte(0x56)}, "(0x12, 0x34, 0x56)");
    txt_test_case(req_context, std::vector<char>{'h', 'i', 'p', 'p', 'o'}, "('h', 'i', 'p', 'p', 'o')");
    txt_test_case(req_context, std::map<char,bool>{{'h',false}, {'i',true}, {'p',false}, {'p',false}, {'o',true}}, "(('h', false), ('i', true), ('o', true), ('p', false))");
    txt_test_case(req_context, std::set<char>{'h', 'i', 'p', 'p', 'o'}, "('h', 'i', 'o', 'p')");
LVD_TEST_END

template <typename T_>
void lit_test_case (req::Context &req_context, T_ const &value, std::string const &expected_output) {
    std::ostringstream out;
    out << lit.out(value);
    req_context.log() << Log::trc() << out.str() << '\n';
    LVD_TEST_REQ_EQ(out.str(), expected_output);
}

LVD_TEST_BEGIN(232__write_text__01__lit)
    lit_test_case(req_context, true, "bool(true)");
    lit_test_case(req_context, false, "bool(false)");
    lit_test_case(req_context, int64_t(12345), "int64_t(12345)");
    lit_test_case(req_context, int32_t(12345), "int32_t(12345)");
    lit_test_case(req_context, 'a', "char('a')");
    lit_test_case(req_context, std::byte(0xA7), "byte(0xA7)");
    lit_test_case(req_context, float(1.25), "float(1.250000000e+00)");
    lit_test_case(req_context, std::string(""), "string(\"\")");
    lit_test_case(req_context, std::string("blahhhh"), "string(\"blahhhh\")");
    lit_test_case(req_context, ty<int32_t>, "type(int32_t)");
    lit_test_case(req_context, ty<std::string>, "type(string)");
    lit_test_case(req_context, ty<std::vector<std::map<std::byte,bool>>>, "type(vector<map<byte,bool>>)");
    lit_test_case(req_context, std::pair('x',true), "pair<char,bool>('x', true)");
    lit_test_case(req_context, std::tuple<>(), "tuple<>()");
    lit_test_case(req_context, std::tuple(int8_t(123)), "tuple<int8_t>(123,)");
    lit_test_case(req_context, std::tuple(true,int8_t(123)), "tuple<bool,int8_t>(true, 123)");
    lit_test_case(req_context, std::tuple('x',true,int8_t(123)), "tuple<char,bool,int8_t>('x', true, 123)");
    lit_test_case(req_context, std::array<std::byte,3>{std::byte(0x12), std::byte(0x34), std::byte(0x56)}, "array<byte,3>(0x12, 0x34, 0x56)");
    lit_test_case(req_context, std::vector<char>{'h', 'i', 'p', 'p', 'o'}, "vector<char>('h', 'i', 'p', 'p', 'o')");
    lit_test_case(req_context, std::map<char,bool>{{'h',false}, {'i',true}, {'p',false}, {'p',false}, {'o',true}}, "map<char,bool>(('h', false), ('i', true), ('o', true), ('p', false))");
    lit_test_case(req_context, std::set<char>{'h', 'i', 'p', 'p', 'o'}, "set<char>('h', 'i', 'o', 'p')");
LVD_TEST_END

} // end namespace lvd
