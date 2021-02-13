// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/read_bin_IndexedTuple_t.hpp"
#include "lvd/read_bin_type.hpp"
#include "lvd/type_string_of_tuple.hpp"
#include <tuple>

namespace lvd {

template <typename... Types_, auto... Params_>
struct ReadInPlace_t<std::tuple<Types_...>,BinEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_istream<CharT_,Traits_> &operator() (std::basic_istream<CharT_,Traits_> &in, BinEncoding_t<Params_...> const &enc, std::tuple<Types_...> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        return in >> inner_enc.in(begin_indexed_tuple(dest_val));
    }
};

// In case either of the pair elements are not default-constructible, this is necessary.
template <typename Encoding_>
struct ReadValue_t<std::tuple<>,Encoding_> {
    template <typename CharT_, typename Traits_>
    std::tuple<> operator() (std::basic_istream<CharT_,Traits_> &in, Encoding_ const &enc) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(ty<std::tuple<>>); // This will throw if the type doesn't match.

        // No contents.
        return std::tuple<>();
    }
};

// In case either of the pair elements are not default-constructible, this is necessary.
template <typename First_, typename Encoding_>
struct ReadValue_t<std::tuple<First_>,Encoding_> {
    template <typename CharT_, typename Traits_>
    std::tuple<First_> operator() (std::basic_istream<CharT_,Traits_> &in, Encoding_ const &enc) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(ty<std::tuple<First_>>); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        return std::tuple(inner_enc.template read<First_>(in));
    }
};

// In case either of the pair elements are not default-constructible, this is necessary.
template <typename First_, typename... Rest_, typename Encoding_>
struct ReadValue_t<std::tuple<First_,Rest_...>,Encoding_> {
    template <typename CharT_, typename Traits_>
    std::tuple<First_,Rest_...> operator() (std::basic_istream<CharT_,Traits_> &in, Encoding_ const &enc) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(ty<std::tuple<First_,Rest_...>>); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        // Have to do this separately to guarantee evaluation order.
        auto tuple_first = std::tuple<First_>(inner_enc.template read<First_>(in));
        return std::tuple_cat(tuple_first, inner_enc.template read<std::tuple<Rest_...>>(in));
    }
};

} // end namespace lvd
