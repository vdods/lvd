// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include <cstddef>
#include "lvd/endian.hpp"
#include "lvd/g_req_context.hpp"
#include "lvd/Range_t.hpp"
#include <map>
#include <set>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

// TODO: Make separate header files for Serialization_t<T_> for different container types T_ so that
// this header doesn't include a zillion other headers.

namespace lvd {

//
// These templates are how serialization/deserialization are defined.  They use the same kind
// of pattern as std::hash<T>, in that implementations should provide an `operator()` method.
//

// Defines how to serialize T_.  Template-specialization should provide method
// `template <typename DestIterator_> void operator() (T_ const &source, DestIterator_ dest) const`
template <typename T_> struct SerializeFrom_t;
// Defines how to deserialize T_ in-place.  Requires T_ to be default-constructible.
// Template-specialization should provide method `void operator() (lvd::Range_t<Iterator_> &dest_range, auto &&source_range) const`
template <typename T_> struct DeserializeTo_t;
// Defines how to deserialize T_, returning by value.  Only needed if T_ is not default-constructible.
// Template-specialization should provide method `T_ operator() (auto &&source_range) const`.
template <typename T_> struct DeserializedTo_t;

//
// These are convenience functions that do type deduction and generally reduce boilerplate.
//

// Serialize a value into dest iterator, using the appropriate specialization of Serialization_t<T_>.
template <typename T_, typename DestIterator_>
void serialize_from (T_ const &source, DestIterator_ dest) {
    SerializeFrom_t<T_>()(source, dest);
}

// Deserialize a value from source_range, using the appropriate specialization of Serialization_t<T_>.
template <typename T_>
void deserialize_to (T_ &dest, auto &&source_range) {
    static_assert(is_Range_t<std::decay_t<decltype(source_range)>>, "source_range must be lvd::Range_t<Iterator_> for some type Iterator_");
    DeserializeTo_t<T_>()(dest, std::forward<decltype(source_range)>(source_range));
}

// Convenience function to get the serialized value as a std::vector<std::byte>.
template <typename T_>
std::vector<std::byte> serialized_from (T_ const &source) {
    std::vector<std::byte> retval;
    serialize_from(source, std::back_inserter(retval));
    return retval;
}

// Convenience function to get a deserialized value.
template <typename T_, typename... Args_>
T_ deserialized_to (Args_&&... args) {
    // We call the constructor explicitly here so that if T_ has an explicit constructor but the
    // implementation of DeserializedTo_t produces a different type, it still works.
    return T_(DeserializedTo_t<T_>()(std::forward<Args_>(args)...));
}

//
// Implementations of SerializeFrom_t<T_>, DeserializeTo_t<T_>, and DeserializedTo_t<T_> for various types.
//

template <typename T_>
inline bool constexpr is_basic_serializable_v = std::is_arithmetic_v<T_> || std::is_same_v<T_,std::byte>;

// This default implementation works only for integral or floating point types.
template <typename T_>
struct SerializeFrom_t {
    template <typename DestIterator_, typename = std::enable_if_t<is_basic_serializable_v<T_>>>
    void operator() (T_ source, DestIterator_ dest) const {
        // Swap byte order in-place, if called for.
        endian_change(machine_endianness(), Endianness::LITTLE, source);
        std::byte const *source_begin = reinterpret_cast<std::byte const *>(&source);
        std::byte const *source_end = source_begin + sizeof(T_);
        std::copy(source_begin, source_end, dest);
    }
};

// This default implementation works only for integral or floating point types.
template <typename T_>
struct DeserializeTo_t {
    template <typename = std::enable_if_t<is_basic_serializable_v<T_>>>
    void operator() (T_ &dest, auto &&source_range) const {
        static_assert(is_Range_t<std::decay_t<decltype(source_range)>>, "source_range must be lvd::Range_t<Iterator_> for some type Iterator_");
        // It's probably rude to abort upon an input error.  But for now, whateva.
        LVD_G_REQ_GEQ(size_t(source_range.size()), sizeof(T_), "source_range.size() is not large enough to read a value of the dest type");

        std::byte *dest_begin = reinterpret_cast<std::byte *>(&dest);
        std::copy(source_range.begin(), source_range.begin()+sizeof(T_), dest_begin);
        // Swap byte order in-place, if called for.
        endian_change(Endianness::LITTLE, machine_endianness(), dest);
        // Advance source_range.begin() so it's ready to continue reading from the next spot.
        source_range.begin() += sizeof(T_);
    }
};

// This default implementation works only for default-constructible types.
template <typename T_>
struct DeserializedTo_t {
    template <typename = std::enable_if_t<std::is_default_constructible_v<T_>>>
    T_ operator() (auto &&source_range) const {
        T_ retval;
        DeserializeTo_t<T_>()(retval, std::forward<decltype(source_range)>(source_range));
        return retval;
    }
};

//
// Note that lvd::Range_t<Iterator_> is used in a qualitatively different way than a container -- lvd::Range_t<Iterator_>
// doesn't [de]serialize the size of the range, whereas a container does.  Serialization for lvd::Range_t<Iterator_> is
// meant to be more low-level and raw.
//

template <typename Iterator_>
struct SerializeFrom_t<lvd::Range_t<Iterator_>> {
    template <typename DestIterator_>
    void operator() (lvd::Range_t<Iterator_> const &source_range, DestIterator_ dest) const {
        // TODO: Could add another case where the range represents a contiguous sequence of values
        // that could be byte-copied and then endian-corrected.
        if constexpr (std::is_same_v<Iterator_,std::byte const *>) {
            // If the range is over bytes, a direct copy of bytes works, no per-element byte-swapping necessary.
            std::copy(source_range.begin(), source_range.begin()+source_range.size(), dest);
        } else {
            for (auto const &source : source_range)
                serialize_from(source, dest);
        }
    }
};

template <typename Iterator_>
struct DeserializeTo_t<lvd::Range_t<Iterator_>> {
    void operator() (lvd::Range_t<Iterator_> &dest_range, auto &&source_range) const {
        static_assert(is_Range_t<std::decay_t<decltype(source_range)>>, "source_range must be lvd::Range_t<Iterator_> for some type Iterator_");
        // TODO: Could add another case where the range represents a contiguous sequence of values
        // that could be byte-copied and then endian-corrected.
        if constexpr (std::is_same_v<Iterator_,std::byte const *>) {
            // If the range is over bytes, a direct copy of bytes works, no per-element byte-swapping necessary.
            auto size = dest_range.size();
            std::copy(source_range.begin(), source_range.begin()+size, dest_range.begin());
            source_range.begin() += size;
        } else {
            for (auto &dest : dest_range)
                deserialize_to(dest, std::forward<decltype(source_range)>(source_range));
        }
    }
};

template <typename Iterator_I_AM_A_COMPILE_ERROR_>
struct DeserializedTo_t<lvd::Range_t<Iterator_I_AM_A_COMPILE_ERROR_>> {
    // operator() is intentionally unimplemented here in order to produce a compile error.
};

//
// Implementation helper for any seqentual container type that has begin(), end(), and a dynamic size.
//

template <typename Container_>
struct SerializeFrom_SequenceContainer_DynamicSize_t {
    template <typename DestIterator_>
    void operator() (Container_ const &source, DestIterator_ dest) const {
        LVD_G_REQ_LT(source.size(), 0x100000000ull, "source container is too big; this serialize function uses uint32_t for container size");
        serialize_from<uint32_t>(source.size(), dest);
        serialize_from(lvd::range(source), dest);
    }
};

template <typename Container_>
struct DeserializeTo_SequenceContainer_DynamicSize_t {
    void operator() (Container_ &dest, auto &&source_range) const {
        static_assert(is_Range_t<std::decay_t<decltype(source_range)>>, "source_range must be lvd::Range_t<Iterator_> for some type Iterator_");

        using ValueType = typename Container_::value_type;
        if constexpr (is_basic_serializable_v<ValueType>) {
            dest.resize(deserialized_to<uint32_t>(std::forward<decltype(source_range)>(source_range)));
            auto r = lvd::range(dest);
            deserialize_to(r, std::forward<decltype(source_range)>(source_range));
        } else {
            dest.clear();
            size_t size = deserialized_to<uint32_t>(std::forward<decltype(source_range)>(source_range));
            dest.reserve(size);
            for (size_t i = 0; i < size; ++i)
                dest.push_back(deserialized_to<ValueType>(std::forward<decltype(source_range)>(source_range)));
            assert(dest.size() == size);
        }
    }
};

//
// Definitions for specific sequential container types.
// -    std::basic_string
// -    std::vector
// -    NOTE: Would add std::deque here (though I'm not sure if the existing implementation would
//      support std::deque<std::byte>, since std::deque doesn't store its elements in a contiguous array.
//

template <typename... Types_>
struct SerializeFrom_t<std::basic_string<Types_...>> : SerializeFrom_SequenceContainer_DynamicSize_t<std::basic_string<Types_...>> { };
template <typename... Types_>
struct DeserializeTo_t<std::basic_string<Types_...>> : DeserializeTo_SequenceContainer_DynamicSize_t<std::basic_string<Types_...>> { };

template <typename... Types_>
struct SerializeFrom_t<std::vector<Types_...>> : SerializeFrom_SequenceContainer_DynamicSize_t<std::vector<Types_...>> { };
template <typename... Types_>
struct DeserializeTo_t<std::vector<Types_...>> : DeserializeTo_SequenceContainer_DynamicSize_t<std::vector<Types_...>> { };

//
// std::array<T,N>
//

template <typename T_, size_t N_>
struct SerializeFrom_t<std::array<T_,N_>> {
    template <typename DestIterator_>
    void operator() (std::array<T_,N_> const &source, DestIterator_ dest) const {
        serialize_from(lvd::range(source), dest);
    }
};

template <typename T_, size_t N_>
struct DeserializeTo_t<std::array<T_,N_>> {
    void operator() (std::array<T_,N_> &dest, auto &&source_range) const {
        static_assert(is_Range_t<std::decay_t<decltype(source_range)>>, "source_range must be lvd::Range_t<Iterator_> for some type Iterator_");
        auto r = lvd::range(dest);
        deserialize_to(r, std::forward<decltype(source_range)>(source_range));
    }
};

// TODO: Figure out how to implement DeserializedTo_t for std::array<T_,N_> where T_ is not default-constructible.

//
// std::pair<F,S>
//

template <typename F_, typename S_>
struct SerializeFrom_t<std::pair<F_,S_>> {
    template <typename DestIterator_>
    void operator() (std::pair<F_,S_> const &source, DestIterator_ dest) const {
        serialize_from(source.first, dest);
        serialize_from(source.second, dest);
    }
};

template <typename F_, typename S_>
struct DeserializeTo_t<std::pair<F_,S_>> {
    void operator() (std::pair<F_,S_> &dest, auto &&source_range) const {
        static_assert(is_Range_t<std::decay_t<decltype(source_range)>>, "source_range must be lvd::Range_t<Iterator_> for some type Iterator_");
        deserialize_to(dest.first, std::forward<decltype(source_range)>(source_range));
        deserialize_to(dest.second, std::forward<decltype(source_range)>(source_range));
    }
};

template <typename F_, typename S_>
struct DeserializedTo_t<std::pair<F_,S_>> {
    std::pair<F_,S_> operator() (auto &&source_range) const {
        if constexpr (std::is_default_constructible_v<F_> && std::is_default_constructible_v<S_>) {
            // This assumes that Container_ is default constructible.
            std::pair<F_,S_> retval;
            deserialize_to(retval, std::forward<decltype(source_range)>(source_range));
            return retval;
        } else {
            return std::pair<F_,S_>(
                deserialize_to<F_>(std::forward<decltype(source_range)>(source_range)),
                deserialize_to<S_>(std::forward<decltype(source_range)>(source_range))
            );
        }
    }
};

//
// Implementation helper for any associative container type.
//

template <typename Container_>
struct SerializeFrom_AssociativeContainer_t {
    template <typename DestIterator_>
    void operator() (Container_ const &source, DestIterator_ dest) const {
        LVD_G_REQ_LT(source.size(), 0x100000000ull, "source container is too big; this serialize function uses uint32_t for container size");
        serialize_from<uint32_t>(source.size(), dest);
        for (auto const &it : source)
            serialize_from(it, dest);
    }
};

template <typename T_>
struct _remove_cv_recursive {
    using type = std::remove_cv_t<T_>;
};

template <typename T_>
using _remove_cv_recursive_t = typename _remove_cv_recursive<T_>::type;

template <typename F_, typename S_>
struct _remove_cv_recursive<std::pair<F_,S_>> {
    using type = std::pair<_remove_cv_recursive_t<F_>,_remove_cv_recursive_t<S_>>;
};

template <typename Container_>
struct DeserializeTo_AssociativeContainer_t {
    void operator() (Container_ &dest, auto &&source_range) const {
        static_assert(is_Range_t<std::decay_t<decltype(source_range)>>, "source_range must be lvd::Range_t<Iterator_> for some type Iterator_");

        // _remove_cv_recursive is needed because for std::map and std::unordered_map, value_type is std::pair<Key_ const, T_>.
        using ValueType = _remove_cv_recursive_t<typename Container_::value_type>;
        size_t size = deserialized_to<uint32_t>(std::forward<decltype(source_range)>(source_range));
        dest.clear();
        for (size_t i = 0; i < size; ++i)
            dest.emplace(deserialized_to<ValueType>(std::forward<decltype(source_range)>(source_range)));
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
struct SerializeFrom_t<std::map<Types_...>> : public SerializeFrom_AssociativeContainer_t<std::map<Types_...>> { };
template <typename... Types_>
struct DeserializeTo_t<std::map<Types_...>> : public DeserializeTo_AssociativeContainer_t<std::map<Types_...>> { };

template <typename... Types_>
struct SerializeFrom_t<std::set<Types_...>> : public SerializeFrom_AssociativeContainer_t<std::set<Types_...>> { };
template <typename... Types_>
struct DeserializeTo_t<std::set<Types_...>> : public DeserializeTo_AssociativeContainer_t<std::set<Types_...>> { };

template <typename... Types_>
struct SerializeFrom_t<std::unordered_map<Types_...>> : public SerializeFrom_AssociativeContainer_t<std::unordered_map<Types_...>> { };
template <typename... Types_>
struct DeserializeTo_t<std::unordered_map<Types_...>> : public DeserializeTo_AssociativeContainer_t<std::unordered_map<Types_...>> { };

template <typename... Types_>
struct SerializeFrom_t<std::unordered_set<Types_...>> : public SerializeFrom_AssociativeContainer_t<std::unordered_set<Types_...>> { };
template <typename... Types_>
struct DeserializeTo_t<std::unordered_set<Types_...>> : public DeserializeTo_AssociativeContainer_t<std::unordered_set<Types_...>> { };

} // end namespace lvd
