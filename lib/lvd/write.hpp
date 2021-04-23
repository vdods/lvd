// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <cstddef>
#include <iomanip>
#include "lvd/Empty.hpp"
#include "lvd/encoding.hpp"
#include "lvd/literal.hpp"
#include "lvd/type_string_of.hpp"
#include <ostream>

namespace lvd {

// Template-specialize this in order to provide an implementation for writing a value to the stream.
// Should provide:
//
// template <typename CharT_, typename Traits_>
// std::basic_ostream<CharT_,Traits_> &operator() (
//      std::basic_ostream<CharT_,Traits_> &out,
//      Encoding_ const &enc,
//      T_ const &src_val
// ) const
template <typename T_, typename Encoding_>
struct WriteValue_t;

//
// Convenience function for type deduction.
//

// NOTE: If you're getting a compile error like "invalid use of incomplete type...", then you
// need to include <lvd/write_XXX.hpp> for some XXX, e.g. bin_array or text_pair.
template <typename CharT_, typename Traits_, typename T_, typename Encoding_>
std::basic_ostream<CharT_,Traits_> &write_value (std::basic_ostream<CharT_,Traits_> &out, Encoding_ const &enc, T_ const &src_val) {
    return WriteValue_t<T_,Encoding_>()(out, enc, src_val);
}

//
// Method definitions from lvd/encoding.hpp
//

// NOTE: If you're getting a compile error like "invalid use of incomplete type...", then you
// need to include <lvd/write_XXX.hpp> for some XXX, e.g. bin_array or text_pair.
template <typename CharT_, typename Traits_, typename T_, typename Encoding_>
std::basic_ostream<CharT_,Traits_> &operator<< (std::basic_ostream<CharT_,Traits_> &out, Out_t<T_,Encoding_> const &o) {
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
    template <typename CharT_, typename Traits_>
    std::basic_ostream<CharT_,Traits_> &operator() (std::basic_ostream<CharT_,Traits_> &out, BinEncoding_t<Params_...> const &enc, T_ src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));

        static_assert(sizeof(CharT_) == 1, "only supporting chars of size 1 for now");

        if constexpr (std::is_same_v<T_,bool>) {
            out.put(src_val ? uint8_t(1) : uint8_t(0));
        } else if constexpr (sizeof(T_) == 1) {
            out.put(char(src_val));
        } else {
            static_assert(sizeof(T_) > 1);
            endian_change(machine_endianness(), enc.endianness(), src_val);
            out.write(reinterpret_cast<CharT_ const *>(&src_val), sizeof(src_val));
        }
        return out;
    }
};

template <typename T_, auto... Params_>
struct WriteValue_Builtin_t<T_,TextEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_ostream<CharT_,Traits_> &operator() (std::basic_ostream<CharT_,Traits_> &out, TextEncoding_t<Params_...> const &enc, T_ src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << ty<T_> << '(';

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

//
// One for Empty, which has no content besides its type.
//

template <auto... Params_>
struct WriteValue_t<Empty,BinEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_ostream<CharT_,Traits_> &operator() (std::basic_ostream<CharT_,Traits_> &out, BinEncoding_t<Params_...> const &enc, Empty const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << enc.with_demoted_type_encoding().out(type_of(src_val));
        // No content.
        return out;
    }
};

template <auto... Params_>
struct WriteValue_t<Empty,TextEncoding_t<Params_...>> {
    template <typename CharT_, typename Traits_>
    std::basic_ostream<CharT_,Traits_> &operator() (std::basic_ostream<CharT_,Traits_> &out, TextEncoding_t<Params_...> const &enc, Empty const &src_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            out << ty<Empty>;
        // No content.
        return out;
    }
};

} // end namespace lvd
