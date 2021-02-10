// 2021.01.28 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/sst/semantic_class.hpp"
#include <ostream>
#include <stdexcept>
#include <type_traits>

namespace lvd {
namespace sst {

// Sigil type to get certain behavior from certain constructors.
struct NoCheck { };

// Convenient value of type NoCheck which can be used in constructors to avoid validity check.
inline static NoCheck const no_check = NoCheck{};

// Need a forward declaration of Mutation_t<S_,C_> in order for the friend declaration inside SV_t to work.
template <typename S_, typename C_> class Mutation_t;

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

    // TODO: Figure out if this should have First_ and Second_, so that overload resolution between
    // this and the T ctor is mutually exclusive.
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
    // TODO: Figure out if this should have First_ and Second_, so that overload resolution between
    // this and the T ctor is mutually exclusive.
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
    // TODO: Probably add SV_T and T_SV versions of these
    decltype(auto) operator== (SV_t const &rhs) const { return m_cv == rhs.cv(); }
    decltype(auto) operator!= (SV_t const &rhs) const { return m_cv != rhs.cv(); }
    decltype(auto) operator< (SV_t const &rhs) const { return m_cv < rhs.cv(); }
    decltype(auto) operator<= (SV_t const &rhs) const { return m_cv <= rhs.cv(); }
    decltype(auto) operator> (SV_t const &rhs) const { return m_cv > rhs.cv(); }
    decltype(auto) operator>= (SV_t const &rhs) const { return m_cv >= rhs.cv(); }

    #define LVD_DEFINE_INPLACE_OPERATOR_METHODS_FOR(op, opname) \
    SV_t &operator op (SV_t const &rhs) { \
        m_cv op rhs.cv(); \
        check<decltype(check_policy_for__##opname##_SV(S{}))::VALUE>(); \
        return *this; \
    } \
    template <typename T_> \
    SV_t &operator op (T_ const &rhs) { \
        m_cv op rhs; \
        check<decltype(check_policy_for__##opname##_T<SV_t,C,T_>(S{}))::VALUE>(); \
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

    // Note that operator=, operator->, operator(), and operator[] must each be nonstatic methods; they can't be global functions.
    // TODO: Figure out how to allow non-const version of this -- use scope guard with check at end
    decltype(auto) operator-> () const {
        // A bit of a hack, but apparently you can't call .operator->() as a method on built-in pointer types.
        auto retval = operator_arrow(cv());
        using RetvalSemanticType = decltype(S{}.operator->());
        if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
            auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
            SV_retval.template check<decltype(check_policy_for__arrow(S{}))::VALUE>();
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

    // Note that operator=, operator->, operator(), and operator[] must each be nonstatic methods; they can't be global functions.
    // TODO: Figure out how to allow non-const version of this -- it would be some delegate
    // that calls check<...>() after the value of this is changed.  Use a scope guard.
    template <typename... Args_>
    decltype(auto) operator() (Args_&&... args) const {
        auto retval = cv()(std::forward<Args_>(args)...);
        using RetvalSemanticType = decltype(S{}(std::forward<Args_>(args)...));
        if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
            auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
            SV_retval.template check<decltype(check_policy_for__call<SV_t,C,Args_...>(S{}))::VALUE>();
            return SV_retval;
        } else {
            return retval;
        }
    }
    // Note that operator=, operator->, operator(), and operator[] must each be nonstatic methods; they can't be global functions.
    template <typename T_>
    decltype(auto) operator[] (T_ &&arg) const {
        auto retval = cv()[std::forward<T_>(arg)];
        using RetvalSemanticType = decltype(S{}[std::forward<T_>(arg)]);
        if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
            auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
            SV_retval.template check<decltype(check_policy_for__elem<SV_t,C,T_>(S{}))::VALUE>();
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

    friend class Mutation_t<S_,C_>;
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
    if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) { \
        auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval}; \
        SV_retval.template check<decltype(check_policy_for__##opname(LhsS_{}, RhsS_{}))::VALUE>(); \
        return SV_retval; \
    } else { \
        return retval; \
    } \
} \
template <typename S_, typename C_, typename T_> \
decltype(auto) operator op (SV_t<S_,C_> const &lhs, T_ const &rhs) { \
    auto retval = lhs.cv() op rhs; \
    auto constexpr RESULT_POLICY = decltype(result_policy_for__##opname##_SV_T<SV_t<S_,C_>,C_,T_>(S_{}))::VALUE; \
    if constexpr (RESULT_POLICY != DONT_CAST) { \
        auto SV_retval = SV_t<S_,C_>{no_check, retval}; \
        SV_retval.template check<as_check_policy(RESULT_POLICY)>(); \
        return SV_retval; \
    } else { \
        return retval; \
    } \
} \
template <typename S_, typename C_, typename T_> \
decltype(auto) operator op (T_ const &lhs, SV_t<S_,C_> const &rhs) { \
    auto retval = lhs op rhs.cv(); \
    auto constexpr RESULT_POLICY = decltype(result_policy_for__##opname##_T_SV<SV_t<S_,C_>,C_,T_>(S_{}))::VALUE; \
    if constexpr (RESULT_POLICY != DONT_CAST) { \
        auto SV_retval = SV_t<S_,C_>{no_check, retval}; \
        SV_retval.template check<as_check_policy(RESULT_POLICY)>(); \
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
    if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) { \
        auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval}; \
        SV_retval.template check<decltype(check_policy_for__##opname(S_{}))::VALUE>(); \
        return SV_retval; \
    } else { \
        return retval; \
    } \
}

LVD_DEFINE_GLOBAL_UNARY_OPERATOR_FOR(+, pos)
LVD_DEFINE_GLOBAL_UNARY_OPERATOR_FOR(-, neg)
LVD_DEFINE_GLOBAL_UNARY_OPERATOR_FOR(~, not)
LVD_DEFINE_GLOBAL_UNARY_OPERATOR_FOR(!, bang)
LVD_DEFINE_GLOBAL_UNARY_OPERATOR_FOR(*, deref)

#undef LVD_DEFINE_GLOBAL_UNARY_OPERATOR_FOR

//
// Just define the operators for pre- and post-increment and decrement directly.
//

template <typename S_, typename C_>
decltype(auto) operator++ (SV_t<S_,C_> &operand) {
    auto retval = ++operand.cv();
    using RetvalSemanticType = decltype(S_{}.operator++());
    if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
        auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
        SV_retval.template check<decltype(check_policy_for__preincr(S_{}))::VALUE>();
        return SV_retval;
    } else {
        return retval;
    }
}
template <typename S_, typename C_>
decltype(auto) operator-- (SV_t<S_,C_> &operand) {
    auto retval = --operand.cv();
    using RetvalSemanticType = decltype(S_{}.operator--());
    if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
        auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
        SV_retval.template check<decltype(check_policy_for__predecr(S_{}))::VALUE>();
        return SV_retval;
    } else {
        return retval;
    }
}

template <typename S_, typename C_>
decltype(auto) operator++ (SV_t<S_,C_> &operand, int dummy) {
    auto retval = operand.cv()++;
    using RetvalSemanticType = decltype(S_{}.operator++(dummy));
    if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
        auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
        SV_retval.template check<decltype(check_policy_for__postincr(S_{}))::VALUE>();
        return SV_retval;
    } else {
        return retval;
    }
}
template <typename S_, typename C_>
decltype(auto) operator-- (SV_t<S_,C_> &operand, int dummy) {
    auto retval = operand.cv()--;
    using RetvalSemanticType = decltype(S_{}.operator--(dummy));
    if constexpr (!std::is_same_v<RetvalSemanticType,Base_s>) {
        auto SV_retval = SV_t<RetvalSemanticType,C_>{no_check, retval};
        SV_retval.template check<decltype(check_policy_for__postdecr(S_{}))::VALUE>();
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

} // end namespace sst
} // end namespace lvd
