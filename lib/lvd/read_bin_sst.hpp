// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/read_bin_type.hpp"
#include "lvd/sst/SV_t.hpp"

namespace lvd {

template <typename S_, typename C_, auto... Params_>
struct ReadInPlace_t<sst::SV_t<S_,C_>,BinEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_istream<CharT_,Traits_> &operator() (std::basic_istream<CharT_,Traits_> &in, BinEncoding_t<Params_...> const &enc, sst::SV_t<S_,C_> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        // Use assign-move so that the validity check is done automatically.
        dest_val = inner_enc.template read<C_>(in);
        return in;
    }
};

// SV_t<S_,C_> is not in general default-constructible, so ReadValue_t has to be specialized.
template <typename S_, typename C_, typename Encoding_>
struct ReadValue_t<sst::SV_t<S_,C_>,Encoding_> {
    template <typename CharT_, typename Traits_>
    sst::SV_t<S_,C_> operator() (std::basic_istream<CharT_,Traits_> &in, Encoding_ const &enc) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(ty<sst::SV_t<S_,C_>>); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();

        auto retval = sst::SV_t<S_,C_>{sst::no_check};
        retval = inner_enc.template read<C_>(in);
        return retval;
//         auto retval = sst::SV_t<S_,C_>{sst::no_check, inner_enc.template read<C_>(in)};
//         retval->template check<check_policy_for__ctor_move_C(S_{})>();
//         return retval;

//         return sst::SV_t<S_,C_>{inner_enc.template read<C_>(in)};
    }
};

} // end namespace lvd
