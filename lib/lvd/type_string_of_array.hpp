// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include "lvd/fmt.hpp"
#include "lvd/type_string_of.hpp"

namespace lvd {

template <typename T_, size_t N_>
struct TypeString_t<std::array<T_,N_>> {
    static std::string const &get () {
        static std::string const STR{"array<" + type_string_of<T_>() + ',' + LVD_FMT(N_) + '>'};
        return STR;
    }
};

} // end namespace lvd
