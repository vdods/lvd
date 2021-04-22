// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/write.hpp"

#include "lvd/comma.hpp"

namespace lvd {

// Helper template
template <typename Container_, typename Encoding_>
struct WriteValue_Container_t;

template <typename Container_, auto... Params_>
struct WriteValue_Container_t<Container_,TextEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_ostream<CharT_,Traits_> &operator() (std::basic_ostream<CharT_,Traits_> &out, TextEncoding_t<Params_...> const &enc, Container_ const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << ty<Container_>;

        auto d = lvd::make_comma_space_delimiter();
        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        out << '(';
        for (auto const &element : src_val)
            out << d << inner_enc.out(element);
        out << ')';
        return out;
    }
};

} // end namespace lvd
