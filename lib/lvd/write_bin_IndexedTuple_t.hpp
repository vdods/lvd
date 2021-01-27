// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/IndexedTuple_t.hpp"
#include "lvd/write.hpp"

namespace lvd {

template <size_t INDEX_, typename... Types_, auto... Params_>
struct WriteValue_t<IndexedTuple_t<INDEX_,Types_...>,BinEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, IndexedTuple_t<INDEX_,Types_...> const &src_val) const {
        if constexpr (src_val.has_ended()) {
            // Nothing to do, we're already at/past the end.
            return out;
        } else {
            return out << enc.out(src_val.current()) << enc.out(src_val.incremented());
        }
    }
};

} // end namespace lvd
