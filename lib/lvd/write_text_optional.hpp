// 2021.02.13 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of_optional.hpp"
#include "lvd/write.hpp"
#include <optional>

namespace lvd {

template <typename T_, auto... Params_>
struct WriteValue_t<std::optional<T_>,TextEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_ostream<CharT_,Traits_> &operator() (std::basic_ostream<CharT_,Traits_> &out, TextEncoding_t<Params_...> const &enc, std::optional<T_> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << ty<std::optional<T_>> << '(';

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        bool has_value = src_val.has_value();
        if (has_value)
            out << inner_enc.out(src_val.value());
        else
            out << "nullopt";

        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << ')';
        return out;
    }
};

} // end namespace lvd
