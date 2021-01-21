// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/abort.hpp"
#include <map>
#include <random>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lvd {

// TODO: Create some sort of RandomContext class which allows setting of various parameters,
// such as length and content of random strings, distributions to use on arithmetic types,
// iteration limits on is_valid-generation of various types, etc.

// Implementations should provide `void operator() (T_ &dest, auto &rng) const`, which
// populates dest in-place with a random value, and where ... is an optional is_valid function
// which determines what a valid value is; the result should satisfy is_valid(dest), or
// throw or abort.
template <typename T_> struct PopulateRandom_t;

// Implementations should provide `T_ operator() (auto &rng, ...) const`, which returns an
// appropriate random value, and where ... is an optional is_valid function which determines
// what a valid value is; the return value should satisfy is_valid(retval), or throw or abort.
// Note that if T_ is default constructible, then the default implementation of MakeRandom_t
// will use PopulateRandom_t.  Otherwise, template-specialize MakeRandom_t.
template <typename T_> struct MakeRandom_t;

template <typename T_>
void populate_random (T_ &dest, auto &rng) {
    return PopulateRandom_t<T_>()(dest, rng);
}

template <typename T_>
void populate_random (T_ &dest, auto &rng, auto is_valid) {
    auto pr = PopulateRandom_t<T_>();
    size_t constexpr ITERATION_LIMIT = 1000;
    for (size_t i = 0; i < ITERATION_LIMIT; ++i) {
        pr(dest, rng);
        if (is_valid(dest))
            return;
    }
    LVD_ABORT("`is_valid` version of populate_random reached iteration limit without generating a valid value");
}

// Convenience function which calls the appropriate template specialization of MakeRandom_t.
template <typename T_>
T_ make_random (auto &rng) {
    return MakeRandom_t<T_>()(rng);
}

// This is a form takes an `is_valid` function which is used to filter out values in
// a loop until one is produced.
template <typename T_>
T_ make_random (auto &rng, auto is_valid) {
    auto mr = MakeRandom_t<T_>();
    size_t constexpr ITERATION_LIMIT = 1000;
    for (size_t i = 0; i < ITERATION_LIMIT; ++i) {
        auto retval = mr(rng);
        if (is_valid(retval))
            return retval;
    }
    LVD_ABORT("`is_valid` version of populate_random reached iteration limit without generating a valid value");
}

//
// Default implementations of PopulateRandom_t and MakeRandom_t.
//

// Default implementation exists if T_ is trivially copyable.  If T_ is not trivially
// copyable, then you must provide your own template specialization of PopulateRandom_t.
template <typename T_>
struct PopulateRandom_t {
    template <typename = std::enable_if_t<std::is_trivially_copyable_v<T_>>>
    void operator() (T_ &dest, auto &rng) const {
        // I'm sure this could be make faster without much work.
        static std::uniform_int_distribution<uint8_t> d(0x00, 0xFF);
        auto begin = reinterpret_cast<uint8_t *>(&dest);
        auto end = begin + sizeof(T_);
        for (auto it = begin; it < end; ++it)
            *it = d(rng);
    }
};

// Default implementation using PopulateRandom_t<T_> exists if T_ is default constructible (which
// obviously depends on PopulateRandom_t<T_> having an implementation.  If T_ is not default
// constructible, then you must provide your own template specialization of MakeRandom_t.
template <typename T_>
struct MakeRandom_t {
    template <typename = std::enable_if_t<std::is_default_constructible_v<T_>>>
    T_ operator() (auto &rng) const {
        T_ retval;
        PopulateRandom_t<T_>()(retval, rng);
        return retval;
    }
};

//
// Other useful template specializations of PopulateRandom_t.  Each of these is default constructible,
// so no implementation of MakeRandom_t is needed.
//

// Generates a random std::string of size within [0,10] and only printable ascii chars.
template <>
struct PopulateRandom_t<std::string> {
    static bool ascii_char_is_printable (char c) { return ' ' <= c && c <= '~'; }

    void operator() (std::string &dest, auto &rng) const {
        dest.resize(std::uniform_int_distribution<size_t>(0, 10)(rng), ' ');
        for (auto &c : dest)
            populate_random(c, rng, ascii_char_is_printable);
    }
};

// Generates a random std::vector of size within [0,10].
template <typename... Types_>
struct PopulateRandom_t<std::vector<Types_...>> {
    using Vector = std::vector<Types_...>;
    void operator() (Vector &dest, auto &rng) const {
        dest.resize(std::uniform_int_distribution<size_t>(0, 10)(rng));
        for (auto &c : dest)
            populate_random(c, rng);
    }
};

// Generates a random std::array.
template <typename T_, size_t N_>
struct PopulateRandom_t<std::array<T_,N_>> {
    void operator() (std::array<T_,N_> &dest, auto &rng) const {
        for (auto &c : dest)
            populate_random(c, rng);
    }
};

// Generates a random std::pair.
template <typename F_, typename S_>
struct PopulateRandom_t<std::pair<F_,S_>> {
    void operator() (std::pair<F_,S_> &dest, auto &rng) const {
        populate_random(dest.first, rng);
        populate_random(dest.second, rng);
    }
};

// Generates a random std::map of size within [0,10].
template <typename... Types_>
struct PopulateRandom_t<std::map<Types_...>> {
    using Map = std::map<Types_...>;
    void operator() (Map &dest, auto &rng) const {
        using Key = typename Map::key_type;
        using Value = typename Map::mapped_type;
        using Pair = std::pair<Key,Value>;

        auto size = std::uniform_int_distribution<size_t>(0, 10)(rng);
        dest.clear();
        size_t i = 0;
        // WATCHDOG_LIMIT is needed because it's possible that the generated keys will collide.
        size_t constexpr WATCHDOG_LIMIT = 100;
        while (dest.size() < size && i < WATCHDOG_LIMIT) {
            dest.emplace(make_random<Pair>(rng));
            ++i;
        }
    }
};

// Generates a random std::unordered_map of size within [0,10].
template <typename... Types_>
struct PopulateRandom_t<std::unordered_map<Types_...>> {
    using UnorderedMap = std::unordered_map<Types_...>;
    void operator() (UnorderedMap &dest, auto &rng) const {
        using Key = typename UnorderedMap::key_type;
        using Value = typename UnorderedMap::mapped_type;
        using Pair = std::pair<Key,Value>;

        auto size = std::uniform_int_distribution<size_t>(0, 10)(rng);
        dest.clear();
        size_t i = 0;
        // WATCHDOG_LIMIT is needed because it's possible that the generated keys will collide.
        size_t constexpr WATCHDOG_LIMIT = 100;
        while (dest.size() < size && i < WATCHDOG_LIMIT) {
            dest.emplace(make_random<Pair>(rng));
            ++i;
        }
    }
};

// Generates a random std::set of size within [0,10].
template <typename... Types_>
struct PopulateRandom_t<std::set<Types_...>> {
    using Set = std::set<Types_...>;
    void operator() (Set &dest, auto &rng) const {
        using Key = typename Set::key_type;

        auto size = std::uniform_int_distribution<size_t>(0, 10)(rng);
        dest.clear();
        size_t i = 0;
        // WATCHDOG_LIMIT is needed because it's possible that the generated keys will collide.
        size_t constexpr WATCHDOG_LIMIT = 100;
        while (dest.size() < size && i < WATCHDOG_LIMIT) {
            dest.emplace(make_random<Key>(rng));
            ++i;
        }
    }
};

// Generates a random std::unordered_set of size within [0,10].
template <typename... Types_>
struct PopulateRandom_t<std::unordered_set<Types_...>> {
    using UnorderedSet = std::unordered_set<Types_...>;
    void operator() (UnorderedSet &dest, auto &rng) const {
        using Key = typename UnorderedSet::key_type;

        auto size = std::uniform_int_distribution<size_t>(0, 10)(rng);
        dest.clear();
        size_t i = 0;
        // WATCHDOG_LIMIT is needed because it's possible that the generated keys will collide.
        size_t constexpr WATCHDOG_LIMIT = 100;
        while (dest.size() < size && i < WATCHDOG_LIMIT) {
            dest.emplace(make_random<Key>(rng));
            ++i;
        }
    }
};

} // end namespace lvd
