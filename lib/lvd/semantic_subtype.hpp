// 2021.01.28 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

// #include <array>
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
-   Also at minimum, a semantic class should inherit `SemanticClassDefaultsBase` which defines a
    reasonable default behavior for the various capabilities of a C++ class (constructors, assignment
    operators, operator overloads, etc), such that a broadly useful semantic subtype can be defined
    with only few lines of code.
-   The various attributes defined in `SemanticClassDefaultsBase` can be overridden in a semantic class,
    thereby customizing the particular C++ behavior of the semantic value type `SV_t<S,C>`.
-   Subclassing semantic classes has the naturally expected behavior, though (for now), if `A` and `B`
    are semantic classes with `B` a subclass of `A`, it doesn't follow that `SV_t<B,C>` is a subclass
    of `SV_t<A,C>`.  This might change later.

TODO
-   Allow custom behavior for assert and throw, defined by `S`, with defaults in `SemanticClassDefaultsBase`.
-   Develop some theory and code regarding an actionable subtype hierarchy of semantic types and
    the corresponding semantic subtypes.  For example, let `NonEmpty_s`, `EmailAddr_s`, and `NormedEmailAddr_s`
    denote semantic [C++] classes with the expected definitions, and assume that the class hierarchy is

        NonEmpty_s <- EmailAddr_s <- NormedEmailAddr_s

    Then ideally, it should be that the semantic value types have the same relationship:

        SV_t<NonEmpty_s,std::string> <- SV_t<EmailAddr_s,std::string> <- SV_t<NormedEmailAddr_s,std::string>

    Obviously the semantic class' definitions of `is_valid` would have to be compatible for them to
    actually form a legitimate semantic type hierarchy.
-   Idea for more-complex, ad-hoc in-place modification that still respects the validity:
    -   Create a guard object that "allows modification" of the SV_t object arbitrarily.
    -   There should be an "obtain modification guard" method which produces the guard.
    -   When the guard is destroyed, the validity checker is run, throwing if invalid.
    -   Unfortunately there's no way to guarantee that other non-const references to the object
        don't exist (like there is in Rust).  However, this behavior can be approximated by
        causing the guard object to move the concrete value out of the SV_t, and moving it
        back in once the guard goes out of scope.
-   Idea for a different way to define semantic-subtype-specific behavior -- could define
    operator overloads on Type_t<T_> to define what type results from each operation.
    Not sure if this would be more of a pain in the ass.
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

class SemanticClassDefaultsBase {
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

    inline static CheckPolicy constexpr __ctor_default__ = ALLOW__VERIFY_OR_THROW;

    // The incoming SV is already valid, ostensibly.
    inline static CheckPolicy constexpr __ctor_copy_SV__ = ALLOW__NO_CHECK;
    inline static CheckPolicy constexpr __ctor_copy_C__ = ALLOW__VERIFY_OR_THROW;
    template <typename SV_, typename C_, typename T_> static CheckPolicy constexpr __ctor_copy_T__ () { return ALLOW__VERIFY_OR_THROW; }

    // The incoming SV is already valid, ostensibly.
    inline static CheckPolicy constexpr __ctor_move_SV__ = ALLOW__NO_CHECK;
    inline static CheckPolicy constexpr __ctor_move_C__ = ALLOW__VERIFY_OR_THROW;
    template <typename SV_, typename C_, typename T_> static CheckPolicy constexpr __ctor_move_T__ () { return ALLOW__VERIFY_OR_THROW; }

    template <typename SV_, typename C_, typename... Args_> static CheckPolicy constexpr __ctor_variadic__ () { return ALLOW__VERIFY_OR_THROW; }

    // The incoming SV is already valid, ostensibly.
    inline static CheckPolicy constexpr __assign_copy_SV__ = ALLOW__NO_CHECK;
    template <typename SV_, typename C_, typename T_> static CheckPolicy constexpr __assign_copy_T__ () { return ALLOW__VERIFY_OR_THROW; }

    // The incoming SV is already valid, ostensibly.
    inline static CheckPolicy constexpr __assign_move_SV__ = ALLOW__NO_CHECK;
    template <typename SV_, typename C_, typename T_> static CheckPolicy constexpr __assign_move_T__ () { return ALLOW__VERIFY_OR_THROW; }

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

    #define LVD_DEFINE_GLOBAL_OPERATOR_PROPERTIES_FOR(opname) \
    inline static ResultPolicy constexpr __##opname##_SV_SV__ = DONT_CAST; \
    template <typename SV_, typename C_, typename T_> static ResultPolicy constexpr __##opname##_SV_T__ () { return DONT_CAST; } \
    template <typename SV_, typename C_, typename T_> static CheckPolicy constexpr __##opname##_T_SV__ () { return DONT_CAST; }

    LVD_DEFINE_GLOBAL_OPERATOR_PROPERTIES_FOR(add)
    LVD_DEFINE_GLOBAL_OPERATOR_PROPERTIES_FOR(sub)
    LVD_DEFINE_GLOBAL_OPERATOR_PROPERTIES_FOR(mul)
    LVD_DEFINE_GLOBAL_OPERATOR_PROPERTIES_FOR(div)
    LVD_DEFINE_GLOBAL_OPERATOR_PROPERTIES_FOR(mod)
    LVD_DEFINE_GLOBAL_OPERATOR_PROPERTIES_FOR(xor)
    LVD_DEFINE_GLOBAL_OPERATOR_PROPERTIES_FOR(and)
    LVD_DEFINE_GLOBAL_OPERATOR_PROPERTIES_FOR(or)
    LVD_DEFINE_GLOBAL_OPERATOR_PROPERTIES_FOR(shl)
    LVD_DEFINE_GLOBAL_OPERATOR_PROPERTIES_FOR(shr)

    #undef LVD_DEFINE_GLOBAL_OPERATOR_PROPERTIES_FOR

    #define LVD_DEFINE_UNARY_OPERATOR_PROPERTIES_FOR(opname) \
    inline static ResultPolicy constexpr __##opname##_SV__ = DONT_CAST;

    LVD_DEFINE_UNARY_OPERATOR_PROPERTIES_FOR(not)
    LVD_DEFINE_UNARY_OPERATOR_PROPERTIES_FOR(bang)

    #undef LVD_DEFINE_UNARY_OPERATOR_PROPERTIES_FOR

    // TODO: pre and post increment and decrement.
};

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
        check<S::__ctor_default__>();
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
        check<S::__ctor_copy_SV__>();
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
        check<S::__ctor_copy_SV__>();
    }
    SV_t (NoCheck, SV_t &other)
        :   m_cv{other.cv()}
    {
        // NoCheck
    }

    SV_t (SV_t &&other)
        :   m_cv{other.move_cv()}
    {
        check<S::__ctor_move_SV__>();
    }
    SV_t (NoCheck, SV_t &&other)
        :   m_cv{other.move_cv()}
    {
        // NoCheck
    }

    explicit SV_t (C const &other)
        :   m_cv{other}
    {
        check<S::__ctor_copy_C__>();
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
        check<S::__ctor_copy_C__>();
    }
    explicit SV_t (NoCheck, C &other)
        :   m_cv{static_cast<C const &>(other)}
    {
        // NoCheck
    }

    explicit SV_t (C &&other)
        :   m_cv{std::move(other)}
    {
        check<S::__ctor_move_C__>();
    }
    explicit SV_t (NoCheck, C &&other)
        :   m_cv{std::move(other)}
    {
        // NoCheck
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
    explicit SV_t (First_ &&first, Rest_&&... rest)
        :   m_cv(std::forward<First_>(first), std::forward<Rest_>(rest)...)
    {
        check<S::template __ctor_variadic__<SV_t,C,First_,Rest_...>()>();
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
        check<S::__assign_copy_SV__>();
        return *this;
    }
    SV_t &operator= (SV_t &&other) {
        m_cv = other.move_cv();
        check<S::__assign_move_SV__>();
        return *this;
    }
    template <typename T_>
    SV_t &operator= (T_ const &rhs) {
        m_cv = rhs;
        check<S::template __assign_copy_T__<SV_t,C,T_>()>();
        return *this;
    }
    template <typename T_>
    SV_t &operator= (T_ &&rhs) {
        m_cv = std::move(rhs);
        check<S::template __assign_move_T__<SV_t,C,T_>()>();
        return *this;
    }

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

    #define LVD_DEFINE_UNARY_OPERATOR_METHODS_FOR(op, opname) \
    decltype(auto) operator op () const { \
        auto retval = op cv(); \
        if constexpr (S_::__##opname##_SV__ != DONT_CAST) { \
            auto SV_retval = SV_t<S_,C_>{no_check, retval}; \
            SV_retval.template check<as_check_policy(S_::__##opname##_SV__)>(); \
            return SV_retval; \
        } else { \
            return retval; \
        } \
    } \

    LVD_DEFINE_UNARY_OPERATOR_METHODS_FOR(~, not)
    LVD_DEFINE_UNARY_OPERATOR_METHODS_FOR(!, bang)

    #undef LVD_DEFINE_UNARY_OPERATOR_METHODS_FOR

    // TODO: Figure out how to define this generally.
    // TODO: Figure out how to allow non-const version of this
    decltype(auto) operator* () const { return cv().operator*(); }

    // TODO: Figure out how to allow non-const version of this
    decltype(auto) operator-> () const { return cv().operator->(); }

    explicit operator bool () const { return cv().operator bool(); }

    // TODO: Figure out how to allow non-const version of this -- it would be some delegate
    // that calls check<...>() after the value of this is changed.
    template <typename... Args_>
    decltype(auto) operator() (Args_&&... args) const {
        // TODO: Implement ResultPolicy logic here
        return cv().operator()(std::forward<Args_>(args)...);
    }

    // TODO: Figure out how to allow non-const version of this -- it would be some delegate
    // that calls check<...>() after the value of this is changed.
    template <typename T_>
    decltype(auto) operator[] (T_ &&x) const {
        // TODO: Implement ResultPolicy logic here
        return cv().operator[](std::forward<T_>(x));
    }

    C const &cv () const { return m_cv; }
    C move_cv () { return std::move(m_cv); } // Should use RVO

    template <CheckPolicy L_>
    void check () const {
        check<L_>(cv());
    }

    template <CheckPolicy L_>
    static void check (C const &cv) {
        static_assert(L_ != PROHIBIT, "you tried to use a PROHIBIT'ed method; go up the call stack a few to find which one you had the gall to use.");

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

    C m_cv;
};

//
// Binary operator overloads
//

#define LVD_DEFINE_GLOBAL_OPERATORS_FOR(op, opname) \
template <typename S_, typename C_> \
decltype(auto) operator op (SV_t<S_,C_> const &lhs, SV_t<S_,C_> const &rhs) { \
    auto retval = lhs.cv() op rhs.cv(); \
    if constexpr (S_::__##opname##_SV_SV__ != DONT_CAST) { \
        auto SV_retval = SV_t<S_,C_>{no_check, retval}; \
        SV_retval.template check<as_check_policy(S_::__##opname##_SV_SV__)>(); \
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

LVD_DEFINE_GLOBAL_OPERATORS_FOR(+, add)
LVD_DEFINE_GLOBAL_OPERATORS_FOR(-, sub)
LVD_DEFINE_GLOBAL_OPERATORS_FOR(*, mul)
LVD_DEFINE_GLOBAL_OPERATORS_FOR(/, div)
LVD_DEFINE_GLOBAL_OPERATORS_FOR(%, mod)
LVD_DEFINE_GLOBAL_OPERATORS_FOR(^, xor)
LVD_DEFINE_GLOBAL_OPERATORS_FOR(&, and)
LVD_DEFINE_GLOBAL_OPERATORS_FOR(|, or)
// LVD_DEFINE_GLOBAL_OPERATORS_FOR(<<, shl) // TODO: Figure out how this can work
LVD_DEFINE_GLOBAL_OPERATORS_FOR(>>, shr)

#undef LVD_DEFINE_GLOBAL_OPERATORS_FOR

template <typename S_, typename C_>
inline std::ostream &operator<< (std::ostream &out, SV_t<S_,C_> const &sv) {
    return S_::template print<S_>(out, sv.cv());
}

} // end namespace lvd
