// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include <cstddef>
#include <iomanip>
#include "lvd/endian.hpp"
#include "lvd/literal.hpp"
#include "lvd/remove_cv_recursive.hpp"
#include "lvd/Type_t.hpp"
#include "lvd/TypeString_t.hpp"
#include <ostream>
#include <type_traits>

namespace lvd {

// Enum for specifying if type information should be included in the encoding or not.
enum class TypeEncoding : uint8_t {
    EXCLUDED = 0,   // Types are known in context (i.e. static types)
    INCLUDED,       // Types are included in the encoding (i.e. dynamic types)

    __LOWEST__ = EXCLUDED,
    __HIGHEST__ = INCLUDED
};

inline std::string const &as_string (TypeEncoding x) {
    auto constexpr COUNT = size_t(TypeEncoding::__HIGHEST__) - size_t(TypeEncoding::__LOWEST__) + 1;
    static std::array<std::string,COUNT> const TABLE{
        "EXCLUDED",
        "INCLUDED",
    };
    return TABLE.at(size_t(x));
}

inline std::ostream &operator << (std::ostream &out, TypeEncoding x) {
    return out << as_string(x);
}

// Template for storing a value to output along with a representation spec.
template <typename T_, typename Repr_>
class Out_t {
public:

    using ReprSpec = Repr_;

    Out_t () = delete;
    Out_t (Out_t const &) = delete;
    Out_t (Out_t &&) = delete;
    explicit Out_t (T_ const &src_val, ReprSpec const &repr) : m_src_val(src_val), m_repr(repr) { }

    T_ const &src_val () const { return m_src_val; }
    ReprSpec const &repr () const { return m_repr; }
    std::ostream &write (std::ostream &out) const;

private:

    T_ const &m_src_val;
    ReprSpec const &m_repr;
};

template <typename T_, typename Repr_>
std::ostream &operator<< (std::ostream &out, Out_t<T_,Repr_> const &o);

template <typename T_, typename Repr_>
class In_t {
public:

    using ReprSpec = Repr_;

    In_t () = delete;
    In_t (In_t const &) = delete;
    In_t (In_t &&) = delete;
    explicit In_t (T_ &dest_val, ReprSpec const &repr)
        :   m_dest_val(dest_val)
        ,   m_repr(repr)
    { }

    T_ &dest_val () const { return m_dest_val; }
    ReprSpec const &repr () const { return m_repr; }
    std::istream &read (std::istream &in) const;

private:

    T_ &m_dest_val;
    ReprSpec const &m_repr;
};

template <typename T_, typename Repr_>
std::istream &operator>> (std::istream &in, In_t<T_,Repr_> const &i);

// // Helper class for the nice repr syntax.
// template <typename Repr_>
// class ReprOut_t {
// public:
//
//     ReprOut_t (Repr_ const &repr) : m_repr(repr) { }
//
//     template <typename T_>
//     Out_t<T_,Repr_> operator() (T_ const &src_val) const {
//         return Out_t(src_val, m_repr);
//     }
//
// private:
//
//     Repr_ const &m_repr;
// };
//
// // Helper class for the nice repr syntax.
// template <typename Repr_>
// class ReprIn_t {
// public:
//
//     ReprIn_t (Repr_ const &repr) : m_repr(repr) { }
//
//     template <typename T_>
//     In_t<T_,Repr_> operator() (T_ &dest_val) const {
//         return In_t(dest_val, m_repr);
//     }
//
// private:
//
//     Repr_ const &m_repr;
// };

// Binary representation with endianness state variable.
// TODO: Make one where Endianness is a template param.
template <TypeEncoding TYPE_ENCODING_>
class BinRepr_t {
public:

    static TypeEncoding constexpr TYPE_ENCODING = TYPE_ENCODING_;

    BinRepr_t (Endianness endianness) : m_endianness(endianness) { }

//     // This creates a factory for Out_t objects that have a reference to this Repr.
//     ReprOut_t<BinRepr_t> out () const { return ReprOut_t<BinRepr_t>(*this); }
//     // This creates a factory for In_t objects that have a reference to this Repr.
//     ReprIn_t<BinRepr_t> in () const { return ReprIn_t<BinRepr_t>(*this); }

    template <typename T_>
    Out_t<T_,BinRepr_t> out (T_ const &src_val) const {
        return Out_t(src_val, *this);
    }
    template <typename T_>
    In_t<T_,BinRepr_t> in (T_ &dest_val) const {
        return In_t(dest_val, *this);
    }

    // For reading from a stream and producing a value, instead of populating it in-place.
    template <typename T_>
    T_ read (std::istream &in) const;

    Endianness endianness () const { return m_endianness; }

    inline static BinRepr_t const SINGLETON_BIG_ENDIAN = BinRepr_t(Endianness::BIG);
    inline static BinRepr_t const SINGLETON_LITTLE_ENDIAN = BinRepr_t(Endianness::LIL);

private:

    Endianness m_endianness;
};

// Human-readable representation.
template <TypeEncoding TYPE_ENCODING_>
class HrRepr_t {
public:

    static TypeEncoding constexpr TYPE_ENCODING = TYPE_ENCODING_;

//     // This creates a factory for Out_t objects that have a reference to this Repr.
//     ReprOut_t<HrRepr_t> out () const { return ReprOut_t<HrRepr_t>(*this); }
//     // This creates a factory for In_t objects that have a reference to this Repr.
//     ReprIn_t<HrRepr_t> in () const { return ReprIn_t<HrRepr_t>(*this); }

    template <typename T_>
    Out_t<T_,HrRepr_t> out (T_ const &src_val) const {
        return Out_t(src_val, *this);
    }
    template <typename T_>
    In_t<T_,HrRepr_t> in (T_ &dest_val) const {
        return In_t(dest_val, *this);
    }

    // For reading from a stream and producing a value, instead of populating it in-place.
    template <typename T_>
    T_ read (std::istream &in) const;

    inline static HrRepr_t const SINGLETON = HrRepr_t();
};

// Human-readable representation, types not explicitly encoded.
using HrRepr = HrRepr_t<TypeEncoding::EXCLUDED>;
// Human-readable representation, types explicitly encoded -- "literal"
using LitRepr = HrRepr_t<TypeEncoding::INCLUDED>;
// Binary representation, types not explicitly encoded -- "static binary"
using BinRepr = BinRepr_t<TypeEncoding::EXCLUDED>;
// Binary representation, types explicitly encoded -- "typed binary"
using TBinRepr = BinRepr_t<TypeEncoding::INCLUDED>;


template <typename T_>
Out_t<T_,HrRepr> hr (T_ const &value) {
    return Out_t(value, HrRepr::SINGLETON);
}

template <typename T_>
Out_t<T_,LitRepr> lit (T_ const &value) {
    return Out_t(value, LitRepr::SINGLETON);
}

template <typename T_>
Out_t<T_,BinRepr> bin (T_ const &value) {
    // Always choose little endian for serialization (for now).
    return Out_t(value, BinRepr::SINGLETON_LITTLE_ENDIAN);
}

template <typename T_>
Out_t<T_,TBinRepr> tbin (T_ const &value) {
    // Always choose little endian for serialization (for now).
    return Out_t(value, TBinRepr::SINGLETON_LITTLE_ENDIAN);
}

//
// Human-readable
//

inline std::ostream &write_out (std::ostream &out, Out_t<bool,HrRepr> const &o) {
    return out << (o.src_val() ? "true" : "false");
}
inline std::ostream &write_out (std::ostream &out, Out_t<std::byte,HrRepr> const &o) {
    return out << "0x" << LVD_FMT(std::hex << std::uppercase << std::setw(2) << std::setfill('0') << uint32_t(o.src_val()));
}
inline std::ostream &write_out (std::ostream &out, Out_t<char,HrRepr> const &o) {
    return out << literal_of(o.src_val());
}
inline std::ostream &write_out (std::ostream &out, Out_t<int8_t,HrRepr> const &o) {
    return out << int32_t(o.src_val());
}
inline std::ostream &write_out (std::ostream &out, Out_t<uint8_t,HrRepr> const &o) {
    return out << uint32_t(o.src_val());
}
inline std::ostream &write_out (std::ostream &out, Out_t<int16_t,HrRepr> const &o) {
    return out << o.src_val();
}
inline std::ostream &write_out (std::ostream &out, Out_t<uint16_t,HrRepr> const &o) {
    return out << o.src_val();
}
inline std::ostream &write_out (std::ostream &out, Out_t<int32_t,HrRepr> const &o) {
    return out << o.src_val();
}
inline std::ostream &write_out (std::ostream &out, Out_t<uint32_t,HrRepr> const &o) {
    return out << o.src_val();
}
inline std::ostream &write_out (std::ostream &out, Out_t<int64_t,HrRepr> const &o) {
    return out << o.src_val();
}
inline std::ostream &write_out (std::ostream &out, Out_t<uint64_t,HrRepr> const &o) {
    return out << o.src_val();
}
template <typename Float_, typename = std::enable_if_t<std::is_floating_point_v<Float_>>>
std::ostream &write_out (std::ostream &out, Out_t<Float_,HrRepr> const &o) {
    return out << LVD_FMT(std::scientific << std::setprecision(std::numeric_limits<Float_>::max_digits10) << o.src_val());
}
inline std::ostream &write_out (std::ostream &out, Out_t<std::string,HrRepr> const &o) {
    return out << literal_of(o.src_val());
}

// Any type that isn't an aggregation of other types.
template <typename T_, typename = std::enable_if_t<std::is_arithmetic_v<T_> || std::is_same_v<T_,std::byte> || std::is_same_v<T_,std::string>>>
std::ostream &write_out (std::ostream &out, Out_t<T_,LitRepr> const &o) {
    out << type_v<T_> << '(';
    auto spec = HrRepr(); // TODO: Use params from o.repr()
    write_out(out, Out_t(o.src_val(), spec));
    return out << ')';
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::pair<Types_...>,HrRepr_t<TYPE_ENCODING_>> const &o) {
    if constexpr (TYPE_ENCODING_ == TypeEncoding::INCLUDED) {
        out << type_v<std::pair<Types_...>>;
    }
    out << '(';
    // No need for type info here, it's already present above.
    write_out(out, Out_t(o.src_val().first, HrRepr::SINGLETON));
    out << ", ";
    write_out(out, Out_t(o.src_val().second, HrRepr::SINGLETON));
    return out << ')';
}

// Helper function
template <typename Container_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out_sequence_container (std::ostream &out, Out_t<Container_,HrRepr_t<TYPE_ENCODING_>> const &o) {
    if constexpr (TYPE_ENCODING_ == TypeEncoding::INCLUDED) {
        out << type_v<Container_>;
    }
    out << '(';
    bool has_printed_element = false;
    for (auto const &element : o.src_val()) {
        if (has_printed_element)
            out << ", ";
        else
            has_printed_element = true;
        // No need for type info here, it's already present above.
        write_out(out, Out_t(element, HrRepr::SINGLETON));
    }
    return out << ')';
}

template <typename T_, size_t N_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::array<T_,N_>,HrRepr_t<TYPE_ENCODING_>> const &o) {
    return write_out_sequence_container(out, o);
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::vector<Types_...>,HrRepr_t<TYPE_ENCODING_>> const &o) {
    return write_out_sequence_container(out, o);
}

// Helper function
template <typename Container_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out_associative_container (std::ostream &out, Out_t<Container_,HrRepr_t<TYPE_ENCODING_>> const &o) {
    if constexpr (TYPE_ENCODING_ == TypeEncoding::INCLUDED) {
        out << type_v<Container_>;
    }
    out << '(';
    bool has_printed_element = false;
    for (auto const &element : o.src_val()) {
        if (has_printed_element)
            out << ", ";
        else
            has_printed_element = true;
        // No need for type info here, it's already present above.
        write_out(out, Out_t(element, HrRepr::SINGLETON));
    }
    return out << ')';
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::map<Types_...>,HrRepr_t<TYPE_ENCODING_>> const &o) {
    return write_out_associative_container(out, o);
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::set<Types_...>,HrRepr_t<TYPE_ENCODING_>> const &o) {
    return write_out_associative_container(out, o);
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::unordered_map<Types_...>,HrRepr_t<TYPE_ENCODING_>> const &o) {
    return write_out_associative_container(out, o);
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::unordered_set<Types_...>,HrRepr_t<TYPE_ENCODING_>> const &o) {
    return write_out_associative_container(out, o);
}


//
// Binary serialization -- type information is not included; it's expected to be known in context.
//

template <typename T_, typename = std::enable_if_t<is_endiannated_type_v<T_>>>
std::ostream &write_out (std::ostream &out, Out_t<T_,BinRepr> const &o) {
    if constexpr (std::is_same_v<T_,bool>) {
        return out.put(o.src_val() ? uint8_t(1) : uint8_t(0));
//     } else if constexpr (std::is_same_v<T_,std::byte>) {
//         return out.put(uint8_t(o.src_val()));
    } else if constexpr (sizeof(T_) == 1) {
        return out.put(char(o.src_val()));
    } else {
        static_assert(sizeof(T_) > 1);
        T_ temp = endian_changed(machine_endianness(), o.repr().endianness(), o.src_val());
        return out.write(reinterpret_cast<char const *>(&temp), sizeof(temp));
    }
}

// inline std::ostream &write_out (std::ostream &out, Out_t<bool,BinRepr> const &o) {
//     return out.put(o.src_val() ? uint8_t(1) : uint8_t(0));
// }
// inline std::ostream &write_out (std::ostream &out, Out_t<std::byte,BinRepr> const &o) {
//     return out.put(uint8_t(o.src_val()));
// }
// inline std::ostream &write_out (std::ostream &out, Out_t<char,BinRepr> const &o) {
//     return out.put(o.src_val());
// }
// inline std::ostream &write_out (std::ostream &out, Out_t<int8_t,BinRepr> const &o) {
//     return out.put(o.src_val());
// }
// inline std::ostream &write_out (std::ostream &out, Out_t<uint8_t,BinRepr> const &o) {
//     return out.put(o.src_val());
// }
// inline std::ostream &write_out (std::ostream &out, Out_t<int16_t,BinRepr> const &o) {
//     // TODO: Handle endianness
//     return out.write(reinterpret_cast<char const *>(&o.src_val()), sizeof(o.src_val()));
// }
// inline std::ostream &write_out (std::ostream &out, Out_t<uint16_t,BinRepr> const &o) {
//     // TODO: Handle endianness
//     return out.write(reinterpret_cast<char const *>(&o.src_val()), sizeof(o.src_val()));
// }
// inline std::ostream &write_out (std::ostream &out, Out_t<int32_t,BinRepr> const &o) {
//     // TODO: Handle endianness
//     return out.write(reinterpret_cast<char const *>(&o.src_val()), sizeof(o.src_val()));
// }
// inline std::ostream &write_out (std::ostream &out, Out_t<uint32_t,BinRepr> const &o) {
//     // TODO: Handle endianness
//     return out.write(reinterpret_cast<char const *>(&o.src_val()), sizeof(o.src_val()));
// }
// inline std::ostream &write_out (std::ostream &out, Out_t<int64_t,BinRepr> const &o) {
//     // TODO: Handle endianness
//     return out.write(reinterpret_cast<char const *>(&o.src_val()), sizeof(o.src_val()));
// }
// inline std::ostream &write_out (std::ostream &out, Out_t<uint64_t,BinRepr> const &o) {
//     // TODO: Handle endianness
//     return out.write(reinterpret_cast<char const *>(&o.src_val()), sizeof(o.src_val()));
// }
// template <typename Float_, typename = std::enable_if_t<std::is_floating_point_v<Float_>>>
// std::ostream &write_out (std::ostream &out, Out_t<Float_,BinRepr> const &o) {
//     // TODO: Handle endianness
//     return out.write(reinterpret_cast<char const *>(&o.src_val()), sizeof(o.src_val()));
// }
inline std::ostream &write_out (std::ostream &out, Out_t<std::string,BinRepr> const &o) {
    out << bin(o.src_val().size()); // Could limit to uint32_t
    return out.write(&o.src_val()[0], o.src_val().size());
}
template <typename... Types_>
std::ostream &write_out (std::ostream &out, Out_t<std::pair<Types_...>,BinRepr> const &o) {
    write_out(out, Out_t(o.src_val().first, o.repr()));
    write_out(out, Out_t(o.src_val().second, o.repr()));
    return out;
}

template <typename T_, size_t N_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::array<T_,N_>,BinRepr_t<TYPE_ENCODING_>> const &o) {
    if constexpr (TYPE_ENCODING_ == TypeEncoding::INCLUDED) {
        out << type_v<std::array<T_,N_>>;
    }
    for (auto const &element : o.src_val())
        write_out(out, Out_t(element, BinRepr_t<TypeEncoding::EXCLUDED>(o.repr().endianness())));
    return out;
}

// Helper function
template <typename Container_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out_container (std::ostream &out, Out_t<Container_,BinRepr_t<TYPE_ENCODING_>> const &o) {
    static_assert(TYPE_ENCODING_ != TypeEncoding::INCLUDED, "not supported yet");
//     if constexpr (TYPE_ENCODING_ == TypeEncoding::INCLUDED) {
//         out << type_v<Container_>;
//     }
    write_out(out, Out_t(o.src_val().size(), BinRepr_t<TypeEncoding::EXCLUDED>(o.repr().endianness())));
    for (auto const &element : o.src_val())
        write_out(out, Out_t(element, BinRepr_t<TypeEncoding::EXCLUDED>(o.repr().endianness())));
    return out;
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::vector<Types_...>,BinRepr_t<TYPE_ENCODING_>> const &o) {
    return write_out_container(out, o);
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::map<Types_...>,BinRepr_t<TYPE_ENCODING_>> const &o) {
    return write_out_container(out, o);
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::set<Types_...>,BinRepr_t<TYPE_ENCODING_>> const &o) {
    return write_out_container(out, o);
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::unordered_map<Types_...>,BinRepr_t<TYPE_ENCODING_>> const &o) {
    return write_out_container(out, o);
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::ostream &write_out (std::ostream &out, Out_t<std::unordered_set<Types_...>,BinRepr_t<TYPE_ENCODING_>> const &o) {
    return write_out_container(out, o);
}

//
// stuffs
//

template <typename T_, typename Repr_>
std::ostream &Out_t<T_,Repr_>::write (std::ostream &out) const {
    return write_out(out, *this);
}

template <typename T_, typename Repr_>
std::ostream &operator<< (std::ostream &out, Out_t<T_,Repr_> const &o) {
    return write_out(out, o);
}

// NOTE: These are commented out (and the corresponding read_in functions not implemented)
// because parsing human-readable content is nontrivial, e.g. with having to deal with space
// between content, as well as potentially nontrivial parsing.

// template <typename T_>
// In_t<T_,HrRepr> into_hr (T_ &dest_val) {
//     return In_t(dest_val, HrRepr::SINGLETON);
// }
//
// template <typename T_>
// In_t<T_,LitRepr> into_lit (T_ const &dest_val) {
//     return In_t(dest_val, LitRepr::SINGLETON);
// }

template <typename T_>
In_t<T_,BinRepr> into_bin (T_ &dest_val) {
    // Assume serialized stream is always little endian for now.
    return In_t(dest_val, BinRepr::SINGLETON_LITTLE_ENDIAN);
}

template <typename T_>
In_t<T_,TBinRepr> into_tbin (T_ const &dest_val) {
    // Assume serialized stream is always little endian for now.
    return In_t(dest_val, TBinRepr::SINGLETON_LITTLE_ENDIAN);
}


//
// Binary serialization -- type information is not included; it's expected to be known in context.
//

// Default implementation requires that T_ is default-constructible.
// Provide your own overload for your own types.
template <typename T_, typename = std::enable_if_t<std::is_default_constructible_v<T_>>>
T_ read_bin (std::istream &in, BinRepr const &repr);

// Default implementation requires that T_ is default-constructible.
// Provide your own overload for your own types.
template <typename T_, typename = std::enable_if_t<std::is_default_constructible_v<T_>>>
T_ read_tbin (std::istream &in, TBinRepr const &repr);

// // Template-specialize this in order to provide an implementation for populating a value
// // using data from the stream.  Default implementation provides some code.
// // Should provide `std::istream &operator() (std::istream &in, In_t<T_,Repr_> const &i) const`
// template <typename T_, typename Repr_>
// struct ReadInPlace_t {
//     template <typename = std::enable_if_t<is_endiannated_type_v<T_>>>
//     std::istream &read_in (std::istream &in, In_t<T_,BinRepr> const &i) const {
//         if constexpr (std::is_same_v<T_,bool>) {
//             i.dest_val() = in.get() != 0;
//         } else if constexpr (sizeof(T_) == 1) {
//             i.dest_val() = T_(in.get());
//         } else {
//             static_assert(sizeof(T_) > 1);
//             in.read(reinterpret_cast<char *>(&i.dest_val()), sizeof(i.dest_val()));
//             endian_change(i.repr().endianness(), machine_endianness(), i.dest_val());
//         }
//         return in;
//     }
//
// }

template <typename T_, typename = std::enable_if_t<is_endiannated_type_v<T_>>>
std::istream &read_in (std::istream &in, In_t<T_,BinRepr> const &i) {
    if constexpr (std::is_same_v<T_,bool>) {
        i.dest_val() = in.get() != 0;
    } else if constexpr (sizeof(T_) == 1) {
        i.dest_val() = T_(in.get());
    } else {
        static_assert(sizeof(T_) > 1);
        in.read(reinterpret_cast<char *>(&i.dest_val()), sizeof(i.dest_val()));
        endian_change(i.repr().endianness(), machine_endianness(), i.dest_val());
    }
    return in;
}

// inline std::istream &read_in (std::istream &in, In_t<bool,BinRepr> const &i) {
//     i.dest_val() = in.get() != 0;
//     return in;
// }
// inline std::istream &read_in (std::istream &in, In_t<std::byte,BinRepr> const &i) {
//     i.dest_val() = std::byte(in.get());
//     return in;
// }
// inline std::istream &read_in (std::istream &in, In_t<char,BinRepr> const &i) {
//     i.dest_val() = char(in.get());
//     return in;
// }
// inline std::istream &read_in (std::istream &in, In_t<int8_t,BinRepr> const &i) {
//     i.dest_val() = int8_t(in.get());
//     return in;
// }
// inline std::istream &read_in (std::istream &in, In_t<uint8_t,BinRepr> const &i) {
//     i.dest_val() = uint8_t(in.get());
//     return in;
// }
// inline std::istream &read_in (std::istream &in, In_t<int16_t,BinRepr> const &i) {
//     // TODO: Handle endianness
//     return in.read(reinterpret_cast<char *>(&i.dest_val()), sizeof(i.dest_val()));
// }
// inline std::istream &read_in (std::istream &in, In_t<uint16_t,BinRepr> const &i) {
//     // TODO: Handle endianness
//     return in.read(reinterpret_cast<char *>(&i.dest_val()), sizeof(i.dest_val()));
// }
// inline std::istream &read_in (std::istream &in, In_t<int32_t,BinRepr> const &i) {
//     // TODO: Handle endianness
//     return in.read(reinterpret_cast<char *>(&i.dest_val()), sizeof(i.dest_val()));
// }
// inline std::istream &read_in (std::istream &in, In_t<uint32_t,BinRepr> const &i) {
//     // TODO: Handle endianness
//     return in.read(reinterpret_cast<char *>(&i.dest_val()), sizeof(i.dest_val()));
// }
// inline std::istream &read_in (std::istream &in, In_t<int64_t,BinRepr> const &i) {
//     // TODO: Handle endianness
//     return in.read(reinterpret_cast<char *>(&i.dest_val()), sizeof(i.dest_val()));
// }
// inline std::istream &read_in (std::istream &in, In_t<uint64_t,BinRepr> const &i) {
//     // TODO: Handle endianness
//     return in.read(reinterpret_cast<char *>(&i.dest_val()), sizeof(i.dest_val()));
// }
// template <typename Float_, typename = std::enable_if_t<std::is_floating_point_v<Float_> && !std::is_integral_v<Float_>>>
// std::istream &read_in (std::istream &in, In_t<Float_,BinRepr> const &i) {
//     // TODO: Handle endianness
//     return in.read(reinterpret_cast<char *>(&i.dest_val()), sizeof(i.dest_val()));
// }
inline std::istream &read_in (std::istream &in, In_t<std::string,BinRepr> const &i) {
    auto size = read_bin<size_t>(in, i.repr());
    // TODO: Probably put a limit on the size to avoid bugs in which a huge value is read
    // causing memory to fill up.
    i.dest_val().resize(size);
    return in.read(&i.dest_val()[0], size);
}
template <typename... Types_>
std::istream &read_in (std::istream &in, In_t<std::pair<Types_...>,BinRepr> const &i) {
    read_in(in, In_t(i.dest_val().first, i.repr()));
    read_in(in, In_t(i.dest_val().second, i.repr()));
    return in;
}

template <typename T_, size_t N_, TypeEncoding TYPE_ENCODING_>
std::istream &read_in (std::istream &in, In_t<std::array<T_,N_>,BinRepr_t<TYPE_ENCODING_>> const &i) {
    static_assert(TYPE_ENCODING_ != TypeEncoding::INCLUDED, "not supported yet");
//     if constexpr (TYPE_ENCODING_ == TypeEncoding::INCLUDED) {
//         in << type_v<Container_>;
//     }
    for (auto &element : i.dest_val())
        read_in(in, In_t(element, BinRepr_t<TypeEncoding::EXCLUDED>(i.repr().endianness())));
    return in;
}

// Helper function
template <typename Container_, TypeEncoding TYPE_ENCODING_>
std::istream &read_in_sequence_container (std::istream &in, In_t<Container_,BinRepr_t<TYPE_ENCODING_>> const &i) {
    using ValueType = remove_cv_recursive_t<typename Container_::value_type>;

    static_assert(TYPE_ENCODING_ != TypeEncoding::INCLUDED, "not supported yet");
//     if constexpr (TYPE_ENCODING_ == TypeEncoding::INCLUDED) {
//         in << type_v<Container_>;
//     }
    i.dest_val().clear();
    auto size = read_bin<size_t>(in, BinRepr_t<TypeEncoding::EXCLUDED>(i.repr().endianness()));
    i.dest_val().reserve(size);
    for (size_t n = 0; n < size; ++n)
        i.dest_val().emplace_back(read_bin<ValueType>(in, BinRepr_t<TypeEncoding::EXCLUDED>(i.repr().endianness())));
    return in;
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::istream &read_in (std::istream &in, In_t<std::vector<Types_...>,BinRepr_t<TYPE_ENCODING_>> const &i) {
    return read_in_sequence_container(in, i);
}

// Helper function
template <typename Container_, TypeEncoding TYPE_ENCODING_>
std::istream &read_in_associative_container (std::istream &in, In_t<Container_,BinRepr_t<TYPE_ENCODING_>> const &i) {
    using ValueType = remove_cv_recursive_t<typename Container_::value_type>;

    static_assert(TYPE_ENCODING_ != TypeEncoding::INCLUDED, "not supported yet");
//     if constexpr (TYPE_ENCODING_ == TypeEncoding::INCLUDED) {
//         in << type_v<Container_>;
//     }
    i.dest_val().clear();
    auto size = read_bin<size_t>(in, BinRepr_t<TypeEncoding::EXCLUDED>(i.repr().endianness()));
    for (size_t n = 0; n < size; ++n)
        i.dest_val().emplace(read_bin<ValueType>(in, BinRepr_t<TypeEncoding::EXCLUDED>(i.repr().endianness())));
    return in;
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::istream &read_in (std::istream &in, In_t<std::map<Types_...>,BinRepr_t<TYPE_ENCODING_>> const &i) {
    return read_in_associative_container(in, i);
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::istream &read_in (std::istream &in, In_t<std::set<Types_...>,BinRepr_t<TYPE_ENCODING_>> const &i) {
    return read_in_associative_container(in, i);
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::istream &read_in (std::istream &in, In_t<std::unordered_map<Types_...>,BinRepr_t<TYPE_ENCODING_>> const &i) {
    return read_in_associative_container(in, i);
}

template <typename... Types_, TypeEncoding TYPE_ENCODING_>
std::istream &read_in (std::istream &in, In_t<std::unordered_set<Types_...>,BinRepr_t<TYPE_ENCODING_>> const &i) {
    return read_in_associative_container(in, i);
}

// Template-specialize this in order to provide an implementation for producing a value
// from the stream.  This specialization is only needed for types that aren't default
// constructible.  The default implementation is for types that are default constructible.
template <typename T_, typename Repr_>
struct Read_t {
    template <typename = std::enable_if_t<std::is_default_constructible_v<T_>>>
    T_ operator() (std::istream &in, Repr_ const &repr) const {
        T_ retval;
        read_in(in, In_t(retval, repr));
        return retval;
    }
};

// Provide overloads of these for types that can't be default constructed and then populated.
// TODO: Get rid of these
template <typename T_,  typename = std::enable_if_t<std::is_default_constructible_v<T_>>>
T_ read_bin (std::istream &in, BinRepr const &repr) {
    T_ retval;
    read_in(in, In_t(retval, repr));
    return retval;
}

// Provide overloads of these for types that can't be default constructed and then populated.
// TODO: Get rid of these
template <typename T_, typename = std::enable_if_t<std::is_default_constructible_v<T_>>>
T_ read_tbin (std::istream &in, TBinRepr const &repr) {
    T_ retval;
    read_in(in, In_t(retval, repr));
    return retval;
}

//
// stuffs
//

template <typename T_, typename Repr_>
std::istream &In_t<T_,Repr_>::read (std::istream &in) const {
    return read_in(in, *this);
}

template <typename T_, typename Repr_>
std::istream &operator>> (std::istream &in, In_t<T_,Repr_> const &i) {
    return read_in(in, i);
}

template <TypeEncoding TYPE_ENCODING_>
template <typename T_>
T_ BinRepr_t<TYPE_ENCODING_>::read (std::istream &in) const {
    return Read_t<T_,BinRepr_t<TYPE_ENCODING_>>()(in, *this);
}

template <TypeEncoding TYPE_ENCODING_>
template <typename T_>
T_ HrRepr_t<TYPE_ENCODING_>::read (std::istream &in) const {
    return Read_t<T_,HrRepr_t<TYPE_ENCODING_>>()(in, *this);
}

} // end namespace lvd
