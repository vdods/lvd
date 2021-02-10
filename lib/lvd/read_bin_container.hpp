// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/read.hpp"
#include "lvd/read_bin_type.hpp"
#include "lvd/remove_cv_recursive.hpp"

namespace lvd {

// Helper template
template <typename Container_, typename Encoding_>
struct ReadInPlace_AssociativeContainer_t;

template <typename Container_, auto... Params_>
struct ReadInPlace_AssociativeContainer_t<Container_,BinEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_istream<CharT_,Traits_> &operator() (std::basic_istream<CharT_,Traits_> &in, BinEncoding_t<Params_...> const &enc, Container_ &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        using ValueType = remove_cv_recursive_t<typename Container_::value_type>;
        dest_val.clear();
        // TODO: Use uint32_t, since size_t is machine-dependent.
        auto size = read_value<size_t>(in, inner_enc);
        for (size_t n = 0; n < size; ++n)
            dest_val.emplace(read_value<ValueType>(in, inner_enc));
        return in;
    }
};

// Helper template
template <typename Container_, typename Encoding_>
struct ReadInPlace_SequenceContainer_t;

template <typename Container_, auto... Params_>
struct ReadInPlace_SequenceContainer_t<Container_,BinEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_istream<CharT_,Traits_> &operator() (std::basic_istream<CharT_,Traits_> &in, BinEncoding_t<Params_...> const &enc, Container_ &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        using ValueType = remove_cv_recursive_t<typename Container_::value_type>;
        dest_val.clear();
        auto size = read_value<size_t>(in, inner_enc);
        dest_val.reserve(size);
        for (size_t n = 0; n < size; ++n)
            dest_val.emplace_back(read_value<ValueType>(in, inner_enc));
        return in;
    }
};

} // end namespace lvd
