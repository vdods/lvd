// 2021.04.21 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/fmt.hpp"
#include "lvd/read.hpp"
#include "lvd/read_bin_type.hpp"
#include "lvd/type.hpp"
#include "lvd/type_string_of_variant.hpp"
#include "lvd/variant.hpp"
#include <stdexcept>
#include <tuple>

namespace lvd {

template <typename... Types_, auto... Params_>
struct ReadInPlace_t<std::variant<Types_...>,BinEncoding_t<Params_...>> {
    static_assert(sizeof...(Types_) > 0, "empty variants not (yet) supported");
    static_assert(sizeof...(Types_) < 0x10000, "that's a lot of types, guy!");

    template <typename CharT_, typename Traits_>
    std::basic_istream<CharT_,Traits_> &operator() (std::basic_istream<CharT_,Traits_> &in, BinEncoding_t<Params_...> const &enc, std::variant<Types_...> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // No need to read the type of the index, it's known from this context.
        auto inner_enc = enc.with_demoted_type_encoding();
        size_t index = inner_enc.template read<uint16_t>(in);

        call_on_indexed_type<0,Types_...>(
            index,
            [&in, &enc, &dest_val](auto &&t){
                static_assert(is_Type_t_v<std::decay_t<decltype(t)>>);
                using T_ = typename std::decay_t<decltype(t)>::T;
                auto inner_enc = enc.with_demoted_type_encoding();
                // Read the type, if called for.  This will throw if the type doesn't match.
                if (enc.type_encoding() == TypeEncoding::INCLUDED)
                    in >> inner_enc.in(ty<T_>); // This will throw if the type doesn't match.
                dest_val = inner_enc.template read<T_>(in);
            }
        );
        return in;
    }
};

// In case T_ is not default-constructible, this is necessary.
template <typename... Types_, typename Encoding_>
struct ReadValue_t<std::variant<Types_...>,Encoding_> {
    static_assert(sizeof...(Types_) > 0, "empty variants not (yet) supported");
    static_assert(sizeof...(Types_) < 0x10000, "that's a lot of types, guy!");

    template <typename CharT_, typename Traits_>
    std::variant<Types_...> operator() (std::basic_istream<CharT_,Traits_> &in, Encoding_ const &enc) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(ty<std::variant<Types_...>>); // This will throw if the type doesn't match.

        // No need to read the type of the index, it's known from this context.
        size_t index = enc.with_demoted_type_encoding().template read<uint16_t>(in);

        // TEMP HACK: This won't work for non-default constructible first alternative type
        std::variant<Types_...> retval;
        call_on_indexed_type<0,Types_...>(
            index,
            [&in, &enc, &retval](auto &&t){
                static_assert(is_Type_t_v<std::decay_t<decltype(t)>>);
                using T_ = typename std::decay_t<decltype(t)>::T;
                auto inner_enc = enc.with_demoted_type_encoding();
                // Read the type, if called for.  This will throw if the type doesn't match.
                if (enc.type_encoding() == TypeEncoding::INCLUDED)
                    in >> inner_enc.in(ty<T_>); // This will throw if the type doesn't match.
                retval = inner_enc.template read<T_>(in);
            }
        );
        return retval;
    }
};

} // end namespace lvd
