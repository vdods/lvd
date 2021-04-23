// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/random_container.hpp"
#include <unordered_map>

namespace lvd {

template <typename... Types_>
struct PopulateRandom_t<std::unordered_map<Types_...>> : public PopulateRandom_AssociativeContainer_t<std::unordered_map<Types_...>> { };

} // end namespace lvd
