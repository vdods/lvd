// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include <cstddef>
#include <iomanip>
#include "lvd/encoding.hpp"
#include "lvd/endian.hpp"
#include "lvd/literal.hpp"
#include "lvd/remove_cv_recursive.hpp"
#include "lvd/TypeString_t.hpp" // TODO: Fix this, this is overkill for the time being
#include <map>
#include <ostream>
#include <set>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace lvd {

// Template-specialize this in order to provide an implementation for writing a value to the stream.
// Should provide `std::ostream &operator() (std::ostream &out, Encoding_ const &enc, T_ const &src_val) const`
template <typename T_, typename Encoding_>
struct WriteValue_t;

//
// Convenience function for type deduction.
//

template <typename T_, typename Encoding_>
std::ostream &write_value (std::ostream &out, Encoding_ const &enc, T_ const &src_val) {
    return WriteValue_t<T_,Encoding_>()(out, enc, src_val);
}

//
// Method definitions from lvd/encoding.hpp
//

template <typename T_, typename Encoding_>
std::ostream &operator<< (std::ostream &out, Out_t<T_,Encoding_> const &o) {
    return write_value(out, o.encoding(), o.src_val());
}

//
// This provides implementations of WriteValue_t for builtin types.
//

template <typename T_, typename Encoding_>
struct WriteValue_Builtin_t;

template <typename T_, auto... Params_>
struct WriteValue_Builtin_t<T_,BinEncoding_t<Params_...>> {
    static_assert(is_endiannated_type_v<T_>);
    std::ostream &operator() (std::ostream &out, BinEncoding_t<Params_...> const &enc, T_ src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));

        if constexpr (std::is_same_v<T_,bool>) {
            out.put(src_val ? uint8_t(1) : uint8_t(0));
        } else if constexpr (sizeof(T_) == 1) {
            out.put(char(src_val));
        } else {
            static_assert(sizeof(T_) > 1);
            endian_change(machine_endianness(), enc.endianness(), src_val);
            out.write(reinterpret_cast<char const *>(&src_val), sizeof(src_val));
        }
        return out;
    }
};

template <typename T_, auto... Params_>
struct WriteValue_Builtin_t<T_,TextEncoding_t<Params_...>> {
    std::ostream &operator() (std::ostream &out, TextEncoding_t<Params_...> const &enc, T_ src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << type_v<T_> << '(';

        if constexpr (std::is_same_v<T_,bool>) {
            out << (src_val ? "true" : "false");
        } else if constexpr (std::is_same_v<T_,std::byte>) {
            out << "0x" << LVD_FMT(std::hex << std::uppercase << std::setw(2) << std::setfill('0') << uint32_t(src_val));
        } else if constexpr (std::is_same_v<T_,char>) {
            out << literal_of(src_val);
        } else if constexpr (std::is_same_v<T_,int8_t>) {
            out << int32_t(src_val);
        } else if constexpr (std::is_same_v<T_,uint8_t>) {
            out << uint32_t(src_val);
        } else if constexpr (sizeof(T_) == 1) {
            static_assert(sizeof(T_) == -1, "unhandled case"); // Not sure how static_assert(false) is possible, so this is a hack
        } else if constexpr (std::is_integral_v<T_>) {
            out << src_val;
        } else if constexpr (std::is_floating_point_v<T_>) {
            out << LVD_FMT(std::scientific << std::setprecision(std::numeric_limits<T_>::max_digits10) << src_val);
        } else {
            static_assert(sizeof(T_) == -1, "unhandled case"); // Not sure how static_assert(false) is possible, so this is a hack
        }

        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << ')';
        return out;
    }
};

template <typename Encoding_> struct WriteValue_t<bool,Encoding_> : public WriteValue_Builtin_t<bool,Encoding_> { };
template <typename Encoding_> struct WriteValue_t<char,Encoding_> : public WriteValue_Builtin_t<char,Encoding_> { };
template <typename Encoding_> struct WriteValue_t<std::byte,Encoding_> : public WriteValue_Builtin_t<std::byte,Encoding_> { };
template <typename Encoding_> struct WriteValue_t<int8_t,Encoding_> : public WriteValue_Builtin_t<int8_t,Encoding_> { };
template <typename Encoding_> struct WriteValue_t<uint8_t,Encoding_> : public WriteValue_Builtin_t<uint8_t,Encoding_> { };
template <typename Encoding_> struct WriteValue_t<int16_t,Encoding_> : public WriteValue_Builtin_t<int16_t,Encoding_> { };
template <typename Encoding_> struct WriteValue_t<uint16_t,Encoding_> : public WriteValue_Builtin_t<uint16_t,Encoding_> { };
template <typename Encoding_> struct WriteValue_t<int32_t,Encoding_> : public WriteValue_Builtin_t<int32_t,Encoding_> { };
template <typename Encoding_> struct WriteValue_t<uint32_t,Encoding_> : public WriteValue_Builtin_t<uint32_t,Encoding_> { };
template <typename Encoding_> struct WriteValue_t<int64_t,Encoding_> : public WriteValue_Builtin_t<int64_t,Encoding_> { };
template <typename Encoding_> struct WriteValue_t<uint64_t,Encoding_> : public WriteValue_Builtin_t<uint64_t,Encoding_> { };
template <typename Encoding_> struct WriteValue_t<float,Encoding_> : public WriteValue_Builtin_t<float,Encoding_> { };
template <typename Encoding_> struct WriteValue_t<double,Encoding_> : public WriteValue_Builtin_t<double,Encoding_> { };

} // end namespace lvd