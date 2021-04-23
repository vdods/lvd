// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/random_IndexedTuple_t.hpp"

namespace lvd {

// Generates a random std::tuple.
template <typename... Types_>
struct PopulateRandom_t<std::tuple<Types_...>> {
    template <typename Rng_>
    void operator() (std::tuple<Types_...> &dest, Rng_ &rng) const {
        populate_random(begin_indexed_tuple(dest), rng);
    }
};

} // end namespace lvd
