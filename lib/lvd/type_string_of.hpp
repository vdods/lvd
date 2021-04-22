// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <cstddef>
#include "lvd/fmt.hpp"
#include "lvd/type.hpp"
#include <ostream>
#include <string>

namespace lvd {

// TODO: Create VerboseType and TerseType helpers, which would come after T_ in TypeString_t<T_>, and:
// -    TerseType causes the generally useful type to be printed, e.g. std::vector<T_> (this is default)
// -    VerboseType causes the whole type to be printed, e.g. std::vector<T_,Allocator_> (not typically desired)

template <typename T_>
struct TypeString_t;

// Default implementation -- unknown type -- produce an intentionally ugly name that's still a valid C identifier.
// template <typename T_>
// struct TypeString_t {
//     static std::string const &get () {
//         static std::string const STR{"__UNKNOWN_TYPE__"};
//         return STR;
//     }
// };

template <>
struct TypeString_t<VariadicSequence_t<>> {
    static std::string const &get () {
        static std::string const STR{}; // empty string
        return STR;
    }
};

template <typename Only_>
struct TypeString_t<VariadicSequence_t<Only_>> : public TypeString_t<Only_> { };

template <typename First_, typename... Rest_>
struct TypeString_t<VariadicSequence_t<First_,Rest_...>> {
    static std::string const &get () {
        static std::string const STR{TypeString_t<First_>::get() + ',' + TypeString_t<VariadicSequence_t<Rest_...>>::get()};
        return STR;
    }
};

//
// Helper functions that do type deduction.
//

// NOTE: If you get a compile error to the effect of "incomplete type..." or if you get a type string
// of "__UNKNOWN_TYPE__" somewhere, then you need to include one of type_string_of_XXX.hpp where
// XXX is the desired type, or define TypeString_t<T> for your type T.
template <typename T_>
decltype(auto) type_string_of () {
    return TypeString_t<T_>::get();
}

// NOTE: If you get a compile error to the effect of "incomplete type..." or if you get a type string
// of "__UNKNOWN_TYPE__" somewhere, then you need to include one of type_string_of_XXX.hpp where
// XXX is the desired type, or define TypeString_t<T> for your type T.
template <typename T_>
decltype(auto) type_string_of (T_ const &) {
    return TypeString_t<T_>::get();
}

//
// Definition of how to print Type_t<T_>, which relies on TypeString_t<T_>.
//

template <typename T_>
std::ostream &operator<< (std::ostream &out, Type_t<T_> const &) {
    return out << TypeString_t<T_>::get();
}

template <typename T_>
struct TypeString_t<Type_t<T_>> {
    static std::string const &get () {
        return type_string_of<T_>();
    }
};

} // end namespace lvd

//
// Helper macros
//

#define LVD_DEFINE_TYPE_STRING(T) \
template <> \
struct lvd::TypeString_t<T> { \
    static std::string const &get () { \
        static std::string const STR{#T}; \
        return STR; \
    } \
};

#define LVD_DEFINE_TYPE_STRING_EXPLICIT(T, str) \
template <> \
struct lvd::TypeString_t<T> { \
    static std::string const &get () { \
        static std::string const STR{str}; \
        return STR; \
    } \
};

//
// Template specializations for some common types
//

LVD_DEFINE_TYPE_STRING(bool)
LVD_DEFINE_TYPE_STRING_EXPLICIT(std::byte, "byte")
LVD_DEFINE_TYPE_STRING(char)
LVD_DEFINE_TYPE_STRING(int8_t)
LVD_DEFINE_TYPE_STRING(uint8_t)
LVD_DEFINE_TYPE_STRING(int16_t)
LVD_DEFINE_TYPE_STRING(uint16_t)
LVD_DEFINE_TYPE_STRING(int32_t)
LVD_DEFINE_TYPE_STRING(uint32_t)
LVD_DEFINE_TYPE_STRING(int64_t)
LVD_DEFINE_TYPE_STRING(uint64_t)
LVD_DEFINE_TYPE_STRING(float)
LVD_DEFINE_TYPE_STRING(double)
LVD_DEFINE_TYPE_STRING_EXPLICIT(std::string, "string")
