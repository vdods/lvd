// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/random.hpp"
#include <utility>

namespace lvd {

// Generates a random std::pair.
template <typename F_, typename S_>
struct PopulateRandom_t<std::pair<F_,S_>> {
    template <typename Rng_>
    void operator() (std::pair<F_,S_> &dest, Rng_ &rng) const {
        populate_random(dest.first, rng);
        populate_random(dest.second, rng);
    }
};

} // end namespace lvd
