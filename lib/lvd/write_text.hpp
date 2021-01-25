// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include <cstddef>
#include "lvd/comma.hpp"
#include "lvd/encoding.hpp"
#include "lvd/endian.hpp"
#include "lvd/IndexedTuple_t.hpp"
#include "lvd/remove_cv_recursive.hpp"
#include "lvd/write.hpp"
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lvd {

//
// Implementations for WriteValue_t
//

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

template <typename T_, auto... Params_>
struct WriteValue_t<Type_t<T_>,TextEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, TextEncoding_t<Params_...> const &enc, Type_t<T_> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED) {
            out << "type("; // This is somewhat informally defined and may need to change later.
        }
        out << src_val;
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED) {
            out << ')';
        }
        return out;
    }
};

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::pair<Types_...>,TextEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, TextEncoding_t<Params_...> const &enc, std::pair<Types_...> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED) {
            out << type_v<std::pair<Types_...>>;
        }
        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        out << '(' << inner_enc.out(src_val.first) << cspace << inner_enc.out(src_val.second) << ')';
        return out;
    }
};

template <size_t INDEX_, typename... Types_, auto... Params_>
struct WriteValue_t<IndexedTuple_t<INDEX_,Types_...>,TextEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, TextEncoding_t<Params_...> const &enc, IndexedTuple_t<INDEX_,Types_...> const &src_val) const {
        if constexpr (src_val.has_ended()) {
            // Nothing to do, we're already at/past the end.
            return out;
        } else if constexpr (src_val.size() == 1) { // A 1-tuple should print as (value,)
            return out << enc.out(src_val.current()) << comma;
        } else if constexpr (!src_val.next_has_ended()) {
            return out << enc.out(src_val.current()) << cspace << enc.out(src_val.incremented());
        } else {
            static_assert(src_val.next_has_ended()); // We're at the end, leave out the comma-space.
            return out << enc.out(src_val.current());
        }
    }
};

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::tuple<Types_...>,TextEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, TextEncoding_t<Params_...> const &enc, std::tuple<Types_...> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED) {
            out << type_v<std::tuple<Types_...>>;
        }
        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        return out << '(' << inner_enc.out(begin_indexed_tuple(src_val)) << ')';
    }
};

// Helper template
template <typename Container_, typename Encoding_>
struct WriteValue_Container_t;

template <typename Container_, auto... Params_>
struct WriteValue_Container_t<Container_,TextEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, TextEncoding_t<Params_...> const &enc, Container_ const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED) {
            out << type_v<Container_>;
        }
        auto d = make_comma_space_delimiter();
        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        out << '(';
        for (auto const &element : src_val)
            out << d << inner_enc.out(element);
        out << ')';
        return out;
    }
};

template <typename T_, size_t N_, auto... Params_>
struct WriteValue_t<std::array<T_,N_>,TextEncoding_t<Params_...>> : public WriteValue_Container_t<std::array<T_,N_>,TextEncoding_t<Params_...>> { };
template <typename... Types_, auto... Params_>
struct WriteValue_t<std::vector<Types_...>,TextEncoding_t<Params_...>> : public WriteValue_Container_t<std::vector<Types_...>,TextEncoding_t<Params_...>> { };
template <typename... Types_, auto... Params_>
struct WriteValue_t<std::map<Types_...>,TextEncoding_t<Params_...>> : public WriteValue_Container_t<std::map<Types_...>,TextEncoding_t<Params_...>> { };
template <typename... Types_, auto... Params_>
struct WriteValue_t<std::set<Types_...>,TextEncoding_t<Params_...>> : public WriteValue_Container_t<std::set<Types_...>,TextEncoding_t<Params_...>> { };
template <typename... Types_, auto... Params_>
struct WriteValue_t<std::unordered_map<Types_...>,TextEncoding_t<Params_...>> : public WriteValue_Container_t<std::unordered_map<Types_...>,TextEncoding_t<Params_...>> { };
template <typename... Types_, auto... Params_>
struct WriteValue_t<std::unordered_set<Types_...>,TextEncoding_t<Params_...>> : public WriteValue_Container_t<std::unordered_set<Types_...>,TextEncoding_t<Params_...>> { };
// TODO: Others, like multimap, multiset, unordered_multimap, unordered_multiset

} // end namespace lvd
