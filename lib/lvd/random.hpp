// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <map>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lvd {

// Implementations should provide `void populate (T_ &dest, auto &rng, ...) const`, where ... is an optional is_valid function.
// TODO: This doesn't handle types that aren't default constructible.
template <typename T_> struct Random_t;

template <typename T_>
void populate_random (T_ &dest, auto &rng) {
    return Random_t<T_>().populate(dest, rng);
}

template <typename T_>
void populate_random (T_ &dest, auto &rng, auto is_valid) {
    do {
        Random_t<T_>().populate(dest, rng);
    } while (!is_valid(dest));
}

template <typename T_>
T_ make_random (auto &rng) {
    T_ retval;
    populate_random(retval, rng);
    return retval;
}

template <typename T_>
T_ make_random (auto &rng, auto is_valid) {
    T_ retval;
    populate_random(retval, rng, is_valid);
    return retval;
}

// Generates bitwise random values in the given trivially copyable type.
template <typename T_>
struct Random_t {
    void populate (T_ &dest, auto &rng) const {
        static_assert(std::is_trivially_copyable_v<T_>);
        // This isn't the fastest.
        static std::uniform_int_distribution<uint8_t> d(0x00, 0xFF);
        auto begin = reinterpret_cast<uint8_t *>(&dest);
        auto end = begin + sizeof(T_);
        for (auto it = begin; it < end; ++it)
            *it = d(rng);
    }
};

// Generates a random std::string of size within [0,10] and only printable ascii chars.
template <>
struct Random_t<std::string> {
    static bool ascii_char_is_printable (char c) { return ' ' <= c && c <= '~'; }

    void populate (std::string &dest, auto &rng) const {
        dest.resize(std::uniform_int_distribution<size_t>(0, 10)(rng), ' ');
        for (auto &c : dest)
            populate_random(c, rng, ascii_char_is_printable);
    }
};

// Generates a random std::vector of size within [0,10].
template <typename T_, typename Allocator_>
struct Random_t<std::vector<T_,Allocator_>> {
    void populate (std::vector<T_,Allocator_> &dest, auto &rng) const {
        dest.resize(std::uniform_int_distribution<size_t>(0, 10)(rng));
        for (auto &c : dest)
            populate_random(c, rng);
    }
};

// Generates a random std::array.
template <typename T_, size_t N_>
struct Random_t<std::array<T_,N_>> {
    void populate (std::array<T_,N_> &dest, auto &rng) const {
        for (auto &c : dest)
            populate_random(c, rng);
    }
};

// Generates a random std::pair.
template <typename F_, typename S_>
struct Random_t<std::pair<F_,S_>> {
    void populate (std::pair<F_,S_> &dest, auto &rng) const {
        populate_random(dest.first, rng);
        populate_random(dest.second, rng);
    }
};

// Generates a random std::map of size within [0,10].
template <typename K_, typename V_, typename Compare_, typename Allocator_>
struct Random_t<std::map<K_,V_,Compare_,Allocator_>> {
    void populate (std::map<K_,V_,Compare_,Allocator_> &dest, auto &rng) const {
        auto size = std::uniform_int_distribution<size_t>(0, 10)(rng);
        dest.clear();
        size_t i = 0;
        size_t constexpr WATCHDOG_LIMIT = 100;
        while (dest.size() < size && i < WATCHDOG_LIMIT) {
            dest.emplace(make_random<std::pair<K_,V_>>(rng));
            ++i;
        }
    }
};

// Generates a random std::unordered_map of size within [0,10].
template <typename K_, typename V_, typename Hash_, typename KeyEqual_, typename Allocator_>
struct Random_t<std::unordered_map<K_,V_,Hash_,KeyEqual_,Allocator_>> {
    void populate (std::unordered_map<K_,V_,Hash_,KeyEqual_,Allocator_> &dest, auto &rng) const {
        auto size = std::uniform_int_distribution<size_t>(0, 10)(rng);
        dest.clear();
        size_t i = 0;
        size_t constexpr WATCHDOG_LIMIT = 100;
        while (dest.size() < size && i < WATCHDOG_LIMIT) {
            dest.emplace(make_random<std::pair<K_,V_>>(rng));
            ++i;
        }
    }
};

// Generates a random std::set of size within [0,10].
template <typename K_, typename Compare_, typename Allocator_>
struct Random_t<std::set<K_,Compare_,Allocator_>> {
    void populate (std::set<K_,Compare_,Allocator_> &dest, auto &rng) const {
        auto size = std::uniform_int_distribution<size_t>(0, 10)(rng);
        dest.clear();
        size_t i = 0;
        size_t constexpr WATCHDOG_LIMIT = 100;
        while (dest.size() < size && i < WATCHDOG_LIMIT) {
            dest.emplace(make_random<K_>(rng));
            ++i;
        }
    }
};

// Generates a random std::unordered_set of size within [0,10].
template <typename K_, typename Hash_, typename KeyEqual_, typename Allocator_>
struct Random_t<std::unordered_set<K_,Hash_,KeyEqual_,Allocator_>> {
    void populate (std::unordered_set<K_,Hash_,KeyEqual_,Allocator_> &dest, auto &rng) const {
        auto size = std::uniform_int_distribution<size_t>(0, 10)(rng);
        dest.clear();
        size_t i = 0;
        size_t constexpr WATCHDOG_LIMIT = 100;
        while (dest.size() < size && i < WATCHDOG_LIMIT) {
            dest.emplace(make_random<K_>(rng));
            ++i;
        }
    }
};

} // end namespace lvd
