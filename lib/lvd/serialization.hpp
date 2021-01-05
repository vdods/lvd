// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

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

// Implementations should provide serialize and deserialize methods.
template <typename T_> struct Serialization_t;

// Convenience function to get a deserialized value.
template <typename T_, typename... Args_>
T_ deserialized_to (Args_&&... args) {
    T_ dest;
    deserialize_to(dest, std::forward<Args_>(args)...);
    return dest;
}

// Serialize a value into dest iterator, using the appropriate specialization of Serialization_t<T_>.
template <typename T_, typename DestIterator_>
void serialize_from (T_ const &source, DestIterator_ dest) {
    Serialization_t<T_>().serialize(source, dest);
}

// Deserialize a value from source_range, using the appropriate specialization of Serialization_t<T_>.
template <typename T_>
void deserialize_to (T_ &dest, auto &&source_range) {
    Serialization_t<T_>().deserialize(dest, std::forward<decltype(source_range)>(source_range));
}

//
// Implementations of Serialization_t<T_>
//

template <typename T_>
inline bool constexpr is_basic_serializable_v = std::is_arithmetic_v<T_> || std::is_same_v<T_,std::byte>;

// This default implementation works only for integral or floating point types.
template <typename T_>
struct Serialization_t {
    template <typename DestIterator_, typename = std::enable_if_t<is_basic_serializable_v<T_>>>
    void serialize (T_ source, DestIterator_ dest) {
        // Swap byte order in-place, if called for.
        endian_change(machine_endianness(), Endianness::LITTLE, source);
        std::byte const *source_begin = reinterpret_cast<std::byte const *>(&source);
        std::byte const *source_end = source_begin + sizeof(T_);
        std::copy(source_begin, source_end, dest);
    }
    template <typename = std::enable_if_t<is_basic_serializable_v<T_>>>
    void deserialize (T_ &dest, auto &&source_range) {
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

template <typename Iterator_>
struct Serialization_t<lvd::Range_t<Iterator_>> {
    template <typename DestIterator_>
    void serialize (lvd::Range_t<Iterator_> const &source_range, DestIterator_ dest) {
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
    void deserialize (lvd::Range_t<Iterator_> &dest_range, auto &&source_range) {
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

template <typename Char_, typename Traits_, typename Allocator_>
struct Serialization_t<std::basic_string<Char_,Traits_,Allocator_>> {
    template <typename DestIterator_>
    void serialize (std::basic_string<Char_,Traits_,Allocator_> const &source, DestIterator_ dest) {
        LVD_G_REQ_LT(source.size(), 0x100000000ull, "source container is too big; this serialize function uses uint32_t for container size");
        serialize_from<uint32_t>(source.size(), dest);
        serialize_from(lvd::range(source.begin(), source.end()), dest);
    }
    void deserialize (std::basic_string<Char_,Traits_,Allocator_> &dest, auto &&source_range) {
        dest.resize(deserialized_to<uint32_t>(std::forward<decltype(source_range)>(source_range)));
        auto r = lvd::range(dest.begin(), dest.end());
        deserialize_to(r, std::forward<decltype(source_range)>(source_range));
    }
};

template <typename T_, typename Allocator_>
struct Serialization_t<std::vector<T_,Allocator_>> {
    template <typename DestIterator_>
    void serialize (std::vector<T_,Allocator_> const &source, DestIterator_ dest) {
        LVD_G_REQ_LT(source.size(), 0x100000000ull, "source container is too big; this serialize function uses uint32_t for container size");
        serialize_from<uint32_t>(source.size(), dest);
        serialize_from(lvd::range(source.begin(), source.end()), dest);
    }
    void deserialize (std::vector<T_,Allocator_> &dest, auto &&source_range) {
        dest.resize(deserialized_to<uint32_t>(std::forward<decltype(source_range)>(source_range)));
        auto r = lvd::range(dest.begin(), dest.end());
        deserialize_to(r, std::forward<decltype(source_range)>(source_range));
    }
};

template <typename F_, typename S_>
struct Serialization_t<std::pair<F_,S_>> {
    template <typename DestIterator_>
    void serialize (std::pair<F_,S_> const &source, DestIterator_ dest) {
        serialize_from(source.first, dest);
        serialize_from(source.second, dest);
    }
    void deserialize (std::pair<F_, S_> &dest, auto &&source_range) {
        deserialize_to(dest.first, std::forward<decltype(source_range)>(source_range));
        deserialize_to(dest.second, std::forward<decltype(source_range)>(source_range));
    }
};

template <typename Key_, typename Value_, typename Compare_, typename Allocator_>
struct Serialization_t<std::map<Key_,Value_,Compare_,Allocator_>> {
    template <typename DestIterator_>
    void serialize (std::map<Key_,Value_,Compare_,Allocator_> const &source, DestIterator_ dest) {
        LVD_G_REQ_LT(source.size(), 0x100000000ull, "source container is too big; this serialize function uses uint32_t for container size");
        serialize_from<uint32_t>(source.size(), dest);
        for (auto it : source)
            serialize_from(it, dest);
    }
    void deserialize (std::map<Key_,Value_,Compare_,Allocator_> &dest, auto &&source_range) {
        size_t size = deserialized_to<uint32_t>(std::forward<decltype(source_range)>(source_range));
        dest.clear();
        for (size_t i = 0; i < size; ++i)
            dest.emplace(deserialized_to<std::pair<Key_,Value_>>(std::forward<decltype(source_range)>(source_range)));
    }
};

template <typename Key_, typename Compare_, typename Allocator_>
struct Serialization_t<std::set<Key_,Compare_,Allocator_>> {
    template <typename DestIterator_>
    void serialize (std::set<Key_,Compare_,Allocator_> const &source, DestIterator_ dest) {
        LVD_G_REQ_LT(source.size(), 0x100000000ull, "source container is too big; this serialize function uses uint32_t for container size");
        serialize_from<uint32_t>(source.size(), dest);
        for (auto const &x : source)
            serialize_from(x, dest);
    }
    void deserialize (std::set<Key_,Compare_,Allocator_> &dest, auto &&source_range) {
        size_t size = deserialized_to<uint32_t>(std::forward<decltype(source_range)>(source_range));
        dest.clear();
        for (size_t i = 0; i < size; ++i)
            dest.emplace(deserialized_to<Key_>(std::forward<decltype(source_range)>(source_range)));
    }
};

template <typename Key_, typename Value_, typename Hash_, typename KeyEqual_, typename Allocator_>
struct Serialization_t<std::unordered_map<Key_,Value_,Hash_,KeyEqual_,Allocator_>> {
    template <typename DestIterator_>
    void serialize (std::unordered_map<Key_,Value_,Hash_,KeyEqual_,Allocator_> const &source, DestIterator_ dest) {
        LVD_G_REQ_LT(source.size(), 0x100000000ull, "source container is too big; this serialize function uses uint32_t for container size");
        serialize_from<uint32_t>(source.size(), dest);
        for (auto it : source)
            serialize_from(it, dest);
    }
    void deserialize (std::unordered_map<Key_,Value_,Hash_,KeyEqual_,Allocator_> &dest, auto &&source_range) {
        size_t size = deserialized_to<uint32_t>(std::forward<decltype(source_range)>(source_range));
        dest.clear();
        for (size_t i = 0; i < size; ++i)
            dest.emplace(deserialized_to<std::pair<Key_,Value_>>(std::forward<decltype(source_range)>(source_range)));
    }
};

template <typename Key_, typename Hash_, typename KeyEqual_, typename Allocator_>
struct Serialization_t<std::unordered_set<Key_,Hash_,KeyEqual_,Allocator_>> {
    template <typename DestIterator_>
    void serialize (std::unordered_set<Key_,Hash_,KeyEqual_,Allocator_> const &source, DestIterator_ dest) {
        LVD_G_REQ_LT(source.size(), 0x100000000ull, "source container is too big; this serialize function uses uint32_t for container size");
        serialize_from<uint32_t>(source.size(), dest);
        for (auto const &x : source)
            serialize_from(x, dest);
    }
    void deserialize (std::unordered_set<Key_,Hash_,KeyEqual_,Allocator_> &dest, auto &&source_range) {
        size_t size = deserialized_to<uint32_t>(std::forward<decltype(source_range)>(source_range));
        dest.clear();
        for (size_t i = 0; i < size; ++i)
            dest.emplace(deserialized_to<Key_>(std::forward<decltype(source_range)>(source_range)));
    }
};

} // end namespace lvd
