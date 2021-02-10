// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/read_bin_IndexedTuple_t.hpp"
#include "lvd/read_bin_type.hpp"
#include "lvd/type_string_of_tuple.hpp"
#include <tuple>

namespace lvd {

template <typename... Types_, auto... Params_>
struct ReadInPlace_t<std::tuple<Types_...>,BinEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_istream<CharT_,Traits_> &operator() (std::basic_istream<CharT_,Traits_> &in, BinEncoding_t<Params_...> const &enc, std::tuple<Types_...> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        return in >> inner_enc.in(begin_indexed_tuple(dest_val));
    }
};

} // end namespace lvd
