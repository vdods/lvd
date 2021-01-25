// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include <cstddef>
#include "lvd/encoding.hpp"
#include "lvd/endian.hpp"
#include "lvd/IndexedTuple_t.hpp"
#include "lvd/read.hpp"
#include "lvd/remove_cv_recursive.hpp"
#include "lvd/Type_t.hpp"
#include "lvd/TypeString_t.hpp"
#include <istream>
#include <map>
#include <set>
#include <type_traits>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lvd {

//
// Implementations for ReadInPlace_t
//

template <typename T_, auto... Params_>
struct ReadInPlace_t<Type_t<T_>,BinEncoding_t<Params_...>> {
    std::istream &operator() (std::istream &in, BinEncoding_t<Params_...> const &enc, Type_t<T_> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED) {
            auto s = enc.with_demoted_type_encoding().template read<std::string>(in);
            if (s != "type") // This is somewhat informally defined and may need to change later.
                throw std::runtime_error("type mismatch; expected \"type\" but got " + literal_of(s));
        }

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        // This is the string that encodes T_
        auto s = inner_enc.template read<std::string>(in);
        // This just asserts that the type matches.
        if (s != TypeString_t<T_>::get())
            throw std::runtime_error("type mismatch; expected " + literal_of(TypeString_t<T_>::get()) + " but got " + literal_of(s));
        return in;
    }
};

template <typename... Types_, auto... Params_>
struct ReadInPlace_t<std::basic_string<Types_...>,BinEncoding_t<Params_...>> {
    std::istream &operator() (std::istream &in, BinEncoding_t<Params_...> const &enc, std::basic_string<Types_...> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        auto size = inner_enc.template read<size_t>(in);
        // TODO: Probably put a limit on the size to avoid bugs in which a huge value is read
        // causing memory to fill up.
        dest_val.resize(size);
        in.read(reinterpret_cast<char *>(&dest_val[0]), size*sizeof(dest_val[0]));
        // If called for, endian-swap the contents.
        if (machine_endianness() != enc.endianness()) {
            for (auto &c : dest_val)
                swap_byte_order_of(c);
        }
        return in;
    }
};

template <typename... Types_, auto... Params_>
struct ReadInPlace_t<std::pair<Types_...>,BinEncoding_t<Params_...>> {
    std::istream &operator() (std::istream &in, BinEncoding_t<Params_...> const &enc, std::pair<Types_...> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        return in >> inner_enc.in(dest_val.first) >> inner_enc.in(dest_val.second);
    }
};

template <size_t INDEX_, typename... Types_, auto... Params_>
struct ReadInPlace_t<IndexedTuple_t<INDEX_,Types_...>,BinEncoding_t<Params_...>> {
    std::istream &operator() (std::istream &in, BinEncoding_t<Params_...> const &enc, IndexedTuple_t<INDEX_,Types_...> &dest_val) const {
        if constexpr (dest_val.has_ended()) {
            // Nothing to do, we're already at/past the end.
            return in;
        } else {
            return in >> enc.in(dest_val.current()) >> enc.in(dest_val.incremented());
        }
    }
};

template <typename... Types_, auto... Params_>
struct ReadInPlace_t<std::tuple<Types_...>,BinEncoding_t<Params_...>> {
    std::istream &operator() (std::istream &in, BinEncoding_t<Params_...> const &enc, std::tuple<Types_...> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        return in >> inner_enc.in(begin_indexed_tuple(dest_val));
    }
};

template <typename T_, size_t N_, auto... Params_>
struct ReadInPlace_t<std::array<T_,N_>,BinEncoding_t<Params_...>> {
    std::istream &operator() (std::istream &in, BinEncoding_t<Params_...> const &enc, std::array<T_,N_> &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        // The type is already known at this point.
        auto inner_enc = enc.with_demoted_type_encoding();
        for (auto &element : dest_val)
            read_in_place(in, inner_enc, element);
        return in;
    }
};

// Helper template
template <typename Container_, typename Encoding_>
struct ReadInPlace_SequenceContainer_t;

template <typename Container_, auto... Params_>
struct ReadInPlace_SequenceContainer_t<Container_,BinEncoding_t<Params_...>> {
    std::istream &operator() (std::istream &in, BinEncoding_t<Params_...> const &enc, Container_ &dest_val) const {
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

template <typename... Types_, typename Encoding_>
struct ReadInPlace_t<std::vector<Types_...>,Encoding_> : public ReadInPlace_SequenceContainer_t<std::vector<Types_...>,Encoding_> { };

// Helper template
template <typename Container_, typename Encoding_>
struct ReadInPlace_AssociativeContainer_t;

template <typename Container_, auto... Params_>
struct ReadInPlace_AssociativeContainer_t<Container_,BinEncoding_t<Params_...>> {
    std::istream &operator() (std::istream &in, BinEncoding_t<Params_...> const &enc, Container_ &dest_val) const {
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

template <typename... Types_, typename Encoding_>
struct ReadInPlace_t<std::map<Types_...>,Encoding_> : public ReadInPlace_AssociativeContainer_t<std::map<Types_...>,Encoding_> { };
template <typename... Types_, typename Encoding_>
struct ReadInPlace_t<std::set<Types_...>,Encoding_> : public ReadInPlace_AssociativeContainer_t<std::set<Types_...>,Encoding_> { };
template <typename... Types_, typename Encoding_>
struct ReadInPlace_t<std::unordered_map<Types_...>,Encoding_> : public ReadInPlace_AssociativeContainer_t<std::unordered_map<Types_...>,Encoding_> { };
template <typename... Types_, typename Encoding_>
struct ReadInPlace_t<std::unordered_set<Types_...>,Encoding_> : public ReadInPlace_AssociativeContainer_t<std::unordered_set<Types_...>,Encoding_> { };
// TODO: Others

//
// Implementations for ReadValue_t
//

template <size_t INDEX_, typename... Types_, typename Encoding_>
struct ReadValue_t<IndexedTuple_t<INDEX_,Types_...>,Encoding_> {
    IndexedTuple_t<INDEX_,Types_...> operator() (std::istream &in, Encoding_ const &enc) const {
        IndexedTuple_t<INDEX_,Types_...> retval;
        ReadInPlace_t<IndexedTuple_t<INDEX_,Types_...>,Encoding_>()(in, enc, retval);
        return retval;
    }
};

} // end namespace lvd
