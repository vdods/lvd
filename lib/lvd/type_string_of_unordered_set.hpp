// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of.hpp"
#include <unordered_set>

namespace lvd {

template <typename Key_, typename... Rest_>
struct TypeString_t<std::unordered_set<Key_,Rest_...>> {
    static std::string const &get () {
        static std::string const STR{"unordered_set<" + type_string_of<Key_>() + '>'};
        return STR;
    }
};

} // end namespace lvd
