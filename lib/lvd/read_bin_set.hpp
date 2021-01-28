// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/read_bin_container.hpp"
#include "lvd/type_string_of_set.hpp"
#include <set>

namespace lvd {

template <typename... Types_, typename Encoding_>
struct ReadInPlace_t<std::set<Types_...>,Encoding_> : public ReadInPlace_AssociativeContainer_t<std::set<Types_...>,Encoding_> { };

} // end namespace lvd
