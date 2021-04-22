// 2021.04.21 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type_string_of_variant.hpp"
#include "lvd/write.hpp"
#include "lvd/variant.hpp"

namespace lvd {

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::variant<Types_...>,BinEncoding_t<Params_...>> {
    static_assert(sizeof...(Types_) > 0, "empty variants not (yet) supported");
    static_assert(sizeof...(Types_) < 0x10000, "that's a lot of types, guy!");

    template <typename CharT_, typename Traits_>
    std::basic_ostream<CharT_,Traits_> &operator() (std::basic_ostream<CharT_,Traits_> &out, BinEncoding_t<Params_...> const &enc, std::variant<Types_...> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));

        // No need to write the type of the index, it's known from this context.
        auto inner_enc = enc.with_demoted_type_encoding();
        out << inner_enc.out(uint16_t(src_val.index()));

        // Write the alternative's type and value itself.  Have to use visit to do this generically.
        return std::visit(
            [&out, &enc](auto &&src_alternative) -> std::basic_ostream<CharT_,Traits_>& {
                auto inner_enc = enc.with_demoted_type_encoding();
                // Write the type, if called for.
                if (enc.type_encoding() == TypeEncoding::INCLUDED)
                    out << inner_enc.out(type_of(src_alternative));
                return out << inner_enc.out(src_alternative);
            },
            src_val
        );
    }
};

} // end namespace lvd
