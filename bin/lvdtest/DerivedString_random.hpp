// 2021.01.21 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "DerivedString.hpp"
#include "lvd/random.hpp"

namespace lvd {

// Template specialization is necessary here because templates don't account for class polymorphism.
// Ideally, this would be able to call some compile time function to determine the baseclass and use that.

template <> struct PopulateRandom_t<DerivedString_DC_IP> : public PopulateRandom_t<std::string> { };

template <> struct PopulateRandom_t<DerivedString_DC_EP> : public PopulateRandom_t<std::string> { };

template <> struct PopulateRandom_t<DerivedString_NDC_IP> : public PopulateRandom_t<std::string> { };
template <> struct MakeRandom_t<DerivedString_NDC_IP> : public MakeRandom_t<std::string> {
    DerivedString_NDC_IP operator() (auto &rng) const {
        return DerivedString_NDC_IP(make_random<std::string>(rng));
    }
};

template <> struct PopulateRandom_t<DerivedString_NDC_EP> : public PopulateRandom_t<std::string> { };
template <> struct MakeRandom_t<DerivedString_NDC_EP> : public MakeRandom_t<std::string> {
    DerivedString_NDC_EP operator() (auto &rng) const {
        return DerivedString_NDC_EP(make_random<std::string>(rng));
    }
};

} // end namespace lvd
