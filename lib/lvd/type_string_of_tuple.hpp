// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of.hpp"
#include <tuple>

namespace lvd {

template <typename... Types_>
struct TypeString_t<std::tuple<Types_...>> {
    static std::string const &get () {
        static std::string const STR{"tuple<" + type_string_of<VariadicSequence_t<Types_...>>() + '>'};
        return STR;
    }
};

} // end namespace lvd
