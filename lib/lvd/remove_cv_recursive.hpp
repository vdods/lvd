// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <type_traits>
#include <utility>

namespace lvd {

// Applies std::remove_cv_t to component types of a compound type T_, e.g. std::pair.
template <typename T_>
struct remove_cv_recursive {
    using type = std::remove_cv_t<T_>;
};

// Convenience alias.
template <typename T_>
using remove_cv_recursive_t = typename remove_cv_recursive<T_>::type;

template <typename F_, typename S_>
struct remove_cv_recursive<std::pair<F_,S_>> {
    using type = std::pair<remove_cv_recursive_t<F_>,remove_cv_recursive_t<S_>>;
};

// TODO: Implement for std::tuple

} // end namespace lvd
