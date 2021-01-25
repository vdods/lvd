// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include <cstddef>
#include "lvd/encoding.hpp"
#include "lvd/endian.hpp"
#include "lvd/IndexedTuple_t.hpp"
#include "lvd/remove_cv_recursive.hpp"
#include "lvd/write.hpp"
#include <map>
#include <ostream>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lvd {

//
// Implementations for WriteValue_t
//

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::basic_string<Types_...>,BinEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, std::basic_string<Types_...> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();

        out << inner_enc.out(src_val.size()); // TODO: Limit to uint32_t
        if constexpr (sizeof(src_val[0]) == 1) {
            // No need to byte-order-swap in this case.
            out.write(reinterpret_cast<char const *>(&src_val[0]), src_val.size()*sizeof(src_val[0]));
        } else if (machine_endianness() == enc.endianness()) {
            // No need to byte-order-swap in this case.
            out.write(reinterpret_cast<char const *>(&src_val[0]), src_val.size()*sizeof(src_val[0]));
        } else {
            static_assert(sizeof(src_val[0]) > 1);
            // Have to write each element individually, and this will take care of the byte-swap.
            for (auto const &c : src_val)
                out << inner_enc.out(c);
        }
        return out;
    }
};

template <typename T_, auto... Params_>
struct WriteValue_t<Type_t<T_>,BinEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, Type_t<T_> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(std::string("type")); // This is informally defined for now, and may change later.

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        // Just encode T_ as its TypeString for now.  Other choices could be the type_info hash (not portable), or
        // some deterministic index assigned by a registration structure (this is the best).
        return out << inner_enc.out(type_string_of<T_>());
    }
};

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::pair<Types_...>,BinEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, std::pair<Types_...> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        return out << inner_enc.out(src_val.first) << inner_enc.out(src_val.second);
    }
};

template <size_t INDEX_, typename... Types_, auto... Params_>
struct WriteValue_t<IndexedTuple_t<INDEX_,Types_...>,BinEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, IndexedTuple_t<INDEX_,Types_...> const &src_val) const {
        if constexpr (src_val.has_ended()) {
            // Nothing to do, we're already at/past the end.
            return out;
        } else {
            return out << enc.out(src_val.current()) << enc.out(src_val.incremented());
        }
    }
};

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::tuple<Types_...>,BinEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, std::tuple<Types_...> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        return out << inner_enc.out(begin_indexed_tuple(src_val));
    }
};

template <typename T_, size_t N_, auto... Params_>
struct WriteValue_t<std::array<T_,N_>,BinEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, std::array<T_,N_> const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        for (auto const &element : src_val)
            out << inner_enc.out(element);
        return out;
    }
};

// Helper template
template <typename Container_, typename Encoding_>
struct WriteValue_Container_t;

template <typename Container_, auto... Params_>
struct WriteValue_Container_t<Container_,BinEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, Container_ const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));

        // This will suppress unnecessary inner element type info, since it's already present in the given type.
        auto inner_enc = enc.with_demoted_type_encoding();
        out << inner_enc.out(src_val.size());
        for (auto const &element : src_val)
            out << inner_enc.out(element);
        return out;
    }
};

template <typename... Types_, auto... Params_>
struct WriteValue_t<std::vector<Types_...>,BinEncoding_t<Params_...>> : public WriteValue_Container_t<std::vector<Types_...>,BinEncoding_t<Params_...>> { };
template <typename... Types_, auto... Params_>
struct WriteValue_t<std::map<Types_...>,BinEncoding_t<Params_...>> : public WriteValue_Container_t<std::map<Types_...>,BinEncoding_t<Params_...>> { };
template <typename... Types_, auto... Params_>
struct WriteValue_t<std::set<Types_...>,BinEncoding_t<Params_...>> : public WriteValue_Container_t<std::set<Types_...>,BinEncoding_t<Params_...>> { };
template <typename... Types_, auto... Params_>
struct WriteValue_t<std::unordered_map<Types_...>,BinEncoding_t<Params_...>> : public WriteValue_Container_t<std::unordered_map<Types_...>,BinEncoding_t<Params_...>> { };
template <typename... Types_, auto... Params_>
struct WriteValue_t<std::unordered_set<Types_...>,BinEncoding_t<Params_...>> : public WriteValue_Container_t<std::unordered_set<Types_...>,BinEncoding_t<Params_...>> { };
// TODO: Others, like multimap, multiset, unordered_multimap, unordered_multiset

} // end namespace lvd
