// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/abort.hpp"
#include "lvd/remove_cv_recursive.hpp"
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
    // We call the constructor explicitly here so that if T_ has an explicit constructor but the
    // implementation of MakeRandom_t produces a different type, it still works.
    return T_(MakeRandom_t<T_>()(rng));
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

// Helper implementation for associative containers.  Generates a container having size between 0 and 10, inclusive.
template <typename Container_>
struct PopulateRandom_AssociativeContainer_t {
    void operator() (Container_ &dest, auto &rng) const {
        using ValueType = remove_cv_recursive_t<typename Container_::value_type>;
        auto desired_size = std::uniform_int_distribution<size_t>(0, 10)(rng);
        dest.clear();
        size_t i = 0;
        // WATCHDOG_LIMIT is needed because it's possible that the generated keys will collide,
        // and it's further possible that desired_size is not achievable (e.g. std::set<bool>
        // can't be any bigger than size 2).
        size_t constexpr WATCHDOG_LIMIT = 100;
        while (dest.size() < desired_size && i < WATCHDOG_LIMIT) {
            dest.emplace(make_random<ValueType>(rng));
            ++i;
        }
    }
};

//
// Definitions for specific associative containers.
// -    std::map
// -    std::set
// -    std::unordered_map
// -    std::unordered_set
// -    NOTE: Would add std::multimap, std::multiset, std::unordered_multimap, std::unordered_multiset here.
//

template <typename... Types_>
struct PopulateRandom_t<std::map<Types_...>> : public PopulateRandom_AssociativeContainer_t<std::map<Types_...>> { };
template <typename... Types_>
struct PopulateRandom_t<std::set<Types_...>> : public PopulateRandom_AssociativeContainer_t<std::set<Types_...>> { };
template <typename... Types_>
struct PopulateRandom_t<std::unordered_map<Types_...>> : public PopulateRandom_AssociativeContainer_t<std::unordered_map<Types_...>> { };
template <typename... Types_>
struct PopulateRandom_t<std::unordered_set<Types_...>> : public PopulateRandom_AssociativeContainer_t<std::unordered_set<Types_...>> { };

    }
};

} // end namespace lvd
