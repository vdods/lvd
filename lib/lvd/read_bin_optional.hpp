// 2021.02.13 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/read.hpp"
#include "lvd/read_bin_type.hpp"
#include "lvd/type_string_of_optional.hpp"
#include <optional>

namespace lvd {

template <typename T_, auto... Params_>
struct ReadInPlace_t<std::optional<T_>,BinEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_istream<CharT_,Traits_> &operator() (std::basic_istream<CharT_,Traits_> &in, BinEncoding_t<Params_...> const &enc, std::optional<T_> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        auto has_value = inner_enc.template read<bool>(in);
        if (has_value)
            dest_val = inner_enc.template read<T_>(in);
        else
            dest_val = std::nullopt;
        return in;
    }
};

// In case T_ is not default-constructible, this is necessary.
template <typename T_, typename Encoding_>
struct ReadValue_t<std::optional<T_>,Encoding_> {
    template <typename CharT_, typename Traits_>
    std::optional<T_> operator() (std::basic_istream<CharT_,Traits_> &in, Encoding_ const &enc) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(ty<std::optional<T_>>); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        auto has_value = inner_enc.template read<bool>(in);
        if (has_value)
            return std::optional<T_>(inner_enc.template read<T_>(in));
        else
            return std::optional<T_>(std::nullopt);
    }
};

} // end namespace lvd
