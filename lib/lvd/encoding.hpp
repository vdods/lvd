// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
// #include <cstddef>
// #include <iomanip>
#include <istream>
#include "lvd/endian.hpp"
// #include "lvd/literal.hpp"
// #include "lvd/remove_cv_recursive.hpp"
// #include "lvd/Type_t.hpp"
// #include "lvd/TypeString_t.hpp"
#include <ostream>
#include <string>
// #include <type_traits>

namespace lvd {

// // Not sure if this is actually useful.
// enum class Format : uint8_t {
//     BINARY = 0,
//     TEXT,
//
//     __LOWEST__ = BINARY,
//     __HIGHEST__ = TEXT
// };
//
// inline std::string const &as_string (Format x) {
//     auto constexpr COUNT = size_t(Format::__HIGHEST__) - size_t(Format::__LOWEST__) + 1;
//     static std::array<std::string,COUNT> const TABLE{
//         "BINARY",
//         "TEXT",
//     };
//     return TABLE.at(size_t(x));
// }
//
// inline std::ostream &operator << (std::ostream &out, Format x) {
//     return out << as_string(x);
// }

// Enum for specifying if type information should be included in the encoding or not.
enum class TypeEncoding : uint8_t {
    EXCLUDED = 0,   // Types are known in context (i.e. static types)
    CONDITIONAL,    // Types are included when necessary (i.e. types of elements of containers are elided)
    INCLUDED,       // Types are included in the encoding (i.e. dynamic types)

    __LOWEST__ = EXCLUDED,
    __HIGHEST__ = INCLUDED
};

inline std::string const &as_string (TypeEncoding x) {
    auto constexpr COUNT = size_t(TypeEncoding::__HIGHEST__) - size_t(TypeEncoding::__LOWEST__) + 1;
    static std::array<std::string,COUNT> const TABLE{
        "EXCLUDED",
        "CONDITIONAL",
        "INCLUDED",
    };
    return TABLE.at(size_t(x));
}

inline std::ostream &operator << (std::ostream &out, TypeEncoding x) {
    return out << as_string(x);
}


// This type facilitates >> syntax.
template <typename T_, typename Encoding_>
class In_t {
public:

    using Encoding = Encoding_;

    In_t () = delete;
    In_t (In_t const &) = delete;
    In_t (In_t &&) = delete;
    explicit In_t (T_ &dest_val, Encoding const &encoding)
        :   m_dest_val(dest_val)
        ,   m_encoding(encoding)
    { }

    T_ &dest_val () const { return m_dest_val; }
    Encoding const &encoding () const { return m_encoding; }

private:

    T_ &m_dest_val;
    Encoding const &m_encoding;
};

// This is implemented in lvd/read.hpp, which must be included or suffer a linker error.
template <typename T_, typename Encoding_>
std::istream &operator>> (std::istream &in, In_t<T_,Encoding_> const &i);

// This type facilitates << syntax.
template <typename T_, typename Encoding_>
class Out_t {
public:

    using Encoding = Encoding_;

    Out_t () = delete;
    Out_t (Out_t const &) = delete;
    Out_t (Out_t &&) = delete;
    explicit Out_t (T_ const &src_val, Encoding const &encoding) : m_src_val(src_val), m_encoding(encoding) { }

    T_ const &src_val () const { return m_src_val; }
    Encoding const &encoding () const { return m_encoding; }

private:

    T_ const &m_src_val;
    Encoding const &m_encoding;
};

// This is implemented in lvd/write.hpp, which must be included or suffer a linker error.
template <typename T_, typename Encoding_>
std::ostream &operator<< (std::ostream &out, Out_t<T_,Encoding_> const &o);


// Binary encoding with endianness state variable.
// TODO: Make one where Endianness is a template param.
// TODO: Maybe turn TYPE_ENCODING_ template param into a runtime variable.
template <TypeEncoding TYPE_ENCODING_>
class BinEncoding_t {
public:

    BinEncoding_t (Endianness endianness) : m_endianness(endianness) { }

    // For use in >> input syntax.  Must include lvd/read_bin.hpp for this to link.
    template <typename T_>
    In_t<T_,BinEncoding_t> in (T_ &dest_val) const {
        return In_t<T_,BinEncoding_t>(dest_val, *this);
    }
    // For use in << output syntax.  Must include lvd/write.hpp for this to link.
    template <typename T_>
    Out_t<T_,BinEncoding_t> out (T_ const &src_val) const {
        return Out_t<T_,BinEncoding_t>(src_val, *this);
    }
    // For reading from a stream and producing a value, instead of populating it in-place.
    // Must include lvd/read.hpp for this to link.
    template <typename T_>
    T_ read (std::istream &in) const;

    constexpr TypeEncoding type_encoding () const { return TYPE_ENCODING_; }
    Endianness endianness () const { return m_endianness; }

    template <TypeEncoding OTHER_TYPE_ENCODING_>
    BinEncoding_t<OTHER_TYPE_ENCODING_> with_type_encoding () const {
        return BinEncoding_t<OTHER_TYPE_ENCODING_>(this->endianness());
    }

    // For use when eliding type info for nested elements, where the type info is known from context.
    decltype(auto) with_demoted_type_encoding () const {
        if constexpr (TYPE_ENCODING_ == TypeEncoding::INCLUDED)
            return BinEncoding_t<TypeEncoding::CONDITIONAL>(this->endianness());
        else
            return *this;
    }

private:

    Endianness m_endianness;
};

// Some convenient default singleton objects.
inline static BinEncoding_t<TypeEncoding::EXCLUDED> const bin_big_e = BinEncoding_t<TypeEncoding::EXCLUDED>(Endianness::BIG);
inline static BinEncoding_t<TypeEncoding::EXCLUDED> const bin_lil_e = BinEncoding_t<TypeEncoding::EXCLUDED>(Endianness::LIL);
inline static BinEncoding_t<TypeEncoding::EXCLUDED> const bin_machine_e = BinEncoding_t<TypeEncoding::EXCLUDED>(machine_endianness());
inline static BinEncoding_t<TypeEncoding::INCLUDED> const tbin_big_e = BinEncoding_t<TypeEncoding::INCLUDED>(Endianness::BIG);
inline static BinEncoding_t<TypeEncoding::INCLUDED> const tbin_lil_e = BinEncoding_t<TypeEncoding::INCLUDED>(Endianness::LIL);
inline static BinEncoding_t<TypeEncoding::INCLUDED> const tbin_machine_e = BinEncoding_t<TypeEncoding::INCLUDED>(machine_endianness());

// Human-readable text encoding.
template <TypeEncoding TYPE_ENCODING_>
class TextEncoding_t {
public:

    // For use in >> input syntax.  Must include lvd/read_bin.hpp for this to link.
    template <typename T_>
    In_t<T_,TextEncoding_t> in (T_ &dest_val) const {
        return In_t<T_,TextEncoding_t>(dest_val, *this);
    }
    // For use in << output syntax.  Must include lvd/write.hpp for this to link.
    template <typename T_>
    Out_t<T_,TextEncoding_t> out (T_ const &src_val) const {
        return Out_t<T_,TextEncoding_t>(src_val, *this);
    }
    // For reading from a stream and producing a value, instead of populating it in-place.
    // Must include lvd/read.hpp for this to link.
    template <typename T_>
    T_ read (std::istream &in) const;

    constexpr TypeEncoding type_encoding () const { return TYPE_ENCODING_; }

    template <TypeEncoding OTHER_TYPE_ENCODING_>
    TextEncoding_t<OTHER_TYPE_ENCODING_> with_type_encoding () const {
        return TextEncoding_t<OTHER_TYPE_ENCODING_>();
    }

    // For use when eliding type info for nested elements, where the type info is known from context.
    decltype(auto) with_demoted_type_encoding () const {
        if constexpr (TYPE_ENCODING_ == TypeEncoding::INCLUDED)
            return TextEncoding_t<TypeEncoding::CONDITIONAL>();
        else
            return *this;
    }
};

// Some convenient default singleton objects.  `lit` denotes literal, as in value literal.
// `txt` denotes a less formal value literal.
inline static TextEncoding_t<TypeEncoding::EXCLUDED> const txt = TextEncoding_t<TypeEncoding::EXCLUDED>();
inline static TextEncoding_t<TypeEncoding::INCLUDED> const lit = TextEncoding_t<TypeEncoding::INCLUDED>();

} // end namespace lvd
