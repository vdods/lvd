// 2020.04.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <functional>
#include <typeindex>

namespace lvd {

// From https://stackoverflow.com/questions/4948780/magic-number-in-boosthash-combine
// which is talking about boost::hash_combine, but I used the 64 bit magic number
// mentioned in the discussion comments.
template <typename T_>
void hash_combine (size_t &seed, T_ const &value) {
    seed ^= std::hash<T_>()(value) + 0x9e3779b97f4a7c16 + (seed << 6) + (seed >> 2);
}

// Base case for computing hash of sequence of values.
template <typename T_>
size_t hash (T_ const &value) {
    return std::hash<T_>()(value);
}

// Inductively compute hash of sequence of values.
template <typename First_, typename Second_, typename... Rest_>
size_t hash (First_ const &first, Second_ const &second, Rest_&&... rest) {
    size_t seed = std::hash<First_>()(first);
    seed ^= hash(second, std::forward<Rest_>(rest)...) + 0x9e3779b97f4a7c16 + (seed << 6) + (seed >> 2);
    return seed;
}

} // end namespace lvd

namespace std {

// It's silly that the C++ standard doesn't define this.

template <>
struct hash<type_info> {
    size_t operator () (type_info const &t) const {
        return lvd::hash(std::type_index(t));
    }
};

} // end namespace std
