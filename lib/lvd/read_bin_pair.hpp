// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/read.hpp"
#include "lvd/read_bin_type.hpp"
#include "lvd/type_string_of_pair.hpp"
#include <utility>

namespace lvd {

template <typename... Types_, auto... Params_>
struct ReadInPlace_t<std::pair<Types_...>,BinEncoding_t<Params_...>> {
    std::istream &operator() (std::istream &in, BinEncoding_t<Params_...> const &enc, std::pair<Types_...> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        return in >> inner_enc.in(dest_val.first) >> inner_enc.in(dest_val.second);
    }
};

} // end namespace lvd
