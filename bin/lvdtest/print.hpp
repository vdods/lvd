// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include <cstddef>
#include <map>
#include <ostream>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace std {

inline std::ostream &operator << (std::ostream &out, std::byte b) {
    return out << std::setw(2) << std::setfill('0') << std::hex << uint32_t(b);
}

template <typename T_, typename Allocator_>
inline ostream &operator << (ostream &out, vector<T_,Allocator_> const &v) {
    out << '[';
    bool has_printed_element = false;
    for (auto const &x : v) {
        if (has_printed_element)
            out << ", ";
        else
            has_printed_element = true;
        out << x;
    }
    return out << ']';
}

template <typename T_, size_t N_>
ostream &operator << (ostream &out, array<T_,N_> const &a) {
    out << '[';
    bool has_printed_element = false;
    for (auto x : a) {
        if (has_printed_element)
            out << ", ";
        else
            has_printed_element = true;
        out << x;
    }
    return out << ']';
}

template <typename F_, typename S_>
inline ostream &operator << (ostream &out, pair<F_,S_> const &p) {
    return out << '(' << p.first << ", " << p.second << ')';
}

template <typename K_, typename V_, typename Compare_, typename Allocator_>
inline ostream &operator << (ostream &out, map<K_,V_,Compare_,Allocator_> const &m) {
    out << '{';
    bool has_printed_element = false;
    for (auto const &p : m) {
        if (has_printed_element)
            out << ", ";
        else
            has_printed_element = true;
        out << p;
    }
    return out << '}';
}

template <typename K_, typename V_, typename Hash_, typename KeyEqual_, typename Allocator_>
inline ostream &operator << (ostream &out, unordered_map<K_,V_,Hash_,KeyEqual_,Allocator_> const &m) {
    out << '{';
    bool has_printed_element = false;
    for (auto const &p : m) {
        if (has_printed_element)
            out << ", ";
        else
            has_printed_element = true;
        out << p;
    }
    return out << '}';
}

template <typename K_, typename Compare_, typename Allocator_>
inline ostream &operator << (ostream &out, set<K_,Compare_,Allocator_> const &s) {
    out << '{';
    bool has_printed_element = false;
    for (auto const &x : s) {
        if (has_printed_element)
            out << ", ";
        else
            has_printed_element = true;
        out << x;
    }
    return out << '}';
}

template <typename K_, typename Hash_, typename KeyEqual_, typename Allocator_>
inline ostream &operator << (ostream &out, unordered_set<K_,Hash_,KeyEqual_,Allocator_> const &s) {
    out << '{';
    bool has_printed_element = false;
    for (auto const &x : s) {
        if (has_printed_element)
            out << ", ";
        else
            has_printed_element = true;
        out << x;
    }
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

} // end namespace std
