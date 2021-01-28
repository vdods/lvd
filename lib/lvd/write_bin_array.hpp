// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include "lvd/type_string_of_array.hpp"
#include "lvd/write.hpp"

namespace lvd {

template <typename T_, size_t N_, auto... Params_>
struct WriteValue_t<std::array<T_,N_>,BinEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, std::array<T_,N_> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        for (auto const &element : src_val)
            out << inner_enc.out(element);
        return out;
    }
};

} // end namespace lvd
