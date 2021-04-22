// 2021.04.21 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of.hpp"
#include "lvd/variant.hpp"

namespace lvd {

template <typename... Types_>
struct TypeString_t<std::variant<Types_...>> {
    static std::string const &get () {
        static std::string const STR{"variant<" + type_string_of<VariadicSequence_t<Types_...>>() + '>'};
        return STR;
    }
};

} // end namespace lvd
