// 2021.01.21 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "DerivedString.hpp"
#include "DerivedString_random.hpp" // Necessary so random generation works for these types.
#include "lvd/serialization.hpp"

namespace lvd {

// Template specialization is necessary here because templates don't account for class polymorphism.
// Ideally, this would be able to call some compile time function to determine the baseclass and use that.

template <> struct Serialization_t<DerivedString_DC_IP> : public Serialization_t<std::string> { };
template <> struct Serialization_t<DerivedString_DC_EP> : public Serialization_t<std::string> { };
template <> struct Serialization_t<DerivedString_NDC_IP> : public Serialization_t<std::string> { };
template <> struct Serialization_t<DerivedString_NDC_EP> : public Serialization_t<std::string> { };

} // end namespace lvd
