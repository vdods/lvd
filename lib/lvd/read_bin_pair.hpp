// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/read.hpp"
#include "lvd/read_bin_type.hpp"
#include "lvd/type_string_of_pair.hpp"
#include <utility>

namespace lvd {

template <typename... Types_, auto... Params_>
struct ReadInPlace_t<std::pair<Types_...>,BinEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_istream<CharT_,Traits_> &operator() (std::basic_istream<CharT_,Traits_> &in, BinEncoding_t<Params_...> const &enc, std::pair<Types_...> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        return in >> inner_enc.in(dest_val.first) >> inner_enc.in(dest_val.second);
    }
};

// In case either of the pair elements are not default-constructible, this is necessary.
template <typename F_, typename S_, typename Encoding_>
struct ReadValue_t<std::pair<F_,S_>,Encoding_> {
    template <typename CharT_, typename Traits_>
    std::pair<F_,S_> operator() (std::basic_istream<CharT_,Traits_> &in, Encoding_ const &enc) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(ty<std::pair<F_,S_>>); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        // Because we can't depend on the order of evaluation of function arguments, we have to guarantee order this way.
        // This may not work if the types are not movable.
        auto first = inner_enc.template read<F_>(in);
        auto second = inner_enc.template read<S_>(in);
        return std::pair<F_,S_>(std::move(first), std::move(second));
    }
};

} // end namespace lvd
