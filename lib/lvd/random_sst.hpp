// 2021.04.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/abort.hpp"
#include "lvd/random.hpp"
#include "lvd/sst/mutator.hpp"
#include "lvd/sst/SV_t.hpp"

namespace lvd {

// Generates a random SV_t<S_,C_>.  This uses a natural method whose efficiency strongly
// depends on the nature of the S_::is_valid function.  I.e. it generates random concrete
// values cv until S_::is_valid(cv) returns true.  However, if it iterates more than 1000
// times, it will abort.  For types where exceeding the iteration limit is of more than
// negligible likelihood, you should provide your own implementation of
// PopulateRandom_t<lvd::sst::SV_t<S_,C_>>.
template <typename S_, typename C_>
struct PopulateRandom_t<sst::SV_t<S_,C_>> {
    template <typename Rng_>
    void operator() (sst::SV_t<S_,C_> &dest, Rng_ &rng) const {
        size_t constexpr WATCHDOG_LIMIT = 1000;
        for (size_t i = 0; i < WATCHDOG_LIMIT; ++i) {
            auto cv = make_random<C_>(rng);
            if (S_::is_valid(cv)) {
                auto dest_mutator = sst::mutator(dest);
                dest_mutator.cv() = cv;
                return;
            }
        }
        // TODO: Have S_ optionally provide a populate_random method.
        LVD_ABORT("exceeded watchdog limit -- natural is_valid-based random method isn't reliable; implement custom PopulateRandom_t");
    }
};

// Needed because SV_t<S_,C_> isn't necessarily default-constructible.
template <typename S_, typename C_>
struct MakeRandom_t<sst::SV_t<S_,C_>> {
    template <typename Rng_>
    sst::SV_t<S_,C_> operator() (Rng_ &rng) const {
        size_t constexpr WATCHDOG_LIMIT = 1000;
        for (size_t i = 0; i < WATCHDOG_LIMIT; ++i) {
            auto cv = make_random<C_>(rng);
            if (S_::is_valid(cv))
                return sst::SV_t<S_,C_>{cv};
        }
        // TODO: Have S_ optionally provide a make_random method.
        LVD_ABORT("exceeded watchdog limit -- natural is_valid-based random method isn't reliable; implement custom MakeRandom_t");
    }
};

} // end namespace lvd
