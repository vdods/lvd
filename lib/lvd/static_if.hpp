// 2020.04.18 - Victor Dods

#pragma once

#include "lvd/fmt.hpp"

namespace lvd {

template <bool CONDITION_, typename Pos_, typename Neg_> struct static_if;

template <typename Pos_, typename Neg_> struct static_if<true,Pos_,Neg_> { using type = Pos_; };
template <typename Pos_, typename Neg_> struct static_if<false,Pos_,Neg_> { using type = Neg_; };

template <bool CONDITION_, typename Pos_, typename Neg_> using static_if_t = typename static_if<CONDITION_,Pos_,Neg_>::type;

} // end namespace lvd
