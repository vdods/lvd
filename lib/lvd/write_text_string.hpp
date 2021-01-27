// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/write.hpp"
#include <string>

namespace lvd {

// TODO: Implement one for generic std::basic_string
template <auto... Params_>
struct WriteValue_t<std::string,TextEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, TextEncoding_t<Params_...> const &enc, std::string const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED) {
            out << type_v<std::string> << '(';
        }
        out << literal_of(src_val);
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED) {
            out << ')';
        }
        return out;
    }
};

} // end namespace lvd
