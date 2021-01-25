// 2021.01.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

namespace lvd {

// Can be used as a value to represent the type T_ itself, making it possible to
// have types as first class values.  This class has no members, so it's abstractly
// a singleton.  There is an inline static singleton instance declared, so its
// address should uniquely identify the type (probably, maybe there are situations
// in which the linker wouldn't collapse them together).
//
// The global function type_v<T_>() can be used to conveniently retrieve the Type_t<T_> singleton.
//
// TODO: Create a baseclass Type with CRTP, so that Type* can be used as the key in containers.
template <typename T_>
class Type_t {
public:

    // Use of singletons allows type_v<T_> (defined below) to be uniquely identified by address,
    // and therefore can be used as a runtime value for distinction or mapping.  This singleton
    // doesn't need to be const, because there's no state that can be changed anyway..
    inline static Type_t SINGLETON = Type_t();

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
};

// This allows very terse use of Type_t<T_>.
template <typename T_>
inline static Type_t<T_> &type_v = Type_t<T_>::SINGLETON;

// Convenience functions that does type deduction.
template <typename T_>
Type_t<T_> &type_of (T_ const &) {
    return type_v<T_>;
}
// // This one returns a non-singleton instance.
// template <typename T_>
// Type_t<T_> make_type_of (T_ const &) {
//     return Type_t<T_>();
// }

// Template metafunction for determining if a given type is Type_t<T_> for some T_
template <typename T_> struct is_Type_t : public std::false_type { };
template <typename T_> inline bool constexpr is_Type_t_v = is_Type_t<T_>::value;

template <typename T_> struct is_Type_t<Type_t<T_>> : public std::true_type { };

} // end namespace lvd
