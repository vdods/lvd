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
#include <utility>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lvd {

// TODO: Change to a design like random<T_> where custom serialization can be provided for any type.

//
// Forward declarations (dumb, but needed in certain cases)
//

// Convenience function to get a deserialized value.
template <typename T_, typename... Args_>
T_ deserialized (Args_&&... args);

// Serialize a built-in type as its raw bytes, changing to little endian if the machine is big endian.
template <typename T_, typename DestIterator_, typename = std::enable_if_t<std::is_integral_v<T_> || std::is_floating_point_v<T_> || sizeof(T_) == 1>>
void serialize (T_ source, DestIterator_ dest);

// Deserialize a built-in type as its raw bytes, changing from little endian if the machine is big endian.
template <typename T_, typename = std::enable_if_t<std::is_integral_v<T_> || std::is_floating_point_v<T_> || sizeof(T_) == 1>>
void deserialize (T_ &dest, auto &&source_range);

// Serialize a range.
template <typename SourceIterator_, typename DestIterator_>
void serialize (lvd::Range_t<SourceIterator_> const &source_range, DestIterator_ dest);

// Deserialize a range.
template <typename DestIterator_>
void deserialize (lvd::Range_t<DestIterator_> &dest_range, auto &&source_range);

// Serialize std::string.
template <typename DestIterator_>
void serialize (std::string const &source, DestIterator_ dest);

// Deserialize std::string.
inline void deserialize (std::string &dest, auto &&source_range);

// Serialize std::vector.
template <typename T_, typename Allocator_, typename DestIterator_>
void serialize (std::vector<T_,Allocator_> const &source, DestIterator_ dest);

// Deserialize std::vector.
template <typename T_, typename Allocator_>
void deserialize (std::vector<T_,Allocator_> &dest, auto &&source_range);

// Serialize std::pair
template <typename F_, typename S_, typename DestIterator_>
void serialize (std::pair<F_,S_> const &source, DestIterator_ dest);

// Deserialize std::pair
template <typename F_, typename S_>
void deserialize (std::pair<F_, S_> &dest, auto &&source_range);

// Serialize std::map
template <typename Key_, typename Value_, typename Compare_, typename Allocator_, typename DestIterator_>
void serialize (std::map<Key_,Value_,Compare_,Allocator_> const &source, DestIterator_ dest);

// Deserialize std::map
template <typename Key_, typename Value_, typename Compare_, typename Allocator_>
void deserialize (std::map<Key_,Value_,Compare_,Allocator_> &dest, auto &&source_range);

// Serialize std::set
template <typename Key_, typename Compare_, typename Allocator_, typename DestIterator_>
void serialize (std::set<Key_,Compare_,Allocator_> const &source, DestIterator_ dest);

// Deserialize std::set
template <typename Key_, typename Compare_, typename Allocator_>
void deserialize (std::set<Key_,Compare_,Allocator_> &dest, auto &&source_range);

// Serialize std::unordered_map
template <typename Key_, typename Value_, typename Hash_, typename KeyEqual_, typename Allocator_, typename DestIterator_>
void serialize (std::unordered_map<Key_,Value_,Hash_,KeyEqual_,Allocator_> const &source, DestIterator_ dest);

// Deserialize std::unordered_map
template <typename Key_, typename Value_, typename Hash_, typename KeyEqual_, typename Allocator_>
void deserialize (std::unordered_map<Key_,Value_,Hash_,KeyEqual_,Allocator_> &dest, auto &&source_range);

// Serialize std::unordered_set
template <typename Key_, typename Hash_, typename KeyEqual_, typename Allocator_, typename DestIterator_>
void serialize (std::unordered_set<Key_,Hash_,KeyEqual_,Allocator_> const &source, DestIterator_ dest);

// Deserialize std::unordered_set
template <typename Key_, typename Hash_, typename KeyEqual_, typename Allocator_>
void deserialize (std::unordered_set<Key_,Hash_,KeyEqual_,Allocator_> &dest, auto &&source_range);

//
// Implementations
//

// Convenience function to get a deserialized value.
template <typename T_, typename... Args_>
T_ deserialized (Args_&&... args) {
    T_ dest;
    deserialize(dest, std::forward<Args_>(args)...);
    return dest;
}

// Serialize a built-in type as its raw bytes.
template <typename T_, typename DestIterator_, typename = std::enable_if_t<std::is_integral_v<T_> || std::is_floating_point_v<T_> || sizeof(T_) == 1>>
void serialize (T_ source, DestIterator_ dest) {
    endian_change(machine_endianness(), Endianness::LITTLE, source);
    std::byte const *source_begin = reinterpret_cast<std::byte const *>(&source);
    std::byte const *source_end = source_begin + sizeof(T_);
    std::copy(source_begin, source_end, dest);
}

// Deserialize a built-in type from its raw bytes.
template <typename T_, typename = std::enable_if_t<std::is_integral_v<T_> || std::is_floating_point_v<T_> || sizeof(T_) == 1>>
void deserialize (T_ &dest, auto &&source_range) {
    // It's probably rude to abort upon an input error.  But for now, whateva.
    LVD_G_REQ_GEQ(size_t(source_range.size()), sizeof(T_), "source_range.size() is not large enough to read a value of the dest type");

    std::byte *dest_begin = reinterpret_cast<std::byte *>(&dest);
    std::copy(source_range.begin(), source_range.begin()+sizeof(T_), dest_begin);
    endian_change(Endianness::LITTLE, machine_endianness(), dest);
    // Advance source_range.begin() so it's ready to continue reading from the next spot.
    source_range.begin() += sizeof(T_);
}

// Serialize a range.
template <typename SourceIterator_, typename DestIterator_>
void serialize (lvd::Range_t<SourceIterator_> const &source_range, DestIterator_ dest) {
    LVD_G_REQ_LT(size_t(source_range.size()), 0x100000000ull, "source string is too big; this serialize function uses uint32_t for string length");

    if constexpr (std::is_trivially_copyable_v<decltype(*source_range.begin())>) {
        std::copy(source_range.begin(), source_range.begin()+source_range.size(), dest);
    } else {
        for (auto const &source : source_range)
            serialize(source, dest);
    }
}

// Deserialize a range.
template <typename DestIterator_>
void deserialize (lvd::Range_t<DestIterator_> &dest_range, auto &&source_range) {
    if constexpr (std::is_trivially_copyable_v<decltype(*dest_range.begin())>) {
        auto size = dest_range.size();
        // It's probably rude to abort upon an input error.  But for now, whateva.
        LVD_G_REQ_GEQ(source_range.size(), size, "source_range.size() is not large enough to read a value of the dest size");
        std::copy(source_range.begin(), source_range.begin()+size, dest_range.begin());
        source_range.begin() += size;
    } else {
        for (auto &dest : dest_range)
            deserialize(dest, std::forward<decltype(source_range)>(source_range));
    }
}

// Serialize std::string.
template <typename DestIterator_>
void serialize (std::string const &source, DestIterator_ dest) {
    serialize<uint32_t>(source.size(), dest);
    serialize(lvd::range(source.begin(), source.end()), dest);
}

// // Deserialize std::string.
// template <typename SourceIterator_>
// inline void deserialize (std::string &dest, lvd::Range_t<SourceIterator_> &source_range) {
//     dest.resize(deserialized<uint32_t>(source_range));
//     auto r = lvd::range(dest.begin(), dest.end());
//     deserialize(r, source_range);
// //     deserialize(lvd::range(dest.begin(), dest.end()), std::forward<decltype(source_range)>(source_range));
// }

// Deserialize std::string.
inline void deserialize (std::string &dest, auto &&source_range) {
    dest.resize(deserialized<uint32_t>(std::forward<decltype(source_range)>(source_range)));
    auto r = lvd::range(dest.begin(), dest.end());
    deserialize(r, std::forward<decltype(source_range)>(source_range));
}

// Serialize std::vector.
template <typename T_, typename Allocator_, typename DestIterator_>
void serialize (std::vector<T_,Allocator_> const &source, DestIterator_ dest) {
    serialize<uint32_t>(source.size(), dest);
    serialize(lvd::range(source.begin(), source.end()), dest);
}

// Deserialize std::vector.
template <typename T_, typename Allocator_>
void deserialize (std::vector<T_,Allocator_> &dest, auto &&source_range) {
    dest.resize(deserialized<uint32_t>(std::forward<decltype(source_range)>(source_range)));
    auto r = lvd::range(dest.begin(), dest.end());
    deserialize(r, std::forward<decltype(source_range)>(source_range));
//     deserialize(lvd::range(dest.begin(), dest.end()), std::forward<decltype(source_range)>(source_range));
}

// Serialize std::pair
template <typename F_, typename S_, typename DestIterator_>
void serialize (std::pair<F_,S_> const &source, DestIterator_ dest) {
    serialize(source.first, dest);
    serialize(source.second, dest);
}

// Deserialize std::pair
template <typename F_, typename S_>
void deserialize (std::pair<F_, S_> &dest, auto &&source_range) {
    deserialize(dest.first, std::forward<decltype(source_range)>(source_range));
    deserialize(dest.second, std::forward<decltype(source_range)>(source_range));
}

// Serialize std::map
template <typename Key_, typename Value_, typename Compare_, typename Allocator_, typename DestIterator_>
void serialize (std::map<Key_,Value_,Compare_,Allocator_> const &source, DestIterator_ dest) {
    serialize<uint32_t>(source.size(), dest);
    for (auto it : source)
        serialize(it, dest);
}

// Deserialize std::map
template <typename Key_, typename Value_, typename Compare_, typename Allocator_>
void deserialize (std::map<Key_,Value_,Compare_,Allocator_> &dest, auto &&source_range) {
    size_t size = deserialized<uint32_t>(std::forward<decltype(source_range)>(source_range));
    dest.clear();
    for (size_t i = 0; i < size; ++i)
        dest.emplace(deserialized<std::pair<Key_,Value_>>(std::forward<decltype(source_range)>(source_range)));
}

// Serialize std::set
template <typename Key_, typename Compare_, typename Allocator_, typename DestIterator_>
void serialize (std::set<Key_,Compare_,Allocator_> const &source, DestIterator_ dest) {
    serialize<uint32_t>(source.size(), dest);
    for (auto const &x : source)
        serialize(x, dest);
}

// Deserialize std::set
template <typename Key_, typename Compare_, typename Allocator_>
void deserialize (std::set<Key_,Compare_,Allocator_> &dest, auto &&source_range) {
    size_t size = deserialized<uint32_t>(std::forward<decltype(source_range)>(source_range));
    dest.clear();
    for (size_t i = 0; i < size; ++i)
        dest.emplace(deserialized<Key_>(std::forward<decltype(source_range)>(source_range)));
}

// Serialize std::unordered_map
template <typename Key_, typename Value_, typename Hash_, typename KeyEqual_, typename Allocator_, typename DestIterator_>
void serialize (std::unordered_map<Key_,Value_,Hash_,KeyEqual_,Allocator_> const &source, DestIterator_ dest) {
    serialize<uint32_t>(source.size(), dest);
    for (auto it : source)
        serialize(it, dest);
}

// Deserialize std::unordered_map
template <typename Key_, typename Value_, typename Hash_, typename KeyEqual_, typename Allocator_>
void deserialize (std::unordered_map<Key_,Value_,Hash_,KeyEqual_,Allocator_> &dest, auto &&source_range) {
    size_t size = deserialized<uint32_t>(std::forward<decltype(source_range)>(source_range));
    dest.clear();
    for (size_t i = 0; i < size; ++i)
        dest.emplace(deserialized<std::pair<Key_,Value_>>(std::forward<decltype(source_range)>(source_range)));
}

// Serialize std::unordered_set
template <typename Key_, typename Hash_, typename KeyEqual_, typename Allocator_, typename DestIterator_>
void serialize (std::unordered_set<Key_,Hash_,KeyEqual_,Allocator_> const &source, DestIterator_ dest) {
    serialize<uint32_t>(source.size(), dest);
    for (auto const &x : source)
        serialize(x, dest);
}

// Deserialize std::unordered_set
template <typename Key_, typename Hash_, typename KeyEqual_, typename Allocator_>
void deserialize (std::unordered_set<Key_,Hash_,KeyEqual_,Allocator_> &dest, auto &&source_range) {
    size_t size = deserialized<uint32_t>(std::forward<decltype(source_range)>(source_range));
    dest.clear();
    for (size_t i = 0; i < size; ++i)
        dest.emplace(deserialized<Key_>(std::forward<decltype(source_range)>(source_range)));
}

} // end namespace lvd
