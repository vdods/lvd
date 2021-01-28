// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of.hpp"
#include <utility>

namespace lvd {

template <typename... Types_>
struct TypeString_t<std::pair<Types_...>> {
    static std::string const &get () {
        static std::string const STR{"pair<" + type_string_of<VariadicSequence_t<Types_...>>() + '>'};
        return STR;
    }
};

} // end namespace lvd
