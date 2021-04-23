// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/random.hpp"
#include <optional>

namespace lvd {

// Generates a random std::optional with 75% likelihood of holding a value.
template <typename T_>
struct PopulateRandom_t<std::optional<T_>> {
    template <typename Rng_>
    void operator() (std::optional<T_> &dest, Rng_ &rng) const {
        bool has_value = std::uniform_int_distribution<size_t>(0, 3)(rng) != 0;
        if (has_value)
            dest = make_random<T_>(rng);
        else
            dest = std::nullopt;
    }
};

} // end namespace lvd
