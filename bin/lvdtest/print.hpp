// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include <cstddef>
#include "lvd/comma.hpp"
#include "lvd/IndexedTuple_t.hpp"
#include <map>
#include <ostream>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace std {

inline std::ostream &operator << (std::ostream &out, std::byte b) {
    return out << std::setw(2) << std::setfill('0') << std::hex << uint32_t(b);
}

template <typename T_, typename Allocator_>
inline ostream &operator << (ostream &out, vector<T_,Allocator_> const &v) {
    auto d = lvd::make_comma_space_delimiter();
    out << '[';
    for (auto const &x : v)
        out << d << x;
    return out << ']';
}

template <typename T_, size_t N_>
ostream &operator << (ostream &out, array<T_,N_> const &a) {
    auto d = lvd::make_comma_space_delimiter();
    out << '[';
    for (auto x : a)
        out << d << x;
    return out << ']';
}

template <typename F_, typename S_>
inline ostream &operator << (ostream &out, pair<F_,S_> const &p) {
    return out << '(' << p.first << ", " << p.second << ')';
}

template <typename... Types_>
inline ostream &operator << (ostream &out, tuple<Types_...> const &t) {
    out << '(';
    lvd::begin_indexed_tuple(t).print(out);
    return out << ')';
}

template <typename K_, typename V_, typename Compare_, typename Allocator_>
inline ostream &operator << (ostream &out, map<K_,V_,Compare_,Allocator_> const &m) {
    auto d = lvd::make_comma_space_delimiter();
    out << '{';
    for (auto const &p : m)
        out << d << p;
    return out << '}';
}

template <typename K_, typename V_, typename Hash_, typename KeyEqual_, typename Allocator_>
inline ostream &operator << (ostream &out, unordered_map<K_,V_,Hash_,KeyEqual_,Allocator_> const &m) {
    auto d = lvd::make_comma_space_delimiter();
    out << '{';
    for (auto const &p : m)
        out << d << p;
    return out << '}';
}

template <typename K_, typename Compare_, typename Allocator_>
inline ostream &operator << (ostream &out, set<K_,Compare_,Allocator_> const &s) {
    auto d = lvd::make_comma_space_delimiter();
    out << '{';
    for (auto const &x : s)
        out << d << x;
    return out << '}';
}

template <typename K_, typename Hash_, typename KeyEqual_, typename Allocator_>
inline ostream &operator << (ostream &out, unordered_set<K_,Hash_,KeyEqual_,Allocator_> const &s) {
    auto d = lvd::make_comma_space_delimiter();
    out << '{';
    for (auto const &x : s)
        out << d << x;
    return out << '}';
}

template <typename T_>
inline ostream &operator << (ostream &out, optional<T_> const &x) {
    out << "Optional(";
    if (x.has_value())
        out << x.value();
    else
        out << "<no-value>";
    return out << ')';
}

inline ostream &operator << (ostream &out, type_index const &ti) {
    return out << ti.name();
}

} // end namespace std
