// 2021.01.28 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <cstdlib>
#include "lvd/fmt.hpp" // A bit overkill
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>

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

// Sigil type to get certain behavior from certain constructors.
struct NoCheck { };

// Convenient value of type NoCheck which can be used in constructors to avoid validity check.
inline static NoCheck const no_check = NoCheck{};

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

    // Call operator
    template <typename... Args_> decltype(auto) constexpr operator() (Args_&&... args) const { return Base_s{}; }
    // Element operator
    template <typename T_> decltype(auto) constexpr operator[] (T_ &&arg) const { return Base_s{}; }

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

template <typename... Args_>
inline decltype(auto) constexpr check_policy_for__call (Base_s, Args_&&...) { return Value_t<CheckPolicy,PROHIBIT>{}; }
template <typename T_>
inline decltype(auto) constexpr check_policy_for__elem (Base_s, T_ &&) { return Value_t<CheckPolicy,PROHIBIT>{}; }

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

//
// SV_t -- semantic value
//

template <typename S_, typename C_>
class SV_t {
public:

    static_assert(!std::is_same_v<S_,NoCheck>, "S_ is not allowed to be NoCheck");
    static_assert(!std::is_same_v<C_,NoCheck>, "C_ is not allowed to be NoCheck");

    using S = S_;
    using C = C_;

    SV_t ()
        :   m_cv{}
    {
        check<decltype(check_policy_for__ctor_default(S{}))::VALUE>();
    }
    // TODO: Should NoCheck be a template-only parameter instead?
    SV_t (NoCheck)
        :   m_cv{}
    {
        // NoCheck
    }

    SV_t (SV_t const &other)
        :   m_cv{other.cv()}
    {
        check<decltype(check_policy_for__ctor_copy_SV(S{}))::VALUE>();
    }
    SV_t (NoCheck, SV_t const &other)
        :   m_cv{other.cv()}
    {
        // NoCheck
    }

    // This is necessary to have this constructor be called instead of the templatized one below
    // when passing a non-const reference (e.g. a local var) to the constructor.
    SV_t (SV_t &other)
        :   m_cv{other.cv()}
    {
        check<decltype(check_policy_for__ctor_copy_SV(S{}))::VALUE>();
    }
    SV_t (NoCheck, SV_t &other)
        :   m_cv{other.cv()}
    {
        // NoCheck
    }

    SV_t (SV_t &&other)
        :   m_cv{other.move_cv()}
    {
        check<decltype(check_policy_for__ctor_move_SV(S{}))::VALUE>();
    }
    SV_t (NoCheck, SV_t &&other)
        :   m_cv{other.move_cv()}
    {
        // NoCheck
    }

    //
    // Constructing from concrete type
    //

    explicit SV_t (C const &other)
        :   m_cv{other}
    {
        check<decltype(check_policy_for__ctor_copy_C(S{}))::VALUE>();
    }
    explicit SV_t (NoCheck, C const &other)
        :   m_cv{other}
    {
        // NoCheck
    }

    // This is necessary to have this constructor be called instead of the templatized one below
    // when passing a non-const reference (e.g. a local var) to the constructor.
    explicit SV_t (C &other)
        :   m_cv{static_cast<C const &>(other)}
    {
        check<decltype(check_policy_for__ctor_copy_C(S{}))::VALUE>();
    }
    explicit SV_t (NoCheck, C &other)
        :   m_cv{static_cast<C const &>(other)}
    {
        // NoCheck
    }

    explicit SV_t (C &&other)
        :   m_cv{std::move(other)}
    {
        check<decltype(check_policy_for__ctor_move_C(S{}))::VALUE>();
    }
    explicit SV_t (NoCheck, C &&other)
        :   m_cv{std::move(other)}
    {
        // NoCheck
    }

    //
    // Constructing from a sub-semantic-type.
    //

    template <typename SubS_, typename = std::enable_if_t<std::is_base_of_v<S,SubS_>>>
    SV_t (SV_t<SubS_,C> const &other)
        :   m_cv{other.cv()}
    {
        check<decltype(check_policy_for__ctor_copy_SubSV(S{}))::VALUE>();
    }
    template <typename SubS_, typename = std::enable_if_t<std::is_base_of_v<S,SubS_>>>
    SV_t (NoCheck, SV_t<SubS_,C> const &other)
        :   m_cv{other.cv()}
    {
        // NoCheck
    }

    // This is necessary to have this constructor be called instead of the templatized one below
    // when passing a non-const reference (e.g. a local var) to the constructor.
    template <typename SubS_, typename = std::enable_if_t<std::is_base_of_v<S,SubS_>>>
    SV_t (SV_t<SubS_,C> &other)
        :   m_cv{other.cv()}
    {
        check<decltype(check_policy_for__ctor_copy_SubSV(S{}))::VALUE>();
    }
    template <typename SubS_, typename = std::enable_if_t<std::is_base_of_v<S,SubS_>>>
    SV_t (NoCheck, SV_t<SubS_,C> &other)
        :   m_cv{other.cv()}
    {
        // NoCheck
    }

    template <typename SubS_, typename = std::enable_if_t<std::is_base_of_v<S,SubS_>>>
    SV_t (SV_t<SubS_,C> &&other)
        :   m_cv{other.cv()}
    {
        check<decltype(check_policy_for__ctor_move_SubSV(S{}))::VALUE>();
    }
    template <typename SubS_, typename = std::enable_if_t<std::is_base_of_v<S,SubS_>>>
    SV_t (NoCheck, SV_t<SubS_,C> &&other)
        :   m_cv{other.cv()}
    {
        // NoCheck
    }

    //
    // Perfect variadic forwarding constructor, intended to allow use of C's constructors.
    //

    template <
        typename First_,
        typename... Rest_,
        typename = std::enable_if_t<
            !std::is_same_v<std::decay_t<First_>,NoCheck> &&
            !std::is_same_v<std::decay_t<First_>,SV_t> &&
            !std::is_same_v<std::decay_t<First_>,C>
        >
    >
    explicit SV_t (First_ &&first, Rest_&&... rest)
        :   m_cv(std::forward<First_>(first), std::forward<Rest_>(rest)...)
    {
        check<decltype(check_policy_for__ctor_variadic<SV_t,C,First_,Rest_...>(S{}))::VALUE>();
    }
    template <
        typename First_,
        typename... Rest_,
        typename = std::enable_if_t<
            !std::is_same_v<std::decay_t<First_>,NoCheck> &&
            !std::is_same_v<std::decay_t<First_>,SV_t> &&
            !std::is_same_v<std::decay_t<First_>,C>
        >
    >
    explicit SV_t (NoCheck, First_ &&first, Rest_&&... rest)
        :   m_cv(std::forward<First_>(first), std::forward<Rest_>(rest)...)
    {
        // NoCheck.
    }

    SV_t &operator= (SV_t const &other) {
        m_cv = other.cv();
        check<decltype(check_policy_for__assign_copy_SV(S{}))::VALUE>();
        return *this;
    }
    SV_t &operator= (SV_t &&other) {
        m_cv = other.move_cv();
        check<decltype(check_policy_for__assign_move_SV(S{}))::VALUE>();
        return *this;
    }
    template <typename SubS_, typename = std::enable_if_t<std::is_base_of_v<S,SubS_>>>
    SV_t &operator= (SV_t<SubS_,C> const &other) {
        m_cv = other.cv();
        check<decltype(check_policy_for__assign_copy_SubSV(S{}))::VALUE>();
        return *this;
    }
    template <typename SubS_, typename = std::enable_if_t<std::is_base_of_v<S,SubS_>>>
    SV_t &operator= (SV_t<SubS_,C> &&other) {
        m_cv = other.move_cv();
        check<decltype(check_policy_for__assign_move_SubSV(S{}))::VALUE>();
        return *this;
    }
    template <typename T_>
    SV_t &operator= (T_ const &rhs) {
        m_cv = rhs;
        check<decltype(check_policy_for__assign_copy_T<SV_t,C,T_>(S{}))::VALUE>();
        return *this;
    }
    template <typename T_>
    SV_t &operator= (T_ &&rhs) {
        m_cv = std::move(rhs);
        check<decltype(check_policy_for__assign_move_T<SV_t,C,T_>(S{}))::VALUE>();
        return *this;
    }

    // NOTE: These are best defined in the plain way for now.  Only provide ability to override them using global functions later if needed.
    decltype(auto) operator== (SV_t const &rhs) const { return m_cv == rhs.cv(); }
    decltype(auto) operator!= (SV_t const &rhs) const { return m_cv != rhs.cv(); }
    decltype(auto) operator< (SV_t const &rhs) const { return m_cv < rhs.cv(); }
    decltype(auto) operator<= (SV_t const &rhs) const { return m_cv <= rhs.cv(); }
    decltype(auto) operator> (SV_t const &rhs) const { return m_cv > rhs.cv(); }
    decltype(auto) operator>= (SV_t const &rhs) const { return m_cv >= rhs.cv(); }

    #define LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR(op, opname) \
    SV_t &operator op (SV_t const &rhs) { \
        m_cv op rhs.cv(); \
        check<S::__##opname##_SV__>(); \
        return *this; \
    } \
    template <typename T_> \
    SV_t &operator op (T_ const &rhs) { \
        m_cv op rhs; \
        check<S::template __##opname##_T__<SV_t,C,T_>()>(); \
        return *this; \
    }

    LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR(+=, add_eq)
    LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR(-=, sub_eq)
    LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR(*=, mul_eq)
    LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR(/=, div_eq)
    LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR(%=, mod_eq)
    LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR(^=, xor_eq)
    LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR(&=, and_eq)
    LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR(|=, or_eq)
    LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR(<<=, shl_eq)
    LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR(>>=, shr_eq)

    #undef LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR

    // TODO: Figure out how to allow non-const version of this -- use scope guard with check at end
    decltype(auto) operator* () const {
        auto retval = *cv();
        using RetvalSemanticType = decltype(*S{});
        using CheckPolicyValueType = decltype(check_policy_for__deref(S{}));
        if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
            auto constexpr check_policy = CheckPolicyValueType::VALUE;
            auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
            SV_retval.template check<check_policy>();
            return SV_retval;
        } else {
            return retval;
        }
    }
    decltype(auto) operator-> () const {
        // A bit of a hack, but apparently you can't call .operator->() as a method on built-in pointer types.
        auto retval = operator_arrow(cv());
        using RetvalSemanticType = decltype(S{}.operator->());
        using CheckPolicyValueType = decltype(check_policy_for__arrow(S{}));
        if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
            auto constexpr check_policy = CheckPolicyValueType::VALUE;
            auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
            SV_retval.template check<check_policy>();
            return SV_retval;
        } else {
            return retval;
        }
    }

    explicit operator bool () const { return cv().operator bool(); }

    // Casting to semantic supertype.
    template <typename SuperS_, typename = std::enable_if_t<std::is_base_of_v<SuperS_,S>>>
    operator SV_t<SuperS_,C> const & () const {
        // This works because both only contain a single member var of type C.
        return *reinterpret_cast<SV_t<SuperS_,C> const *>(this);
    }

    // TODO: Figure out how to allow non-const version of this -- it would be some delegate
    // that calls check<...>() after the value of this is changed.  Use a scope guard.
    template <typename... Args_>
    decltype(auto) operator() (Args_&&... args) const {
        auto retval = cv()(std::forward<Args_>(args)...);
        using RetvalSemanticType = decltype(S{}(std::forward<Args_>(args)...));
        using CheckPolicyValueType = decltype(check_policy_for__call(S{}, std::forward<Args_>(args)...));
        if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
            auto constexpr check_policy = CheckPolicyValueType::VALUE;
            auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
            SV_retval.template check<check_policy>();
            return SV_retval;
        } else {
            return retval;
        }
    }
    template <typename T_>
    decltype(auto) operator[] (T_ &&arg) const {
        auto retval = cv()[std::forward<T_>(arg)];
        using RetvalSemanticType = decltype(S{}[std::forward<T_>(arg)]);
        using CheckPolicyValueType = decltype(check_policy_for__elem(S{}, std::forward<T_>(arg)));
        if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
            auto constexpr check_policy = CheckPolicyValueType::VALUE;
            auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
            SV_retval.template check<check_policy>();
            return SV_retval;
        } else {
            return retval;
        }
    }

    C const &cv () const { return m_cv; }
    C move_cv () { return std::move(m_cv); } // Should use RVO

    template <CheckPolicy L_>
    void check () const {
        check<L_>(cv());
    }

    template <CheckPolicy L_>
    static void check (C const &cv) {
        static_assert(L_ != PROHIBIT, "you tried to use a PROHIBIT'ed method; go up the call stack a few to find which prohibited method you had the gall to use.");

        if constexpr (L_ == ALLOW__VERIFY_OR_THROW) {
            if (!S::is_valid(cv))
                throw std::runtime_error("!S::is_valid");
        } else if constexpr (L_ == ALLOW__VERIFY_OR_ABORT) {
            if (!S::is_valid(cv))
                std::abort();
        } else if constexpr (L_ == ALLOW__ASSERT) {
            assert(S::is_valid(cv));
        } else if constexpr (L_ == ALLOW__NO_CHECK) {
            // Nothing to do.
        } else {
            // Invalid, but do nothing.
        }
    }

private:

    C &cv_nonconst () { return m_cv; }

    // This simulates being able to call x.operator->() even when x is a built-in pointer type.
    template <typename T_>
    static decltype(auto) operator_arrow (T_ const &x) {
        if constexpr (std::is_pointer_v<std::decay_t<T_>>) {
            // There is no operator-> method on a built-in pointer type.
            return x;
        } else {
            return x.operator->();
        }
    }

    C m_cv;
};

template <typename T_> struct is_SV_t_ : public std::false_type { };
template <typename S_, typename C_> struct is_SV_t_<SV_t<S_,C_>> : public std::true_type { };

// Determines if a given type T_ is a SV_t<S_,C_> for some types S_, C_.
template <typename T_>
inline bool constexpr is_SV_t = is_SV_t_<T_>::value;


// This will make a semantic value (SV_t) with a specified semantic class and [potentially] deduced concrete type.
template <typename S_, typename C_>
decltype(auto) make_sv (C_ &&cv) {
    return SV_t<S_,std::decay_t<C_>>(std::forward<C_>(cv));
}

// This is the same as make_sv, except that it will "collapse" a SV_t that has matching semantic class passed in
// as the concrete type, so that no nesting of SV_t types happen.
template <typename S_, typename C_>
decltype(auto) make_sv__with_collapse (C_ &&cv) {
    using decay_C = std::decay_t<C_>;
    if constexpr (is_SV_t<decay_C>) {
        if constexpr (std::is_same_v<typename decay_C::S,S_>) {
            // Collapse if the semantic classes are the same.
            return make_sv__with_collapse<S_>(cv.cv());
        } else {
            // Otherwise don't collapse.
            return make_sv<S_>(std::forward<C_>(cv));
        }
    } else {
        // If it's not an SV_t, then no possibility for collapse.
        return make_sv<S_>(std::forward<C_>(cv));
    }
}


#define LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(op, opname) \
template <typename LhsS_, typename RhsS_, typename C_> \
decltype(auto) operator op (SV_t<LhsS_,C_> const &lhs, SV_t<RhsS_,C_> const &rhs) { \
    auto retval = lhs.cv() op rhs.cv(); \
    using RetvalSemanticType = decltype(LhsS_{} op RhsS_{}); \
    using CheckPolicyValueType = decltype(check_policy_for__##opname(LhsS_{}, RhsS_{})); \
    if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) { \
        auto constexpr check_policy = CheckPolicyValueType::VALUE; \
        auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval}; \
        SV_retval.template check<check_policy>(); \
        return SV_retval; \
    } else { \
        return retval; \
    } \
} \
template <typename S_, typename C_, typename T_> \
decltype(auto) operator op (SV_t<S_,C_> const &lhs, T_ const &rhs) { \
    auto retval = lhs.cv() op rhs; \
    if constexpr (S_::template __##opname##_SV_T__<SV_t<S_,C_>,C_,T_>() != DONT_CAST) { \
        auto SV_retval = SV_t<S_,C_>{no_check, retval}; \
        SV_retval.template check<as_check_policy(S_::template __##opname##_SV_T__<SV_t<S_,C_>,C_,T_>())>(); \
        return SV_retval; \
    } else { \
        return retval; \
    } \
} \
template <typename S_, typename C_, typename T_> \
decltype(auto) operator op (T_ const &lhs, SV_t<S_,C_> const &rhs) { \
    auto retval = lhs op rhs.cv(); \
    if constexpr (S_::template __##opname##_T_SV__<SV_t<S_,C_>,C_,T_>() != DONT_CAST) { \
        auto SV_retval = SV_t<S_,C_>{no_check, retval}; \
        SV_retval.template check<as_check_policy(S_::template __##opname##_T_SV__<SV_t<S_,C_>,C_,T_>())>(); \
        return SV_retval; \
    } else { \
        return retval; \
    } \
}

// NOTE: These are best defined in the plain way for now.  Only provide ability to override them later if needed.
// LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(==, eq)
// LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(!=, neq)
// LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(<=, leq)
// LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(<, lt)
// LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(>=, geq)
// LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(>, gt)

LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(+, add)
LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(-, sub)
LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(*, mul)
LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(/, div)
LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(%, mod)
LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(^, xor)
LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(&, and)
LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(|, or)
// LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(<<, shl) // TODO: Figure out how this can work
LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR(>>, shr)

#undef LVD_DEFINE_GLOBAL_BINARY_OPERATORS_FOR

//
// Unary operators
//

#define LVD_DEFINE_GLOBAL_UNARY_OPERATOR_FOR(op, opname) \
template <typename S_, typename C_> \
decltype(auto) operator op (SV_t<S_,C_> const &operand) { \
    auto retval = op operand.cv(); \
    using RetvalSemanticType = decltype(op S_{}); \
    using CheckPolicyValueType = decltype(check_policy_for__##opname(S_{})); \
    if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) { \
        auto constexpr check_policy = CheckPolicyValueType::VALUE; \
        auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval}; \
        SV_retval.template check<check_policy>(); \
        return SV_retval; \
    } else { \
        return retval; \
    } \
}

LVD_DEFINE_GLOBAL_UNARY_OPERATOR_FOR(+, pos)
LVD_DEFINE_GLOBAL_UNARY_OPERATOR_FOR(-, neg)
LVD_DEFINE_GLOBAL_UNARY_OPERATOR_FOR(~, not)
LVD_DEFINE_GLOBAL_UNARY_OPERATOR_FOR(!, bang)

#undef LVD_DEFINE_GLOBAL_UNARY_OPERATOR_FOR

//
// Just define the operators for pre- and post-increment and decrement directly.
//

template <typename S_, typename C_>
decltype(auto) operator++ (SV_t<S_,C_> &operand) {
    auto retval = ++operand.cv();
    using RetvalSemanticType = decltype(S_{}.operator++());
    using CheckPolicyValueType = decltype(check_policy_for__preincr(S_{}));
    if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
        auto constexpr check_policy = CheckPolicyValueType::VALUE;
        auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
        SV_retval.template check<check_policy>();
        return SV_retval;
    } else {
        return retval;
    }
}
template <typename S_, typename C_>
decltype(auto) operator-- (SV_t<S_,C_> &operand) {
    auto retval = --operand.cv();
    using RetvalSemanticType = decltype(S_{}.operator--());
    using CheckPolicyValueType = decltype(check_policy_for__predecr(S_{}));
    if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
        auto constexpr check_policy = CheckPolicyValueType::VALUE;
        auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
        SV_retval.template check<check_policy>();
        return SV_retval;
    } else {
        return retval;
    }
}

template <typename S_, typename C_>
decltype(auto) operator++ (SV_t<S_,C_> &operand, int dummy) {
    auto retval = operand.cv()++;
    using RetvalSemanticType = decltype(S_{}.operator++(dummy));
    using CheckPolicyValueType = decltype(check_policy_for__postincr(S_{}));
    if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
        auto constexpr check_policy = CheckPolicyValueType::VALUE;
        auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
        SV_retval.template check<check_policy>();
        return SV_retval;
    } else {
        return retval;
    }
}
template <typename S_, typename C_>
decltype(auto) operator-- (SV_t<S_,C_> &operand, int dummy) {
    auto retval = operand.cv()--;
    using RetvalSemanticType = decltype(S_{}.operator--(dummy));
    using CheckPolicyValueType = decltype(check_policy_for__postdecr(S_{}));
    if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
        auto constexpr check_policy = CheckPolicyValueType::VALUE;
        auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
        SV_retval.template check<check_policy>();
        return SV_retval;
    } else {
        return retval;
    }
}

// Convenient printing of SV_t.

template <typename S_, typename C_>
inline std::ostream &operator<< (std::ostream &out, SV_t<S_,C_> const &sv) {
    return S_::template print<S_>(out, sv.cv());
}

} // end namespace lvd
