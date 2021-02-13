// 2021.02.13 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of.hpp"
#include <optional>

namespace lvd {

template <typename T_>
struct TypeString_t<std::optional<T_>> {
    static std::string const &get () {
        static std::string const STR{"optional<" + type_string_of<T_>() + '>'};
        return STR;
    }
};

} // end namespace lvd
