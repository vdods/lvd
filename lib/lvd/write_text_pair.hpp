// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of_pair.hpp"
#include "lvd/write.hpp"
#include <utility>

namespace lvd {

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::pair<Types_...>,TextEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, TextEncoding_t<Params_...> const &enc, std::pair<Types_...> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << ty<std::pair<Types_...>>;

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        out << '(' << inner_enc.out(src_val.first) << cspace << inner_enc.out(src_val.second) << ')';
        return out;
    }
};

} // end namespace lvd
