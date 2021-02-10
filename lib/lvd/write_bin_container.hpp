// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/write.hpp"

namespace lvd {

// Helper template
template <typename Container_, typename Encoding_>
struct WriteValue_Container_t;

template <typename Container_, auto... Params_>
struct WriteValue_Container_t<Container_,BinEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_ostream<CharT_,Traits_> &operator() (std::basic_ostream<CharT_,Traits_> &out, BinEncoding_t<Params_...> const &enc, Container_ const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        out << inner_enc.out(src_val.size());
        for (auto const &element : src_val)
            out << inner_enc.out(element);
        return out;
    }
};

} // end namespace lvd
