// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include "lvd/type_string_of_array.hpp"
#include "lvd/type_string_of_map.hpp"
#include "lvd/type_string_of_pair.hpp"
#include "lvd/type_string_of_set.hpp"
#include "lvd/type_string_of_unordered_map.hpp"
#include "lvd/type_string_of_unordered_set.hpp"
#include "lvd/type_string_of_variant.hpp"
#include "lvd/type_string_of_vector.hpp"

namespace lvd {

LVD_TEST_BEGIN(006__TypeString_t__00)
    LVD_TEST_REQ_EQ(type_string_of<bool>(), "bool");
    LVD_TEST_REQ_EQ(type_string_of<std::byte>(), "byte");
    LVD_TEST_REQ_EQ(type_string_of<char>(), "char");
    LVD_TEST_REQ_EQ(type_string_of<int8_t>(), "int8_t");
    LVD_TEST_REQ_EQ(type_string_of<uint8_t>(), "uint8_t");
    LVD_TEST_REQ_EQ(type_string_of<int16_t>(), "int16_t");
    LVD_TEST_REQ_EQ(type_string_of<uint16_t>(), "uint16_t");
    LVD_TEST_REQ_EQ(type_string_of<int32_t>(), "int32_t");
    LVD_TEST_REQ_EQ(type_string_of<uint32_t>(), "uint32_t");
    LVD_TEST_REQ_EQ(type_string_of<int64_t>(), "int64_t");
    LVD_TEST_REQ_EQ(type_string_of<uint64_t>(), "uint64_t");
    LVD_TEST_REQ_EQ(type_string_of<float>(), "float");
    LVD_TEST_REQ_EQ(type_string_of<double>(), "double");
    LVD_TEST_REQ_EQ(type_string_of<std::string>(), "string");
    LVD_TEST_REQ_EQ(type_string_of<Type_t<std::byte>>(), "byte");
    LVD_TEST_REQ_EQ(type_string_of(ty<std::byte>), "byte");
    LVD_TEST_REQ_EQ((type_string_of<std::variant<uint32_t,std::string,bool>>()), "variant<uint32_t,string,bool>");
    LVD_TEST_REQ_EQ(type_string_of<std::vector<uint32_t>>(), "vector<uint32_t>");
    LVD_TEST_REQ_EQ((type_string_of<std::pair<uint32_t,std::string>>()), "pair<uint32_t,string>");
    LVD_TEST_REQ_EQ((type_string_of<std::map<uint32_t,bool>>()), "map<uint32_t,bool>");
    LVD_TEST_REQ_EQ((type_string_of<std::set<uint32_t>>()), "set<uint32_t>");
    LVD_TEST_REQ_EQ((type_string_of<std::unordered_map<uint32_t,bool>>()), "unordered_map<uint32_t,bool>");
    LVD_TEST_REQ_EQ((type_string_of<std::unordered_set<uint32_t>>()), "unordered_set<uint32_t>");

    LVD_TEST_REQ_EQ((type_string_of<std::vector<std::pair<std::map<int,bool>,std::set<float>>>>()), "vector<pair<map<int32_t,bool>,set<float>>>");
LVD_TEST_END

} // end namespace lvd
