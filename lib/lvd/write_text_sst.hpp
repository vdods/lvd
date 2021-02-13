// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/sst/SV_t.hpp"
#include "lvd/type_string_of.hpp"
#include "lvd/write_text_type.hpp"

namespace lvd {

template <typename S_, typename C_, auto... Params_>
struct WriteValue_t<sst::SV_t<S_,C_>,TextEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_ostream<CharT_,Traits_> &operator() (std::basic_ostream<CharT_,Traits_> &out, TextEncoding_t<Params_...> const &enc, sst::SV_t<S_,C_> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << ty<sst::SV_t<S_,C_>> << '(';

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        // TODO: Figure out how to suppress parens when src_val.cv() would print parens itself.
        out << inner_enc.out(src_val.cv());

        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << ')';
        return out;
    }
};

} // end namespace lvd
