// 2021.01.21 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "DerivedString.hpp"
#include "DerivedString_random.hpp" // Necessary so random generation works for these types.
#include "lvd/serialization.hpp"

namespace lvd {

// Template specialization is necessary here because templates don't account for class polymorphism.
// Ideally, this would be able to call some compile time function to determine the baseclass and use that.

template <> struct SerializeFrom_t<DerivedString_DC_EP> : public SerializeFrom_t<std::string> { };
template <> struct DeserializeTo_t<DerivedString_DC_EP> : public DeserializeTo_t<std::string> { };

template <> struct SerializeFrom_t<DerivedString_DC_IP> : public SerializeFrom_t<std::string> { };
template <> struct DeserializeTo_t<DerivedString_DC_IP> : public DeserializeTo_t<std::string> { };

// For non-default-constructible types, have to explicitly define DeserializedTo_t.

template <> struct SerializeFrom_t<DerivedString_NDC_EP> : public SerializeFrom_t<std::string> { };
template <> struct DeserializeTo_t<DerivedString_NDC_EP> : public DeserializeTo_t<std::string> { };
template <> struct DeserializedTo_t<DerivedString_NDC_EP> : public DeserializedTo_t<std::string> { };

template <> struct SerializeFrom_t<DerivedString_NDC_IP> : public SerializeFrom_t<std::string> { };
template <> struct DeserializeTo_t<DerivedString_NDC_IP> : public DeserializeTo_t<std::string> { };
template <> struct DeserializedTo_t<DerivedString_NDC_IP> : public DeserializedTo_t<std::string> { };

} // end namespace lvd
