// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/IndexedTuple_t.hpp"
#include "lvd/write.hpp"

namespace lvd {

template <size_t INDEX_, typename... Types_, auto... Params_>
struct WriteValue_t<IndexedTuple_t<INDEX_,Types_...>,TextEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_ostream<CharT_,Traits_> &operator() (std::basic_ostream<CharT_,Traits_> &out, TextEncoding_t<Params_...> const &enc, IndexedTuple_t<INDEX_,Types_...> const &src_val) const {
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

} // end namespace lvd
