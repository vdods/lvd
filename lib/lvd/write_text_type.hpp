// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of.hpp"
#include "lvd/write.hpp"

namespace lvd {

template <typename T_, auto... Params_>
struct WriteValue_t<Type_t<T_>,TextEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_ostream<CharT_,Traits_> &operator() (std::basic_ostream<CharT_,Traits_> &out, TextEncoding_t<Params_...> const &enc, Type_t<T_> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << "type("; // This is somewhat informally defined and may need to change later.
        out << src_val;
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << ')';
        return out;
    }
};

} // end namespace lvd
