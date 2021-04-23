// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/IndexedTuple_t.hpp"
#include "lvd/random.hpp"

namespace lvd {

// Generates a random IndexedTuple_t.
template <size_t INDEX_, typename... Types_>
struct PopulateRandom_t<IndexedTuple_t<INDEX_,Types_...>> {
    template <typename Rng_>
    void operator() (IndexedTuple_t<INDEX_,Types_...> &dest, Rng_ &rng) const {
        if constexpr (!dest.has_ended()) {
            populate_random(dest.current(), rng);
            populate_random(dest.incremented(), rng);
        }
    }
};

} // end namespace lvd
