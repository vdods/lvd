// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <cstddef>
#include "lvd/abort.hpp"
#include "lvd/IndexedTuple_t.hpp"
#include "lvd/remove_cv_recursive.hpp"
#include <map>
#include <optional>
#include <random>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace lvd {

// TODO: Create some sort of RandomContext class which allows setting of various parameters,
// such as length and content of random strings, distributions to use on arithmetic types,
// iteration limits on is_valid-generation of various types, etc.  This would be a template
// parameter after T_ in the various functions, and if there were any state to that struct,
// such as specific params for distributions, then that would be passed in after rng.

// Implementations should provide `void operator() (T_ &dest, Rng_ &rng) const`, which
// populates dest in-place with a random value, and where ... is an optional is_valid function
// which determines what a valid value is; the result should satisfy is_valid(dest), or
// throw or abort.
template <typename T_> struct PopulateRandom_t;

// Implementations should provide `T_ operator() (Rng_ &rng, ...) const`, which returns an
// appropriate random value, and where ... is an optional is_valid function which determines
// what a valid value is; the return value should satisfy is_valid(retval), or throw or abort.
// Note that if T_ is default constructible, then the default implementation of MakeRandom_t
// will use PopulateRandom_t.  Otherwise, template-specialize MakeRandom_t.
template <typename T_> struct MakeRandom_t;

template <typename T_, typename Rng_>
void populate_random (T_ &dest, Rng_ &rng) {
    return PopulateRandom_t<T_>()(dest, rng);
}

template <typename T_, typename Rng_, typename IsValid_>
void populate_random (T_ &dest, Rng_ &rng, IsValid_ is_valid) {
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
template <typename T_, typename Rng_>
T_ make_random (Rng_ &rng) {
    // We call the constructor explicitly here so that if T_ has an explicit constructor but the
    // implementation of MakeRandom_t produces a different type, it still works.
    return T_(MakeRandom_t<T_>()(rng));
}

// This is a form takes an `is_valid` function which is used to filter out values in
// a loop until one is produced.
template <typename T_, typename Rng_, typename IsValid_>
T_ make_random (Rng_ &rng, IsValid_ is_valid) {
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

// Need to specify implementation for bool specifically, since bool apparently can store values
// outside of just 0 and 1.
template <>
struct PopulateRandom_t<bool> {
    template <typename Rng_>
    void operator() (bool &dest, Rng_ &rng) const {
        // There's probably a faster way to generate a single random bit.
        static std::uniform_int_distribution<uint8_t> d(0, 1);
        dest = d(rng) != 0;
    }
};

// Need to specify implementation for std::byte, since it's not considered an arithmetic type.
template <>
struct PopulateRandom_t<std::byte> {
    template <typename Rng_>
    void operator() (std::byte &dest, Rng_ &rng) const {
        static std::uniform_int_distribution<uint8_t> d;
        dest = std::byte(d(rng));
    }
};

//
// Implementation for integral types.
//

// Helper template
template <typename Int_>
struct PopulateRandom_Int_t {
    template <typename Rng_>
    void operator() (Int_ &dest, Rng_ &rng) const {
        static_assert(std::is_integral_v<Int_>);
        static std::uniform_int_distribution<Int_> d;
        dest = d(rng);
    }
};

template <>
struct PopulateRandom_t<char> : public PopulateRandom_Int_t<char> { };
template <>
struct PopulateRandom_t<long long int> : public PopulateRandom_Int_t<long long int> { };
template <>
struct PopulateRandom_t<unsigned long long int> : public PopulateRandom_Int_t<unsigned long long int> { };
template <>
struct PopulateRandom_t<int8_t> : public PopulateRandom_Int_t<int8_t> { };
template <>
struct PopulateRandom_t<uint8_t> : public PopulateRandom_Int_t<uint8_t> { };
template <>
struct PopulateRandom_t<int16_t> : public PopulateRandom_Int_t<int16_t> { };
template <>
struct PopulateRandom_t<uint16_t> : public PopulateRandom_Int_t<uint16_t> { };
template <>
struct PopulateRandom_t<int32_t> : public PopulateRandom_Int_t<int32_t> { };
template <>
struct PopulateRandom_t<uint32_t> : public PopulateRandom_Int_t<uint32_t> { };
template <>
struct PopulateRandom_t<int64_t> : public PopulateRandom_Int_t<int64_t> { };
template <>
struct PopulateRandom_t<uint64_t> : public PopulateRandom_Int_t<uint64_t> { };

//
// Need to specify implementation for floating point types specifically, to avoid generating a NaN or infinity.
//

// Helper template
template <typename Float_>
struct PopulateRandom_Float_t {
    template <typename Rng_>
    void operator() (Float_ &dest, Rng_ &rng) const {
        static_assert(std::is_floating_point_v<Float_>);
        static std::normal_distribution<float> d(Float_(0));
        dest = d(rng);
    }
};

template <>
struct PopulateRandom_t<float> : public PopulateRandom_Float_t<float> { };
template <>
struct PopulateRandom_t<double> : public PopulateRandom_Float_t<double> { };
template <>
struct PopulateRandom_t<long double> : public PopulateRandom_Float_t<long double> { };

// Default implementation using PopulateRandom_t<T_> exists if T_ is default constructible (which
// obviously depends on PopulateRandom_t<T_> having an implementation.  If T_ is not default
// constructible, then you must provide your own template specialization of MakeRandom_t.
template <typename T_>
struct MakeRandom_t {
    template <typename Rng_, typename = std::enable_if_t<std::is_default_constructible_v<T_>>>
    T_ operator() (Rng_ &rng) const {
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

    template <typename Rng_>
    void operator() (std::string &dest, Rng_ &rng) const {
        dest.resize(std::uniform_int_distribution<size_t>(0, 10)(rng), ' ');
        for (auto &c : dest)
            populate_random(c, rng, ascii_char_is_printable);
    }
};

// Generates a random std::vector of size within [0,10].
template <typename... Types_>
struct PopulateRandom_t<std::vector<Types_...>> {
    using Vector = std::vector<Types_...>;
    template <typename Rng_>
    void operator() (Vector &dest, Rng_ &rng) const {
        dest.resize(std::uniform_int_distribution<size_t>(0, 10)(rng));
        for (auto &c : dest)
            populate_random(c, rng);
    }
};

// Generates a random std::array.
template <typename T_, size_t N_>
struct PopulateRandom_t<std::array<T_,N_>> {
    template <typename Rng_>
    void operator() (std::array<T_,N_> &dest, Rng_ &rng) const {
        for (auto &c : dest)
            populate_random(c, rng);
    }
};

// Generates a random std::pair.
template <typename F_, typename S_>
struct PopulateRandom_t<std::pair<F_,S_>> {
    template <typename Rng_>
    void operator() (std::pair<F_,S_> &dest, Rng_ &rng) const {
        populate_random(dest.first, rng);
        populate_random(dest.second, rng);
    }
};

// Generates a random std::tuple.
template <size_t INDEX_, typename... Types_>
struct PopulateRandom_t<IndexedTuple_t<INDEX_,Types_...>> {
    template <typename Rng_>
    void operator() (IndexedTuple_t<INDEX_,Types_...> &dest, Rng_ &rng) const {
        if constexpr (!dest.has_ended()) {
            populate_random(dest.current(), rng);
            populate_random(dest.incremented(), rng);
        }
    }
};

// Generates a random std::tuple.
template <typename... Types_>
struct PopulateRandom_t<std::tuple<Types_...>> {
    template <typename Rng_>
    void operator() (std::tuple<Types_...> &dest, Rng_ &rng) const {
        populate_random(begin_indexed_tuple(dest), rng);
    }
};

// Helper implementation for associative containers.  Generates a container having size between 0 and 10, inclusive.
template <typename Container_>
struct PopulateRandom_AssociativeContainer_t {
    template <typename Rng_>
    void operator() (Container_ &dest, Rng_ &rng) const {
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

// Generates a random std::optional with 75% likelihood of holding a value.
template <typename T_>
struct PopulateRandom_t<std::optional<T_>> {
    template <typename Rng_>
    void operator() (std::optional<T_> &dest, Rng_ &rng) const {
        bool has_value = std::uniform_int_distribution<size_t>(0, 3)(rng) != 0;
        if (has_value)
            dest = make_random<T_>(rng);
        else
            dest = std::nullopt;
    }
};

} // end namespace lvd
