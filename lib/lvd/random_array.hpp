// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include "lvd/random.hpp"

namespace lvd {

// Generates a random std::array.
template <typename T_, size_t N_>
struct PopulateRandom_t<std::array<T_,N_>> {
    template <typename Rng_>
    void operator() (std::array<T_,N_> &dest, Rng_ &rng) const {
        for (auto &c : dest)
            populate_random(c, rng);
    }
};

} // end namespace lvd
