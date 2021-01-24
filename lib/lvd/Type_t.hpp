// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

namespace lvd {

// Can be used as a value to represent the type T_ itself, making it possible to
// have types as first class values.  This class has no members, so it's necessarily
// a singleton, and in fact there is an inline static singleton declared, so its
// address should uniquely identify the type (probably, maybe there are situations
// in which the linker wouldn't collapse them together).
//
// The global function type_v<T_>() can be used to conveniently retrieve the Type_t<T_> singleton.
template <typename T_>
class Type_t {
public:

    // Use of singletons allows type_v<T_> (defined below) to be uniquely identified by address,
    // and therefore can be used in
    inline static Type_t const SINGLETON = Type_t();

    // Prevent copy and move construction.  This is part of the uniqueness described by SINGLETON.;
    Type_t (Type_t const &) = delete;
    Type_t (Type_t &&) = delete;

    // Prevent copy and move assignment.  This is part of the uniqueness described by SINGLETON.;
    Type_t &operator = (Type_t const &) = delete;
    Type_t &operator = (Type_t &&) = delete;

    // This will call the constructor of T_ with the given args, so that an instance of Type_t<T_>
    // can behave like `T_` itself.
    template <typename... Args_>
    auto operator() (Args_&&... args) const {
        return T_(std::forward<Args_>(args)...);
    }

    template <typename Other_>
    constexpr bool operator == (Type_t<Other_> const &) const { return std::is_same_v<T_,Other_>; }
    template <typename Other_>
    constexpr bool operator != (Type_t<Other_> const &) const { return !std::is_same_v<T_,Other_>; }

    // This defines >= for subtype poset.
    template <typename Other_>
    constexpr bool is_supertype_of (Type_t<Other_> const &) const { return std::is_base_of_v<T_,Other_> || std::is_same_v<T_,Other_>; }
    // This defines > for subtype poset.
    template <typename Other_>
    constexpr bool is_strict_supertype_of (Type_t<Other_> const &) const { return std::is_base_of_v<T_,Other_> && !std::is_same_v<T_,Other_>; }
    // This defines <= for subtype poset.
    template <typename Other_>
    constexpr bool is_subtype_of (Type_t<Other_> const &other) const { return other.is_supertype_of(*this); }
    // This defines < for subtype poset.
    template <typename Other_>
    constexpr bool is_strict_subtype_of (Type_t<Other_> const &other) const { return other.is_strict_supertype_of(*this); }
    // This defines `comparable` condition for subtype poset (one type is a subtype of the other)
    template <typename Other_>
    constexpr bool is_related_to (Type_t<Other_> const &other) const { return this->is_subtype_of(other) || this->is_supertype_of(other); }
    // This defines `incomparable` condition for subtype poset (neither is a subtype of the other)
    template <typename Other_>
    constexpr bool is_unrelated_to (Type_t<Other_> const &other) const { return !this->is_subtype_of(other) && !this->is_supertype_of(other); }

    // TODO: Implement functions to produce types like pair, tuple, etc.

private:

    // Only SINGLETON should be able to construct one of these.
    Type_t () { }
};

// This allows very terse use of Type_t<T_>.
template <typename T_>
inline static Type_t<T_> const &type_v = Type_t<T_>::SINGLETON;

} // end namespace lvd
