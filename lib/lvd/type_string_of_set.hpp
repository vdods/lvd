// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of.hpp"
#include <set>

namespace lvd {

template <typename Key_, typename... Rest_>
struct TypeString_t<std::set<Key_,Rest_...>> {
    static std::string const &get () {
        static std::string const STR{"set<" + type_string_of<Key_>() + '>'};
        return STR;
    }
};

} // end namespace lvd
