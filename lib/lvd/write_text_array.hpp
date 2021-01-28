// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include "lvd/type_string_of_array.hpp"
#include "lvd/write_text_container.hpp"

namespace lvd {

template <typename T_, size_t N_, auto... Params_>
struct WriteValue_t<std::array<T_,N_>,TextEncoding_t<Params_...>> : public WriteValue_Container_t<std::array<T_,N_>,TextEncoding_t<Params_...>> { };

} // end namespace lvd
