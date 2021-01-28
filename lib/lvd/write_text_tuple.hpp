// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of_tuple.hpp"
#include "lvd/write_text_IndexedTuple_t.hpp"
#include <tuple>

namespace lvd {

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::tuple<Types_...>,TextEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, TextEncoding_t<Params_...> const &enc, std::tuple<Types_...> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << ty<std::tuple<Types_...>>;

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        return out << '(' << inner_enc.out(begin_indexed_tuple(src_val)) << ')';
    }
};

} // end namespace lvd
