// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include "lvd/read.hpp"
#include "lvd/read_bin_type.hpp"
#include "lvd/type_string_of_array.hpp"

namespace lvd {

template <typename T_, size_t N_, auto... Params_>
struct ReadInPlace_t<std::array<T_,N_>,BinEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_istream<CharT_,Traits_> &operator() (std::basic_istream<CharT_,Traits_> &in, BinEncoding_t<Params_...> const &enc, std::array<T_,N_> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        for (auto &element : dest_val)
            read_in_place(in, inner_enc, element);
        return in;
    }
};

} // end namespace lvd
