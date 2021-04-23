// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/random.hpp"
#include "lvd/remove_cv_recursive.hpp"

namespace lvd {

// Helper implementation for associative containers.  Generates a container having size between 0 and 10, inclusive.
template <typename Container_>
struct PopulateRandom_AssociativeContainer_t {
    template <typename Rng_>
    void operator() (Container_ &dest, Rng_ &rng) const {
        using ValueType = remove_cv_recursive_t<typename Container_::value_type>;
        auto desired_size = std::uniform_int_distribution<size_t>(0, 10)(rng);
        dest.clear();
        size_t i = 0;
        // WATCHDOG_LIMIT is needed because it's possible that the generated keys will collide,
        // and it's further possible that desired_size is not achievable (e.g. std::set<bool>
        // can't be any bigger than size 2).
        size_t constexpr WATCHDOG_LIMIT = 100;
        while (dest.size() < desired_size && i < WATCHDOG_LIMIT) {
            dest.emplace(make_random<ValueType>(rng));
            ++i;
        }
    }
};

} // end namespace lvd
