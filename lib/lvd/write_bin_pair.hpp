// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of_pair.hpp"
#include "lvd/write.hpp"
#include <utility>

namespace lvd {

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::pair<Types_...>,BinEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, std::pair<Types_...> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        return out << inner_enc.out(src_val.first) << inner_enc.out(src_val.second);
    }
};

} // end namespace lvd
