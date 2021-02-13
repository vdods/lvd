// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include <cstddef>
#include "lvd/comma.hpp"
#include "lvd/encoding.hpp"
#include "lvd/endian.hpp"
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

// Simple semantic subtype for for handling tuples.
// TODO: Should this actually use a reference to a tuple, so there's no copy/move?
template <size_t INDEX_, typename... Types_>
class IndexedTuple_t : public std::tuple<Types_...> {
public:

    using std::tuple<Types_...>::tuple;

    static IndexedTuple_t<INDEX_,Types_...> const &cast_from (std::tuple<Types_...> const &t) {
        return *reinterpret_cast<IndexedTuple_t<INDEX_,Types_...> const *>(&t);
    }
    static IndexedTuple_t<INDEX_,Types_...> &cast_from (std::tuple<Types_...> &t) {
        return *reinterpret_cast<IndexedTuple_t<INDEX_,Types_...> *>(&t);
    }

    // This returns the currently indexed value.
    decltype(auto) current () const {
        return std::get<INDEX_>(*this);
    }
    decltype(auto) current () {
        return std::get<INDEX_>(*this);
    }
    // This just increments the INDEX_ index in the type, it doesn't change the content.
    IndexedTuple_t<INDEX_+1,Types_...> const &incremented () const {
        return *reinterpret_cast<IndexedTuple_t<INDEX_+1,Types_...> const *>(this);
    }
    IndexedTuple_t<INDEX_+1,Types_...> &incremented () {
        return *reinterpret_cast<IndexedTuple_t<INDEX_+1,Types_...> *>(this);
    }
    constexpr bool has_ended () const {
        return INDEX_ >= sizeof...(Types_);
    }
    constexpr bool next_has_ended () const {
        return INDEX_+1 >= sizeof...(Types_);
    }
    constexpr size_t size () const {
        return sizeof...(Types_);
    }

    // This only prints the insides.
    std::ostream &print (std::ostream &out) const {
        if constexpr (INDEX_ >= sizeof...(Types_)) { // has_ended()
            // Nothing to do, we're already at/past the end.
            return out;
        } else if constexpr (sizeof...(Types_) == 1) { // A 1-tuple should print as (value,)
            return out << current() << comma;
        } else if constexpr (!(INDEX_+1 >= sizeof...(Types_))) { // !next_has_ended()
            out << current() << cspace;
            return incremented().print(out);
        } else {
            static_assert(INDEX_+1 >= sizeof...(Types_)); // next_has_ended(); We're at the end, leave out the comma-space.
            return out << current();
        }
    }
};

// Helper functions for type deduction.

template <typename... Types_>
IndexedTuple_t<0,Types_...> const &begin_indexed_tuple (std::tuple<Types_...> const &t) {
    return IndexedTuple_t<0,Types_...>::cast_from(t);
}

template <typename... Types_>
IndexedTuple_t<0,Types_...> &begin_indexed_tuple (std::tuple<Types_...> &t) {
    return IndexedTuple_t<0,Types_...>::cast_from(t);
}

} // end namespace lvd
