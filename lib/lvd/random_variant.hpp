// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/random.hpp"
#include "lvd/variant.hpp"

namespace lvd {

// Generates a random std::variant with uniform likelihood of generating each alternative type.
template <typename... Types_>
struct PopulateRandom_t<std::variant<Types_...>> {
    template <typename Rng_>
    void operator() (std::variant<Types_...> &dest, Rng_ &rng) const {
        auto index = std::uniform_int_distribution<size_t>(0, sizeof...(Types_)-1)(rng);
        call_on_indexed_type<0,Types_...>(
            index,
            [&dest, &rng](auto &&t){
                static_assert(is_Type_t_v<std::decay_t<decltype(t)>>);
                using T_ = typename std::decay_t<decltype(t)>::T;
                if constexpr (!std::is_same_v<T_,void>)
                    dest = make_random<T_>(rng);
            }
        );
    }
};

} // end namespace lvd
