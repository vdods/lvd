// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <cstddef>
#include "lvd/encoding.hpp"
#include "lvd/type.hpp"
#include "lvd/type_string_of.hpp"
#include <istream>

namespace lvd {

// Template-specialize this in order to provide an implementation for populating a value
// using data from the stream.  Default implementation provides code for basic types.
// Should provide
//
// template <typename CharT_, typename Traits_>
// std::basic_istream<CharT_,Traits_> &operator() (
//      std::basic_istream<CharT_,Traits_> &in,
//      Encoding_ const &enc,
//      T_ &dest_val
// ) const
template <typename T_, typename Encoding_>
struct ReadInPlace_t;

// Template-specialize this in order to provide an implementation for producing a value
// from the stream.  This specialization is only needed for types that aren't default
// constructible.  The default implementation is for types that are default constructible.
// Should provide
//
// template <typename CharT_, typename Traits_>
// T_ operator() (std::basic_istream<CharT_,Traits_> &in, Encoding_ const &enc) const
template <typename T_, typename Encoding_>
struct ReadValue_t;

//
// Convenience functions for type deduction.
//

// NOTE: If you're getting a compile error like "invalid use of incomplete type...", then you
// need to include <lvd/read_XXX.hpp> for some XXX, e.g. bin_array or text_pair.
template <typename T_, typename CharT_, typename Traits_, typename Encoding_>
inline std::basic_istream<CharT_,Traits_> &read_in_place (std::basic_istream<CharT_,Traits_> &in, Encoding_ const &enc, T_ &dest_val) {
    return ReadInPlace_t<T_,Encoding_>()(in, enc, dest_val);
}

// NOTE: If you're getting a compile error like "invalid use of incomplete type...", then you
// need to include <lvd/read_XXX.hpp> for some XXX, e.g. bin_array or text_pair.
template <typename T_, typename CharT_, typename Traits_, typename Encoding_>
inline T_ read_value (std::basic_istream<CharT_,Traits_> &in, Encoding_ const &enc) {
    return ReadValue_t<T_,Encoding_>()(in, enc);
}

//
// Implementation of methods declared in encoding.hpp
//

// NOTE: If you're getting a compile error like "invalid use of incomplete type...", then you
// need to include <lvd/read_XXX.hpp> for some XXX, e.g. bin_array or text_pair.
template <typename CharT_, typename Traits_, typename T_, typename Encoding_>
std::basic_istream<CharT_,Traits_> &operator>> (std::basic_istream<CharT_,Traits_> &in, In_t<T_,Encoding_> const &i) {
    return read_in_place(in, i.encoding(), i.dest_val());
}

template <TypeEncoding TYPE_ENCODING_>
template <typename T_, typename CharT_, typename Traits_>
T_ BinEncoding_t<TYPE_ENCODING_>::read (std::basic_istream<CharT_,Traits_> &in) const {
    return read_value<T_>(in, *this);
}

template <TypeEncoding TYPE_ENCODING_>
template <typename T_, typename CharT_, typename Traits_>
T_ TextEncoding_t<TYPE_ENCODING_>::read (std::basic_istream<CharT_,Traits_> &in) const {
    return read_value<T_>(in, *this);
}

//
// This provides implementations for basic types.
//

template <typename T_, typename Encoding_>
struct ReadInPlace_Builtin_t;

template <typename T_, auto... Params_>
struct ReadInPlace_Builtin_t<T_,BinEncoding_t<Params_...>> {
    static_assert(is_endiannated_type_v<T_>);
    template <typename CharT_, typename Traits_>
    std::basic_istream<CharT_,Traits_> &operator() (std::basic_istream<CharT_,Traits_> &in, BinEncoding_t<Params_...> const &enc, T_ &dest_val) const {
        if constexpr (enc.type_encoding() == TypeEncoding::INCLUDED)
            in >> enc.with_demoted_type_encoding().in(type_of(dest_val)); // This will throw if the type doesn't match.

        static_assert(sizeof(CharT_) == 1, "only supporting chars of size 1 for now");

        if constexpr (std::is_same_v<T_,bool>) {
            dest_val = in.get() != 0;
        } else if constexpr (sizeof(T_) == 1) {
            dest_val = T_(in.get());
        } else {
            static_assert(sizeof(T_) > 1);
            in.read(reinterpret_cast<CharT_ *>(&dest_val), sizeof(dest_val));
            endian_change(enc.endianness(), machine_endianness(), dest_val);
        }
        return in;
    }
};

template <typename Encoding_> struct ReadInPlace_t<bool,Encoding_> : public ReadInPlace_Builtin_t<bool,Encoding_> { };
template <typename Encoding_> struct ReadInPlace_t<char,Encoding_> : public ReadInPlace_Builtin_t<char,Encoding_> { };
template <typename Encoding_> struct ReadInPlace_t<std::byte,Encoding_> : public ReadInPlace_Builtin_t<std::byte,Encoding_> { };
template <typename Encoding_> struct ReadInPlace_t<int8_t,Encoding_> : public ReadInPlace_Builtin_t<int8_t,Encoding_> { };
template <typename Encoding_> struct ReadInPlace_t<uint8_t,Encoding_> : public ReadInPlace_Builtin_t<uint8_t,Encoding_> { };
template <typename Encoding_> struct ReadInPlace_t<int16_t,Encoding_> : public ReadInPlace_Builtin_t<int16_t,Encoding_> { };
template <typename Encoding_> struct ReadInPlace_t<uint16_t,Encoding_> : public ReadInPlace_Builtin_t<uint16_t,Encoding_> { };
template <typename Encoding_> struct ReadInPlace_t<int32_t,Encoding_> : public ReadInPlace_Builtin_t<int32_t,Encoding_> { };
template <typename Encoding_> struct ReadInPlace_t<uint32_t,Encoding_> : public ReadInPlace_Builtin_t<uint32_t,Encoding_> { };
template <typename Encoding_> struct ReadInPlace_t<int64_t,Encoding_> : public ReadInPlace_Builtin_t<int64_t,Encoding_> { };
template <typename Encoding_> struct ReadInPlace_t<uint64_t,Encoding_> : public ReadInPlace_Builtin_t<uint64_t,Encoding_> { };
template <typename Encoding_> struct ReadInPlace_t<float,Encoding_> : public ReadInPlace_Builtin_t<float,Encoding_> { };
template <typename Encoding_> struct ReadInPlace_t<double,Encoding_> : public ReadInPlace_Builtin_t<double,Encoding_> { };
// TODO: Figure out how to define for long double in the case where long double == double.

// Default implementation works for default-constructible T_.
// NOTE: If you got a compile error saying "no match for call to lvd::ReadValue_t<...>" and it showed
// you that this candidate failed, then it means you have to implement ReadValue_t<T_,Encoding_> for
// your type T_ and for the encodings you care about.
template <typename T_, typename Encoding_>
struct ReadValue_t {
    template <typename CharT_, typename Traits_, typename = std::enable_if_t<std::is_default_constructible_v<T_>>>
    T_ operator() (std::basic_istream<CharT_,Traits_> &in, Encoding_ const &enc) const {
        T_ retval;
        ReadInPlace_t<T_,Encoding_>()(in, enc, retval);
        return retval;
    }
};

} // end namespace lvd
