// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/write_bin_container.hpp"
#include <map>

namespace lvd {

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::map<Types_...>,BinEncoding_t<Params_...>> : public WriteValue_Container_t<std::map<Types_...>,BinEncoding_t<Params_...>> { };

} // end namespace lvd
