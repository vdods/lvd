// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of.hpp"
#include <map>

namespace lvd {

template <typename Key_, typename Value_, typename... Rest_>
struct TypeString_t<std::map<Key_,Value_,Rest_...>> {
    static std::string const &get () {
        static std::string const STR{"map<" + type_string_of<Key_>() + ',' + type_string_of<Value_>() + '>'};
        return STR;
    }
};

} // end namespace lvd
