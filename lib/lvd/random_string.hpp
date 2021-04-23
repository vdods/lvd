// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/random.hpp"
#include <string>

namespace lvd {

// Generates a random std::string of size within [0,10] and only printable ascii chars.
template <>
struct PopulateRandom_t<std::string> {
    static bool ascii_char_is_printable (char c) { return ' ' <= c && c <= '~'; }

    template <typename Rng_>
    void operator() (std::string &dest, Rng_ &rng) const {
        dest.resize(std::uniform_int_distribution<size_t>(0, 10)(rng), ' ');
        for (auto &c : dest)
            populate_random(c, rng, ascii_char_is_printable);
    }
};

} // end namespace lvd
