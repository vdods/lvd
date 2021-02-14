// 2021.02.13 - Copyright Victor Dods - Licensed under Apache 2.0

#include <any>
#include <functional>
#include "lvd/req.hpp"
#include "lvd/StaticAssociation_t.hpp"
#include "lvd/test.hpp"
#include <map>
#include "print.hpp"
#include <set>
#include <string>
#include <typeindex>
#include <unordered_map>

namespace lvd {

//
// Example of a plain std::map
//

struct FancyName1 { using Container = std::map<std::type_index,std::string>; };
#define FANCYNAME1_REGISTER(type, name) LVD_STATIC_ASSOCIATION_REGISTER(FancyName1, name, typeid(type), #name)

FANCYNAME1_REGISTER(uint32_t, FancyUint32)
FANCYNAME1_REGISTER(std::string, FancyString)
FANCYNAME1_REGISTER(bool, FancyBool)
// Uncommenting this should cause an exception to be thrown at init time.
// FANCYNAME1_REGISTER(bool, FancyBoolio)

struct SimpleName1 { using Container = std::map<std::type_index,std::string>; };
#define SIMPLENAME1_REGISTER(type, name) LVD_STATIC_ASSOCIATION_REGISTER(SimpleName1, name, typeid(type), #name)

SIMPLENAME1_REGISTER(uint16_t, SimpleUint16)
SIMPLENAME1_REGISTER(float, SimpleFloat)
SIMPLENAME1_REGISTER(bool, SimpleBool)

LVD_TEST_BEGIN(015__StaticAssociation_t__00)
    auto const &f = static_association_singleton<FancyName1>();
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(uint16_t)) == f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(uint32_t)) != f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(float)) == f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(std::string)) != f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(bool)) != f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(char)) == f.end());

    auto const &s = static_association_singleton<SimpleName1>();
    LVD_TEST_REQ_IS_TRUE(s.find(typeid(uint16_t)) != s.end());
    LVD_TEST_REQ_IS_TRUE(s.find(typeid(uint32_t)) == s.end());
    LVD_TEST_REQ_IS_TRUE(s.find(typeid(float)) != s.end());
    LVD_TEST_REQ_IS_TRUE(s.find(typeid(std::string)) == s.end());
    LVD_TEST_REQ_IS_TRUE(s.find(typeid(bool)) != s.end());
    LVD_TEST_REQ_IS_TRUE(s.find(typeid(char)) == s.end());

    test_log << Log::trc() << LVD_REFLECT(f) << '\n';
    test_log << Log::trc() << LVD_REFLECT(s) << '\n';
LVD_TEST_END

//
// Example of a plain std::set
//

LVD_STATIC_ASSOCIATION_DEFINE(FancyName2, std::set<std::type_index>)
#define FANCYNAME2_REGISTER(type, name) LVD_STATIC_ASSOCIATION_REGISTER(FancyName2, name, typeid(type))

FANCYNAME2_REGISTER(uint32_t, FancyUint32)
FANCYNAME2_REGISTER(std::string, FancyString)
FANCYNAME2_REGISTER(bool, FancyBool)

LVD_STATIC_ASSOCIATION_DEFINE(SimpleName2, std::set<std::type_index>)
#define SIMPLENAME2_REGISTER(type, name) LVD_STATIC_ASSOCIATION_REGISTER(SimpleName2, name, typeid(type))

SIMPLENAME2_REGISTER(uint16_t, SimpleUint16)
SIMPLENAME2_REGISTER(float, SimpleFloat)
SIMPLENAME2_REGISTER(bool, SimpleBool)

LVD_TEST_BEGIN(015__StaticAssociation_t__01)
    auto const &f = static_association_singleton<FancyName2>();
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(uint16_t)) == f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(uint32_t)) != f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(float)) == f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(std::string)) != f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(bool)) != f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(char)) == f.end());

    auto const &s = static_association_singleton<SimpleName2>();
    LVD_TEST_REQ_IS_TRUE(s.find(typeid(uint16_t)) != s.end());
    LVD_TEST_REQ_IS_TRUE(s.find(typeid(uint32_t)) == s.end());
    LVD_TEST_REQ_IS_TRUE(s.find(typeid(float)) != s.end());
    LVD_TEST_REQ_IS_TRUE(s.find(typeid(std::string)) == s.end());
    LVD_TEST_REQ_IS_TRUE(s.find(typeid(bool)) != s.end());
    LVD_TEST_REQ_IS_TRUE(s.find(typeid(char)) == s.end());

    test_log << Log::trc() << LVD_REFLECT(f) << '\n';
    test_log << Log::trc() << LVD_REFLECT(s) << '\n';
LVD_TEST_END

//
// Example of function overloading with run-time type resolution.
//

struct FancierFunc { using Container = std::unordered_map<std::type_index,std::function<std::string(std::any const &)>>; };
#define FANCIERFUNC_REGISTER(name, type, func) LVD_STATIC_ASSOCIATION_REGISTER(FancierFunc, name, typeid(type), func)

FANCIERFUNC_REGISTER(FancierUint32, uint32_t, [](std::any const &a){
    return LVD_FMT("hippo uint32_t(" << std::any_cast<uint32_t>(a) << ')');
})

FANCIERFUNC_REGISTER(FancierString, std::string, [](std::any const &a){
    return LVD_FMT("hippo std::string(" << std::any_cast<std::string>(a) << ')');
})

FANCIERFUNC_REGISTER(FancierBool, bool, [](std::any const &a){
    return LVD_FMT("hippo bool(" << std::boolalpha << std::any_cast<bool>(a) << ')');
})

// Convenience frontend function for calling the appropriate run-time-resolved function overload.
inline decltype(auto) fancier_func (std::any const &a) {
    return static_association_singleton<FancierFunc>().at(a.type())(a);
}

LVD_TEST_BEGIN(015__StaticAssociation_t__03)
    auto const &f = static_association_singleton<FancierFunc>();
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(uint16_t)) == f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(uint32_t)) != f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(float)) == f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(std::string)) != f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(bool)) != f.end());
    LVD_TEST_REQ_IS_TRUE(f.find(typeid(char)) == f.end());

    LVD_TEST_REQ_EQ(fancier_func(uint32_t(123)), "hippo uint32_t(123)");
    LVD_TEST_REQ_EQ(fancier_func(std::string("blah")), "hippo std::string(blah)");
    LVD_TEST_REQ_EQ(fancier_func(true), "hippo bool(true)");
LVD_TEST_END

} // end namespace lvd
