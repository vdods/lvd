// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/TypeString_t.hpp"
#include "lvd/write.hpp"

namespace lvd {

template <typename T_, auto... Params_>
struct WriteValue_t<Type_t<T_>,BinEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, Type_t<T_> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(std::string("type")); // This is informally defined for now, and may change later.

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        // Just encode T_ as its TypeString for now.  Other choices could be the type_info hash (not portable), or
        // some deterministic index assigned by a registration structure (this is the best).
        return out << inner_enc.out(type_string_of<T_>());
    }
};

} // end namespace lvd
