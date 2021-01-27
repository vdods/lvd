// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/write_text_container.hpp"
#include <unordered_set>

namespace lvd {

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::unordered_set<Types_...>,TextEncoding_t<Params_...>> : public WriteValue_Container_t<std::unordered_set<Types_...>,TextEncoding_t<Params_...>> { };

} // end namespace lvd
