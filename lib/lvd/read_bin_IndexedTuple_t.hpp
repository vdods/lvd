// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/IndexedTuple_t.hpp"
#include "lvd/read.hpp"

namespace lvd {

template <size_t INDEX_, typename... Types_, auto... Params_>
struct ReadInPlace_t<IndexedTuple_t<INDEX_,Types_...>,BinEncoding_t<Params_...>> {
    std::istream &operator() (std::istream &in, BinEncoding_t<Params_...> const &enc, IndexedTuple_t<INDEX_,Types_...> &dest_val) const {
        if constexpr (dest_val.has_ended()) {
            // Nothing to do, we're already at/past the end.
            return in;
        } else {
            return in >> enc.in(dest_val.current()) >> enc.in(dest_val.incremented());
        }
    }
};

template <size_t INDEX_, typename... Types_, typename Encoding_>
struct ReadValue_t<IndexedTuple_t<INDEX_,Types_...>,Encoding_> {
    IndexedTuple_t<INDEX_,Types_...> operator() (std::istream &in, Encoding_ const &enc) const {
        IndexedTuple_t<INDEX_,Types_...> retval;
        ReadInPlace_t<IndexedTuple_t<INDEX_,Types_...>,Encoding_>()(in, enc, retval);
        return retval;
    }
};

} // end namespace lvd
