// 2021.05.16 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <type_traits>

namespace lvd {

// For cloning any type via use of copy-constructor.  This is useful in a Rust-like pattern
// of programming where arguments are generally moved into constructors (and other functions),
// and where a clone (i.e. copy-construction) of a value is made explicitly, since that makes
// the expensive operation "ugly".
template <typename T_, typename = std::enable_if_t<std::is_copy_constructible_v<T_>>>
T_ cloned (T_ const &x) {
    return T_{x};
}

} // end namespace lvd
