// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include <cstddef>
#include "lvd/Range_t.hpp"
#include <string>
#include <type_traits>
#include <utility>

namespace lvd {

enum class Endianness : uint8_t {
    BIG = 0,
    LIL, // Because it's funnier this way (and shorter).

    __LOWEST__ = BIG,
    __HIGHEST__ = LIL,
};

inline std::ostream &operator << (std::ostream &out, Endianness e) {
    auto constexpr COUNT = size_t(Endianness::__HIGHEST__) - size_t(Endianness::__LOWEST__) + 1;
    static std::array<std::string,COUNT> const TABLE{
        "BIG",
        "LIL",
    };
    return out << TABLE.at(uint32_t(e));
}

// Reference: http://esr.ibiblio.org/?p=5095
inline bool constexpr machine_is_big_endian () {
    return (*(uint16_t *)"\0\xff" < 0x100);
}

inline bool constexpr machine_is_lil_endian () {
    return !machine_is_big_endian();
}

inline Endianness constexpr machine_endianness () {
    if (machine_is_big_endian())
        return Endianness::BIG;
    else
        return Endianness::LIL;
}

// Is true iff a type has endianness (arithmetic types) or is invariant under byte-order-swap (e.g. 1-byte types).
// TODO: Use std::is_scalar_v<T_> instead
template <typename T_>
inline bool constexpr is_endiannated_type_v = std::is_arithmetic_v<T_> || std::is_same_v<T_,std::byte> || std::is_same_v<T_,bool>;

template <typename T_, typename = std::enable_if_t<is_endiannated_type_v<T_>>>
void swap_byte_order_of (T_ &value) {
    // TODO: Allow for sizeof(long double)
    static_assert(sizeof(T_) == 1 || sizeof(T_) == 2 || sizeof(T_) == 4 || sizeof(T_) == 8, "can only swap_byte_order_of a type of size 1, 2, 4, or 8.");

    // TODO: Implement faster version, verify it produces minimal asm.
    // Reference: https://mklimenko.github.io/english/2018/08/22/robust-endian-swap/
    auto bytes = reinterpret_cast<std::byte *>(&value);
    if constexpr (sizeof(T_) == 1) {
        // Nothing to do.
    } else if constexpr (sizeof(T_) == 2) {
        std::swap(bytes[0], bytes[1]);
    } else if constexpr (sizeof(T_) == 4) {
        std::swap(bytes[0], bytes[3]);
        std::swap(bytes[1], bytes[2]);
    } else if constexpr (sizeof(T_) == 8) {
        std::swap(bytes[0], bytes[7]);
        std::swap(bytes[1], bytes[6]);
        std::swap(bytes[2], bytes[5]);
        std::swap(bytes[3], bytes[4]);
    }
}

template <typename T_, typename = std::enable_if_t<is_endiannated_type_v<T_>>>
T_ swapped_byte_order_of (T_ value) {
    swap_byte_order_of(value);
    return value;
}

template <typename T_, typename = std::enable_if_t<is_endiannated_type_v<T_>>>
void endian_change (Endianness from, Endianness to, T_ &value) {
    if (from != to)
        swap_byte_order_of(value);
}

// Call endian_change on each element in a range (operates in-place).
template <typename T_>
void endian_change (Endianness from, Endianness to, lvd::Range_t<T_> &value_range) {
    if (from != to)
        for (auto &value : value_range)
            swap_byte_order_of(value);
}

template <typename T_, typename = std::enable_if_t<is_endiannated_type_v<T_>>>
T_ endian_changed (Endianness from, Endianness to, T_ value) {
    endian_change(from, to, value);
    return value;
}

} // end namespace lvd
