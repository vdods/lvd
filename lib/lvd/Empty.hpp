// 2021.04.21 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <functional>
#include <ostream>
#include <string>

namespace lvd {

// C++ doesn't allow instantiation of void, and it doesn't allow zero-sized types.
// Both of these prohibitions are dumb, IMO.  `void` should have precisely one
// inhabitant, called `void()` and it should have zero size.  The address of a zero
// sized value is perfectly well-defined, though it would weaken the assumption that
// distinct objects have distinct addresses.
struct Empty { };

// Singleton instance of Empty
inline static Empty const empty;

// Empty is a singleton.
inline bool operator== (Empty const &lhs, Empty const &rhs) {
    return true;
}

// Empty is a singleton.
inline bool operator< (Empty const &lhs, Empty const &rhs) {
    return false;
}

inline std::ostream &operator<< (std::ostream &out, Empty const &) {
    return out << "Empty";
}

} // end namespace lvd

namespace std {

template <>
struct hash<lvd::Empty> {
    size_t operator() (lvd::Empty const &) const {
        return std::hash<std::string>()("Empty");
    }
};

} // end namespace std

