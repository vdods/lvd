// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/random.hpp"
#include <vector>

namespace lvd {

// Generates a random std::vector of size within [0,10].
template <typename... Types_>
struct PopulateRandom_t<std::vector<Types_...>> {
    using Vector = std::vector<Types_...>;
    template <typename Rng_>
    void operator() (Vector &dest, Rng_ &rng) const {
        dest.resize(std::uniform_int_distribution<size_t>(0, 10)(rng));
        for (auto &c : dest)
            populate_random(c, rng);
    }
};

} // end namespace lvd
