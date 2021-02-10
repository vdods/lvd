// 2021.01.28 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

// #include <cstdlib>
#include "lvd/fmt.hpp" // A bit overkill
#include <ostream>
#include <string>
// #include <type_traits>

/*
Notes on semantic subtypes.
-   Programs use basic data types, e.g. uint16_t, float, bool, std::string, etc to represent quantities.
-   Denote by "concrete type" a data type used by a program to represent a quantity.  By itself, a
    concrete type carries no meaning beyond its representational value.
-   Each variable in a program carries implicit semantic meaning which is not explicitly or formally
    defined or specified in the program, such as:
    -   `std::string legal_name` is meant to signify a person's legal name.
    -   `std::string email_address` is meant to signify an email address.
    -   `std::string normalized_email_address` is meant to signify a normalized (cleaned) email address.
    -   `double mass` is meant to signify a mass, measured in units of grams.  In particular, the
        mass is nonnegative.
    -   `double bodyweight` is meant to signify a mass (of a person's body), measured in units of pounds.
        In particular, it must be positive.
    -   `double displacement` is meant to signify the displacement of a particle along an axis,
        measured in units of meters, and can be positive, negative, or zero, but must be finite.
    -   `double likelihood` is meant to signify the likelihood of some event.  In particular, the
        quantity must be in the range [0,1], inclusive.
    Even though the data types for several of these quantities are the same, it's meaningless to compare
    them, add them, etc, and there's no generic way to operate on or compare quantities having distinct
    meaning.
-   Denote by "semantic type" the additional semantic meaning of a quantity, independent of its
    particular representation in the program.  For example, a program might represent a quantity
    of kilograms using `float`, `double`, `uint32_t`, or many others, but the meaning of the
    quantity is still the same.  This meaning is the "semantic type" of the quantity.
-   In some way, "semantic type" and "concrete type" are independent of each other.  Strictly speaking,
    the semantic type puts some constraints on what an acceptable concrete type is to faithfully
    represent values carrying its semantic meaning, but usually there will be a collection of
    acceptable concrete types for any given semantic type.
-   Denote by "semantic subtype" a pair (S,C) where S is a semantic type and C is a concrete type.
    C has to be a representation that's compatible with S (e.g. `double` would not suffice to
    represent a URL. but `std::string` or `std::array<char,2048>` could).
-   A value of semantic subtype (S,C) is a value of type C which carries the meaning (and potentially
    value constraints) of S.
-   There is a commutative diagram that should be satisfied by operations on semantic types.
    Using the (S,C) notation for a semantic value, it should hold that for any relevant `op`:

        (LhsS, LhsC) op (RhsS, RhsC) == (LhsS op RhsS, LhsC op RhsC)

    whenever `LhsS op RhsS` and `LhsC op RhsC` are well-defined.  In fact, this should be how operators
    should be defined on semantic values.  This should hold for functions in general:

        fancy_func((S1,C1), ..., (Sn,Cn)) := (fancy_func(S1, ..., Sn), fancy_func(C1, ..., Cn))

    if fancy_func has an overload which takes semantic class instances, defining the semantic
    class for the result.  To summarize, given a concrete function fancy_func, if an overload (or
    overloads) of fancy_func is made that takes appropriate semantic class instances and returns
    a semantic class instance, then that defines an overload of fancy_func on the corresponding
    semantic types.

Implementation notes
-   The template class `SV_t<S,C>` is the C++ implementation of a value of semantic subtype (S,C).
    SV stands for "semantic value".  `SV_t<S,C>` contains a single member of type `C`, so in terms
    of its in-memory representation, it's equivalent to `C`.
-   Here, the symbol `S` is a C++ class which represents the semantic type, and will be called the
    "semantic [C++] class" to distinguish it from the abstract concept of "semantic type" (the
    semantic type is abstract, whereas the semantic class is a C++ class which represents that
    semantic type).  A semantic class should have no nonstatic members.  It will not be instantiated
    anywhere.  In particular, at minimum, a semantic class must have a method

        template <typename C>
        static bool is_valid (C const &concrete_value) {
            return <condition defining a valid concrete_value>
        }

    where `C` is the concrete type.  This defines what concrete values are valid under the definition of
    the semantic type.  This is_valid method is used at certain "type boundaries" of `SV_t<S,C>` (e.g.
    constructors, assignments, etc) to guarantee that `SV_t<S,C>` never holds a value incompatible with `S`.
-   By construction, any semantic type behavior defined in the semantic class, e.g. the `is_valid`
    method, is effectively duck-typed, since its argument is given as a template-typed parameter.
-   Also at minimum, a semantic class should inherit `Base_s` which defines a
    reasonable default behavior for the various capabilities of a C++ class (constructors, assignment
    operators, operator overloads, etc), such that a broadly useful semantic subtype can be defined
    with only few lines of code.
-   The various attributes defined in `Base_s` can be overridden in a semantic class,
    thereby customizing the particular C++ behavior of the semantic value type `SV_t<S,C>`.
-   Subclassing semantic classes has the naturally expected behavior, though (for now), if `A` and `B`
    are semantic classes with `B` a subclass of `A`, it doesn't follow that `SV_t<B,C>` is a subclass
    of `SV_t<A,C>`.  This might change later.

TODO
-   Allow custom behavior for assert and throw, defined by `S`, with defaults in `Base_s`.
-   Develop some theory and code regarding an actionable subtype hierarchy of semantic types and
    the corresponding semantic subtypes.  For example, let `NonEmpty_s`, `EmailAddr_s`, and `NormedEmailAddr_s`
    denote semantic [C++] classes with the expected definitions, and assume that the class hierarchy is

        NonEmpty_s <- EmailAddr_s <- NormedEmailAddr_s

    Then ideally, it should be that the semantic value types have the same relationship:

        SV_t<NonEmpty_s,std::string> <- SV_t<EmailAddr_s,std::string> <- SV_t<NormedEmailAddr_s,std::string>

    Obviously the semantic class' definitions of `is_valid` would have to be compatible for them to
    actually form a legitimate semantic type hierarchy.

    For a semantic type S3 which is a subtype of S1 and S2 (i.e. intersection of those types), it should
    follow that

        S3::is_valid(cv) == S1::is_valid(cv) && S2::is_valid(cv)

-   Idea for more-complex, ad-hoc in-place modification that still respects the validity:
    -   Create a guard object that "allows modification" of the SV_t object arbitrarily.
    -   There should be an "obtain modification guard" method which produces the guard.
    -   When the guard is destroyed, the validity checker is run, throwing if invalid.
    -   Unfortunately there's no way to guarantee that other non-const references to the object
        don't exist (like there is in Rust).  However, this behavior can be approximated by
        causing the guard object to move the concrete value out of the SV_t, and moving it
        back in once the guard goes out of scope.
-   Idea for a different way to define semantic-subtype-specific behavior -- could define
    operator overloads on instances of the semantic classes themselves to define what type
    results from each operation.
-   Idea for unions and intersections of semantic subtypes:
    -   Define the is_valid method for union(S1,...,Sn) as S1::is_valid(cv) || ... || Sn::is_valid(cv)
        and for intersection(S1,...,Sn) as S1::is_valid(cv) && ... && Sn::is_valid(cv).
    -   Then if an operator X is defined on all combos of instances of Si,Sj, then define operator X

            inline decltype(auto) operator X (union(LhsS1,...,LhsSm), union(RhsS1,...,RhsSn)) {
                return union((LhsSi X RhsSj) for i in [1,...,m] and j in [1,...,n])
            }

        on instances of union(S1,...,Sn)

        Similarly for intersection.

        This could be done in real C++ using variadic arguments and an inductive definition, e.g.

            inline decltype(auto) operator X (union(LhsS1,...,LhsSm), RhsS) {
                return union((LhsSi X RhsS) for i in [1,...,m])
            }
            inline decltype(auto) operator X (LhsS, union(RhsS1,...,RhsSn)) {
                return union((LhsS X RhsSj) for j in [1,...,n])
            }

        and a way to eliminate redundancies in union.

        Should have one corresponding to xor as well as the `and` and `or` of intersection and union.
-   IDEA: Could add "clamp" to extend checkpolicy, e.g. when float underflow happens, but it's desired
    to have a strictly positive value, clamp the value to the smallest [subnormal] positive value.

Random notes
-   Here's a conceptually similar, but awkward proto-attempt at solving the problem that semantic subtyping solves:
    https://github.com/Keats/validator
*/

namespace lvd {
namespace sst {

// Enum used to define the behavior for various type boundary checks, e.g. constructions, assignments, etc.
enum class CheckPolicy : uint8_t {
    PROHIBIT = 0,
    ALLOW__VERIFY_OR_THROW,
    ALLOW__VERIFY_OR_ABORT,
    ALLOW__ASSERT,
    ALLOW__NO_CHECK,

    __LOWEST__ = PROHIBIT,
    __HIGHEST__ = ALLOW__NO_CHECK,
};

inline static CheckPolicy constexpr PROHIBIT = CheckPolicy::PROHIBIT;
inline static CheckPolicy constexpr ALLOW__VERIFY_OR_THROW = CheckPolicy::ALLOW__VERIFY_OR_THROW;
inline static CheckPolicy constexpr ALLOW__VERIFY_OR_ABORT = CheckPolicy::ALLOW__VERIFY_OR_ABORT;
inline static CheckPolicy constexpr ALLOW__ASSERT = CheckPolicy::ALLOW__ASSERT;
inline static CheckPolicy constexpr ALLOW__NO_CHECK = CheckPolicy::ALLOW__NO_CHECK;

// Enum used to define the behavior for the result of various operations.
enum class ResultPolicy : uint8_t {
    DONT_CAST = 0,
    CAST_TO_SV__VERIFY_OR_THROW,
    CAST_TO_SV__VERIFY_OR_ABORT,
    CAST_TO_SV__ASSERT,
    CAST_TO_SV__NO_CHECK,

    __LOWEST__ = DONT_CAST,
    __HIGHEST__ = CAST_TO_SV__NO_CHECK,
};

inline static ResultPolicy constexpr DONT_CAST = ResultPolicy::DONT_CAST;
inline static ResultPolicy constexpr CAST_TO_SV__VERIFY_OR_THROW = ResultPolicy::CAST_TO_SV__VERIFY_OR_THROW;
inline static ResultPolicy constexpr CAST_TO_SV__VERIFY_OR_ABORT = ResultPolicy::CAST_TO_SV__VERIFY_OR_ABORT;
inline static ResultPolicy constexpr CAST_TO_SV__ASSERT = ResultPolicy::CAST_TO_SV__ASSERT;
inline static ResultPolicy constexpr CAST_TO_SV__NO_CHECK = ResultPolicy::CAST_TO_SV__NO_CHECK;

inline CheckPolicy constexpr as_check_policy (ResultPolicy x) {
    switch (x) {
        case DONT_CAST: return ALLOW__NO_CHECK;
        case CAST_TO_SV__VERIFY_OR_THROW: return ALLOW__VERIFY_OR_THROW;
        case CAST_TO_SV__VERIFY_OR_ABORT: return ALLOW__VERIFY_OR_ABORT;
        case CAST_TO_SV__ASSERT: return ALLOW__ASSERT;
        case CAST_TO_SV__NO_CHECK: return ALLOW__NO_CHECK;
        default: return PROHIBIT; // This shouldn't happen for valid ResultPolicy.
    }
}

class Base_s {
public:

    // Default implementation of type_string, giving only a unique string (type_info::hash_code is ostensibly unique).
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{LVD_FMT("SemanticType" << std::uppercase << std::hex << typeid(S_).hash_code())};
        return STR;
    }
    // Default implementation of print.
    template <typename S_, typename C_>
    static std::ostream &print (std::ostream &out, C_ const &cv) {
        return out << S_::template type_string<S_>() << '(' << cv << ')';
    }
    // Another implementation of print where it excludes the parens, in case the concrete value prints them.
    template <typename S_, typename C_>
    static std::ostream &print_without_extra_parens (std::ostream &out, C_ const &cv) {
        return out << S_::template type_string<S_>() << cv;
    }

    /*
    #define LVD_DEFINE_INPLACE_OPERATOR_PROPERTIES_FOR(opname) \
    inline static CheckPolicy constexpr __##opname##_SV__ = ALLOW__VERIFY_OR_THROW; \
    template <typename SV_, typename C_, typename T_> static CheckPolicy constexpr __##opname##_T__ () { return ALLOW__VERIFY_OR_THROW; }

    LVD_DEFINE_INPLACE_OPERATOR_PROPERTIES_FOR(add_eq)
    LVD_DEFINE_INPLACE_OPERATOR_PROPERTIES_FOR(sub_eq)
    LVD_DEFINE_INPLACE_OPERATOR_PROPERTIES_FOR(mul_eq)
    LVD_DEFINE_INPLACE_OPERATOR_PROPERTIES_FOR(div_eq)
    LVD_DEFINE_INPLACE_OPERATOR_PROPERTIES_FOR(mod_eq)
    LVD_DEFINE_INPLACE_OPERATOR_PROPERTIES_FOR(xor_eq)
    LVD_DEFINE_INPLACE_OPERATOR_PROPERTIES_FOR(and_eq)
    LVD_DEFINE_INPLACE_OPERATOR_PROPERTIES_FOR(or_eq)
    LVD_DEFINE_INPLACE_OPERATOR_PROPERTIES_FOR(shl_eq)
    LVD_DEFINE_INPLACE_OPERATOR_PROPERTIES_FOR(shr_eq)

    #undef LVD_DEFINE_INPLACE_OPERATOR_PROPERTIES_FOR
    */

    // Call operator
    template <typename... Args_> decltype(auto) constexpr operator() (Args_&&... args) const { return Base_s{}; }
    // Element operator
    template <typename T_> decltype(auto) constexpr operator[] (T_ &&arg) const { return Base_s{}; }

    // TODO: Move these out into global
    decltype(auto) operator* () const { return Base_s{}; }
    decltype(auto) operator-> () const { return Base_s{}; }
};

template <typename T_, T_ VALUE_>
struct Value_t {
    using T = T_;
    inline static constexpr T VALUE = VALUE_;
};

template <typename T_, T_ VALUE_>
inline static Value_t<T_,VALUE_> constexpr value_v = Value_t<T_,VALUE_>{};

//
// Operator overloads
//

// NOTE: These are best defined in the plain way for now.  Only provide ability to override them later if needed.
// inline decltype(auto) constexpr operator== (Base_s, Base_s) { return Base_s{}; }
// inline decltype(auto) constexpr operator!= (Base_s, Base_s) { return Base_s{}; }
// inline decltype(auto) constexpr operator<= (Base_s, Base_s) { return Base_s{}; }
// inline decltype(auto) constexpr operator< (Base_s, Base_s) { return Base_s{}; }
// inline decltype(auto) constexpr operator>= (Base_s, Base_s) { return Base_s{}; }
// inline decltype(auto) constexpr operator> (Base_s, Base_s) { return Base_s{}; }

inline decltype(auto) constexpr operator+ (Base_s, Base_s) { return Base_s{}; }
inline decltype(auto) constexpr operator- (Base_s, Base_s) { return Base_s{}; }
inline decltype(auto) constexpr operator* (Base_s, Base_s) { return Base_s{}; }
inline decltype(auto) constexpr operator/ (Base_s, Base_s) { return Base_s{}; }
inline decltype(auto) constexpr operator% (Base_s, Base_s) { return Base_s{}; }
inline decltype(auto) constexpr operator^ (Base_s, Base_s) { return Base_s{}; }
inline decltype(auto) constexpr operator& (Base_s, Base_s) { return Base_s{}; }
inline decltype(auto) constexpr operator| (Base_s, Base_s) { return Base_s{}; }
inline decltype(auto) constexpr operator<< (Base_s, Base_s) { return Base_s{}; }
inline decltype(auto) constexpr operator>> (Base_s, Base_s) { return Base_s{}; }

inline decltype(auto) constexpr operator+ (Base_s) { return Base_s{}; }
inline decltype(auto) constexpr operator- (Base_s) { return Base_s{}; }
inline decltype(auto) constexpr operator~ (Base_s) { return Base_s{}; }
inline decltype(auto) constexpr operator! (Base_s) { return Base_s{}; }

// Pre-increment/decrement
inline decltype(auto) constexpr operator++ (Base_s &) { return Base_s{}; }
inline decltype(auto) constexpr operator-- (Base_s &) { return Base_s{}; }
// Post-increment/decrement
inline decltype(auto) constexpr operator++ (Base_s &, int) { return Base_s{}; }
inline decltype(auto) constexpr operator-- (Base_s &, int) { return Base_s{}; }

//
// Functions that define CheckPolicy for various operations
//

// TODO: The SV_ template arguments seem to be wrong, since the semantic classes should not depend on SV_t.
// Figure out if SV_ param should be removed.

inline decltype(auto) constexpr check_policy_for__ctor_default (Base_s) { return value_v<CheckPolicy,ALLOW__VERIFY_OR_THROW>; }

inline decltype(auto) constexpr check_policy_for__ctor_copy_SV (Base_s) { return value_v<CheckPolicy,ALLOW__NO_CHECK>; }
inline decltype(auto) constexpr check_policy_for__ctor_copy_SubSV (Base_s) { return value_v<CheckPolicy,ALLOW__ASSERT>; }
inline decltype(auto) constexpr check_policy_for__ctor_copy_C (Base_s) { return value_v<CheckPolicy,ALLOW__VERIFY_OR_THROW>; }
// TODO: Figure out if this one is right, or should be modified.
template <typename SV_, typename C_, typename T_>
inline decltype(auto) constexpr check_policy_for__ctor_copy_T (Base_s) { return value_v<CheckPolicy,ALLOW__VERIFY_OR_THROW>; }

// The incoming SV is already valid, ostensibly.
inline decltype(auto) constexpr check_policy_for__ctor_move_SV (Base_s) { return value_v<CheckPolicy,ALLOW__NO_CHECK>; }
inline decltype(auto) constexpr check_policy_for__ctor_move_SubSV (Base_s) { return value_v<CheckPolicy,ALLOW__ASSERT>; }
inline decltype(auto) constexpr check_policy_for__ctor_move_C (Base_s) { return value_v<CheckPolicy,ALLOW__VERIFY_OR_THROW>; }
// TODO: Figure out if this one is right, or should be modified.
template <typename SV_, typename C_, typename T_>
inline decltype(auto) constexpr check_policy_for__ctor_move_T (Base_s) { return value_v<CheckPolicy,ALLOW__VERIFY_OR_THROW>; }

// TODO: Figure out if this one is right, or should be modified.
template <typename SV_, typename C_, typename... Args_>
inline decltype(auto) constexpr check_policy_for__ctor_variadic (Base_s) { return value_v<CheckPolicy,ALLOW__VERIFY_OR_THROW>; }

// The incoming SV is already valid, ostensibly.
inline decltype(auto) constexpr check_policy_for__assign_copy_SV (Base_s) { return value_v<CheckPolicy,ALLOW__NO_CHECK>; }
inline decltype(auto) constexpr check_policy_for__assign_copy_SubSV (Base_s) { return value_v<CheckPolicy,ALLOW__ASSERT>; }
// inline decltype(auto) constexpr check_policy_for__assign_copy_C (Base_s) { return value_v<CheckPolicy,ALLOW__VERIFY_OR_THROW>; }
// TODO: Figure out if this one is right, or should be modified.
template <typename SV_, typename C_, typename T_>
inline decltype(auto) constexpr check_policy_for__assign_copy_T (Base_s) { return value_v<CheckPolicy,ALLOW__VERIFY_OR_THROW>; }

// The incoming SV is already valid, ostensibly.
inline decltype(auto) constexpr check_policy_for__assign_move_SV (Base_s) { return value_v<CheckPolicy,ALLOW__NO_CHECK>; }
inline decltype(auto) constexpr check_policy_for__assign_move_SubSV (Base_s) { return value_v<CheckPolicy,ALLOW__ASSERT>; }
// inline decltype(auto) constexpr check_policy_for__assign_move_C (Base_s) { return value_v<CheckPolicy,ALLOW__VERIFY_OR_THROW>; }
// TODO: Figure out if this one is right, or should be modified.
template <typename SV_, typename C_, typename T_>
inline decltype(auto) constexpr check_policy_for__assign_move_T (Base_s) { return value_v<CheckPolicy,ALLOW__VERIFY_OR_THROW>; }

// NOTE: These are best defined in the plain way for now.  Only provide ability to override them later if needed.
// inline decltype(auto) constexpr check_policy_for__eq (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
// inline decltype(auto) constexpr check_policy_for__neq (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
// inline decltype(auto) constexpr check_policy_for__leq (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
// inline decltype(auto) constexpr check_policy_for__lt (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
// inline decltype(auto) constexpr check_policy_for__geq (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
// inline decltype(auto) constexpr check_policy_for__gt (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }

#define LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(SemanticClass_s, opname, CHECK_POLICY) \
    inline decltype(auto) constexpr check_policy_for__##opname##_SV (SemanticClass_s) { return value_v<CheckPolicy,CHECK_POLICY>; } \
    template <typename SV_, typename C_, typename T_> \
    inline decltype(auto) constexpr check_policy_for__##opname##_T (SemanticClass_s) { return value_v<CheckPolicy,CHECK_POLICY>; }

LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(Base_s, add_eq, ALLOW__VERIFY_OR_THROW)
LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(Base_s, sub_eq, ALLOW__VERIFY_OR_THROW)
LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(Base_s, mul_eq, ALLOW__VERIFY_OR_THROW)
LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(Base_s, div_eq, ALLOW__VERIFY_OR_THROW)
LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(Base_s, mod_eq, ALLOW__VERIFY_OR_THROW)
LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(Base_s, xor_eq, ALLOW__VERIFY_OR_THROW)
LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(Base_s, and_eq, ALLOW__VERIFY_OR_THROW)
LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(Base_s, or_eq, ALLOW__VERIFY_OR_THROW)
LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(Base_s, shl_eq, ALLOW__VERIFY_OR_THROW)
LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(Base_s, shr_eq, ALLOW__VERIFY_OR_THROW)

// TODO: Make macros for these.
// TODO: Make SV_SV and templatized ones: SV_T, and T_SV.
inline decltype(auto) constexpr check_policy_for__add (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__sub (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__mul (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__div (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__mod (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__xor (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__and (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__or  (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__shl (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__shr (Base_s, Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }

inline decltype(auto) constexpr check_policy_for__pos (Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__neg (Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__not (Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__bang (Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }

inline decltype(auto) constexpr check_policy_for__preincr (Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__predecr (Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__postincr (Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__postdecr (Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }

template <typename SV_, typename C_, typename... Args_>
inline decltype(auto) constexpr check_policy_for__call (Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
template <typename SV_, typename C_, typename T_>
inline decltype(auto) constexpr check_policy_for__elem (Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }

inline decltype(auto) constexpr check_policy_for__deref (Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }
inline decltype(auto) constexpr check_policy_for__arrow (Base_s) { return Value_t<CheckPolicy,PROHIBIT>{}; }

#define LVD_DEFINE_SEMANTIC_CLASS_METHOD__TYPE_STRING(str) \
    template <typename S_> \
    static std::string const &type_string () { \
        static std::string const STR{str}; \
        return STR; \
    }

// This defines a static method with templatized concrete value; concrete type is C, concrete value is cv.
#define LVD_DEFINE_SEMANTIC_CLASS_METHOD__IS_VALID(function_body) \
    template <typename C> \
    static bool constexpr is_valid (C const &cv) { \
        function_body \
    }

} // end namespace sst
} // end namespace lvd

/*
#define LVD_DEFINE_SEMANTIC_BIN_OP(op, opname, Lhs_s, Rhs_s, Result_s, CHECK_POLICY) \
namespace lvd { \
namespace sst { \
inline decltype(auto) operator op (Lhs_s, Rhs_s) { return Result_s{}; } \
inline decltype(auto) constexpr check_policy_for__##opname (Lhs_s, Rhs_s) { return Value_t<CheckPolicy,CHECK_POLICY>{}; } \
} \
}

#define LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(op, opname, Lhs_s, Rhs_s, Result_s, CHECK_POLICY) \
namespace lvd { \
namespace sst { \
inline decltype(auto) operator op (Lhs_s, Rhs_s) { return Result_s{}; } \
inline decltype(auto) operator op (Rhs_s, Lhs_s) { return Result_s{}; } \
inline decltype(auto) constexpr check_policy_for__##opname (Lhs_s, Rhs_s) { return Value_t<CheckPolicy,CHECK_POLICY>{}; } \
inline decltype(auto) constexpr check_policy_for__##opname (Rhs_s, Lhs_s) { return Value_t<CheckPolicy,CHECK_POLICY>{}; } \
} \
}

#define LVD_DEFINE_SEMANTIC_UN_OP(op, opname, Operand_s, Result_s, CHECK_POLICY) \
namespace lvd { \
namespace sst { \
inline decltype(auto) operator op (Operand_s) { return Result_s{}; } \
inline decltype(auto) constexpr check_policy_for__##opname (Operand_s) { return Value_t<CheckPolicy,CHECK_POLICY>{}; } \
} \
}*/

#define LVD_DEFINE_SEMANTIC_BIN_OP(op, opname, Lhs_s, Rhs_s, Result_s, CHECK_POLICY) \
inline decltype(auto) operator op (Lhs_s, Rhs_s) { return Result_s{}; } \
inline decltype(auto) constexpr check_policy_for__##opname (Lhs_s, Rhs_s) { return Value_t<CheckPolicy,CHECK_POLICY>{}; }

#define LVD_DEFINE_SEMANTIC_BIN_OP_COMMUTATIVE(op, opname, Lhs_s, Rhs_s, Result_s, CHECK_POLICY) \
inline decltype(auto) operator op (Lhs_s, Rhs_s) { return Result_s{}; } \
inline decltype(auto) operator op (Rhs_s, Lhs_s) { return Result_s{}; } \
inline decltype(auto) constexpr check_policy_for__##opname (Lhs_s, Rhs_s) { return Value_t<CheckPolicy,CHECK_POLICY>{}; } \
inline decltype(auto) constexpr check_policy_for__##opname (Rhs_s, Lhs_s) { return Value_t<CheckPolicy,CHECK_POLICY>{}; }

#define LVD_DEFINE_SEMANTIC_UN_OP(op, opname, Operand_s, Result_s, CHECK_POLICY) \
inline decltype(auto) operator op (Operand_s) { return Result_s{}; } \
inline decltype(auto) constexpr check_policy_for__##opname (Operand_s) { return Value_t<CheckPolicy,CHECK_POLICY>{}; }
