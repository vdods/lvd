// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of.hpp"
#include <vector>

namespace lvd {

template <typename T_, typename... Rest_>
struct TypeString_t<std::vector<T_,Rest_...>> {
    static std::string const &get () {
        static std::string const STR{"vector<" + type_string_of<T_>() + '>'};
        return STR;
    }
};

} // end namespace lvd
