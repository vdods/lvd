// 2021.01.28 - Copyright Victor Dods - Licensed under Apache 2.0

#include <cmath>
#include "lvd/IndexedTuple_t.hpp"
#include "lvd/req.hpp"
#include "lvd/semantic_subtype.hpp"
#include "lvd/test.hpp"
#include "lvd/type.hpp"
#include "lvd/type_string_of.hpp"
#include <limits>

using namespace std::string_literals;

#define LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING(str) \
    template <typename S_> \
    static std::string const &type_string () { \
        static std::string const STR{str}; \
        return STR; \
    }

// This defines a static method with templatized concrete value; concrete type is C, concrete value is cv.
#define LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(function_body) \
    template <typename C> \
    static bool constexpr is_valid (C const &cv) { \
        function_body \
    }

// TODO: Make the CheckPolicy part actually use `operator op` using some special types that include Lhs_s and Rhs_s.
#define LVD_DEFINE_SEMANTIC_OPERATOR(op, opname, Lhs_s, Rhs_s, Result_s, CHECK_POLICY) \
    inline decltype(auto) operator op (Lhs_s, Rhs_s) { return Result_s{}; } \
    inline decltype(auto) constexpr check_policy_for__##opname (Lhs_s, Rhs_s) { return Value_t<CheckPolicy,CHECK_POLICY>{}; }

namespace lvd {

struct NonEmpty_s : public Base_s {
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"NonEmpty"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return !cv.empty();
    }

    // Disallow default construction, since that would be an empty string.
    inline static CheckPolicy constexpr __ctor_default__ = PROHIBIT;

    // Adding to a nonempty string can't make it empty.
    inline static ResultPolicy constexpr __add_eq_SV__ = CAST_TO_SV__NO_CHECK;
    template <typename SV_, typename C_, typename T_> static ResultPolicy constexpr __add_eq_T__ () { return CAST_TO_SV__NO_CHECK; }
    // Sum of nonempty string with anything else is nonempty string.
    inline static ResultPolicy constexpr __add_SV_SV__ = CAST_TO_SV__NO_CHECK;
    template <typename SV_, typename C_, typename T_> static ResultPolicy constexpr __add_SV_T__ () { return CAST_TO_SV__NO_CHECK; }
    template <typename SV_, typename C_, typename T_> static ResultPolicy constexpr __add_T_SV__ () { return CAST_TO_SV__NO_CHECK; }
};

// inline decltype(auto) constexpr operator+ (NonEmpty_s, NonEmpty_s) { return std::pair(NonEmpty_s{}, ALLOW__NO_CHECK); }
LVD_DEFINE_SEMANTIC_OPERATOR(+, add, NonEmpty_s, NonEmpty_s, NonEmpty_s, ALLOW__NO_CHECK)

// Semantic value type factories.
template <typename C_> using NonEmpty_t = SV_t<NonEmpty_s,C_>;

// Arguably this alias is not necessary, NonEmpty_t<std::string> is plenty terse.
using NonEmptyString = NonEmpty_t<std::string>;

NonEmptyString make_nonempty_string () {
    return NonEmptyString("OSTRICH");
}

LVD_TEST_BEGIN(007__semantic_subtype__00)
    auto s1 = NonEmptyString{"blah"};
    auto s2 = NonEmptyString{"thingy"s};

    test_log << Log::dbg() << LVD_REFLECT(s1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(s2) << '\n';

    auto s3 = s1 + s2;
    auto s4 = s1 + "stuff";
    auto s5 = s1 + 'a';
    auto s6 = s1 + "ostriches"s;

    test_log << Log::dbg() << LVD_REFLECT(s3) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(s4) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(s5) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(s6) << '\n';

    // Testing use of some of std::string's other constructors
    auto s7 = NonEmptyString{10, 'x'};
    auto s8 = NonEmptyString{{'a', 'b', 'c', 'd'}};
    auto s9 = NonEmptyString{"a very long string", 10};

    test_log << Log::dbg() << LVD_REFLECT(s7) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(s8) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(s9) << '\n';

    // Testing copy and move constructors
    auto s10 = NonEmptyString{s7};
    auto s11 = NonEmptyString{make_nonempty_string()};

    test_log << Log::dbg() << LVD_REFLECT(s10) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(s11) << '\n';

    // Uncommenting this should cause a compile error "static assertion failed: you tried to use a PROHIBIT'ed method; ..."
//     auto bad = NonEmptyString{};

    test::call_function_and_expect_exception<std::runtime_error>([](){
        NonEmptyString{""};
    });
LVD_TEST_END

// Extended real line (this includes positive and negative infinity) with not-a-number value(s).
struct NaNExtReal_s : virtual Base_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("NaNExtReal")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return true;)
};

struct NaN_s : virtual NaNExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("NaN")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return std::isnan(cv);)
};

// NOTE: This sort of thing could go in semantic_subtype.hpp itself.
// https://en.wikipedia.org/wiki/Extended_real_number_line -- this includes positive and negative infinity, but not NaN.
struct ExtReal_s : virtual NaNExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("ExtReal")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return !std::isnan(cv);)
};

struct Infinite_s : virtual ExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("Infinite")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return !std::isfinite(cv);)
};

struct ExtNonZero_s : virtual ExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("ExtNonZero")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv != C(0);)
};

struct ExtNonNeg_s : virtual ExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("ExtNonNeg")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv >= C(0);)
};

struct ExtNonPos_s : virtual ExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("ExtNonPos")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv <= C(0);)
};

struct ExtPos_s : virtual ExtNonNeg_s, virtual ExtNonZero_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("ExtPos")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv > C(0);)
};

struct ExtNeg_s : virtual ExtNonPos_s, virtual ExtNonZero_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("ExtNeg")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv < C(0);)
};

struct Infinity_s : virtual ExtPos_s, virtual Infinite_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("Infinity")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv == std::numeric_limits<C>::infinity();)
};

struct NegInfinity_s : virtual ExtNeg_s, virtual Infinite_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("NegInfinity")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv == -std::numeric_limits<C>::infinity();)
};

// Represents real numbers.  Relative to ExtReal_s, this is the subset of finite values.
struct Real_s : virtual ExtReal_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("Real")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return std::isfinite(cv);)
};

struct NonZero_s : virtual Real_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("NonZero")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv != C(0) && std::isfinite(cv);)
};

struct NonPos_s : virtual Real_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("NonPos")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv <= C(0) && std::isfinite(cv);)
};

struct NonNeg_s : virtual Real_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("NonNeg")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv >= C(0) && std::isfinite(cv);)
};

struct Pos_s : virtual NonNeg_s, virtual NonZero_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("Pos")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv > C(0) && std::isfinite(cv);)
};

struct Neg_s : virtual NonPos_s, virtual NonZero_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("Neg")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv < C(0) && std::isfinite(cv);)
};

struct Zero_s : virtual NonNeg_s, virtual NonPos_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("Zero")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv == C(0);)
};

struct One_s : virtual Pos_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("One")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv == C(1);)
};

struct NegOne_s : virtual Neg_s {
    LVD_DEFINE_SEMANTIC_CLASS_TYPE_STRING("NegOne")
    LVD_DEFINE_SEMANTIC_CLASS_IS_VALID(return cv == C(-1);)
};

// Catch-all

LVD_DEFINE_SEMANTIC_OPERATOR(+, add, NaNExtReal_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, NaNExtReal_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, NaNExtReal_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, NaNExtReal_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)

// LVD_DEFINE_SEMANTIC_OPERATOR(+, add, NaNExtReal_s, Zero_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(+, add, Zero_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, NaNExtReal_s, Zero_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, Zero_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, NaNExtReal_s, Zero_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, Zero_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(/, div, NaNExtReal_s, Zero_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(/, div, Zero_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
//
// LVD_DEFINE_SEMANTIC_OPERATOR(+, add, NaNExtReal_s, ExtPos_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(+, add, ExtPos_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, NaNExtReal_s, ExtPos_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, ExtPos_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, NaNExtReal_s, ExtPos_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, ExtPos_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(/, div, NaNExtReal_s, ExtPos_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(/, div, ExtPos_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
//
// LVD_DEFINE_SEMANTIC_OPERATOR(+, add, NaNExtReal_s, ExtNeg_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(+, add, ExtNeg_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, NaNExtReal_s, ExtNeg_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, ExtNeg_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, NaNExtReal_s, ExtNeg_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, ExtNeg_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(/, div, NaNExtReal_s, ExtNeg_s, NaNExtReal_s, ALLOW__NO_CHECK)
// LVD_DEFINE_SEMANTIC_OPERATOR(/, div, ExtNeg_s, NaNExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_OPERATOR(+, add, ExtReal_s, ExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK) // inf + -inf = NaN
// LVD_DEFINE_SEMANTIC_OPERATOR(+, ExtReal_s, ExtReal_s, ExtReal_s, ALLOW__VERIFY_OR_THROW) // inf + -inf = NaN; throw
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, ExtReal_s, ExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK) // inf + -inf = NaN
// LVD_DEFINE_SEMANTIC_OPERATOR(-, ExtReal_s, ExtReal_s, ExtReal_s, ALLOW__VERIFY_OR_THROW) // inf - inf = NaN; throw
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, ExtReal_s, ExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK) // 0 * infinity = NaN
// LVD_DEFINE_SEMANTIC_OPERATOR(*, ExtReal_s, ExtReal_s, ExtReal_s, ALLOW__VERIFY_OR_THROW) // 0 * infinity = NaN; throw
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, ExtReal_s, ExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK) // NaN in the case of 0/0
// LVD_DEFINE_SEMANTIC_OPERATOR(/, ExtReal_s, ExtReal_s, ExtReal_s, ALLOW__VERIFY_OR_THROW) // NaN in the case of 0/0; throw
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, ExtReal_s, ExtNonZero_s, NaNExtReal_s, ALLOW__NO_CHECK) // inf / inf = NaN

LVD_DEFINE_SEMANTIC_OPERATOR(+, add, ExtReal_s, Zero_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(+, add, Zero_s, ExtReal_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, ExtReal_s, Zero_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, Zero_s, ExtReal_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, ExtReal_s, Zero_s, NaNExtReal_s, ALLOW__NO_CHECK) // 0 * infinity = NaN
// LVD_DEFINE_SEMANTIC_OPERATOR(*, ExtReal_s, Zero_s, ExtReal_s, ALLOW__VERIFY_OR_THROW) // 0 * infinity = NaN; throw
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, Zero_s, ExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK) // 0 * infinity = NaN
// LVD_DEFINE_SEMANTIC_OPERATOR(*, Zero_s, ExtReal_s, ExtReal_s, ALLOW__VERIFY_OR_THROW) // 0 * infinity = NaN; throw
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, ExtReal_s, Zero_s, NaNExtReal_s, ALLOW__NO_CHECK) // NaN in the case of 0/0
// LVD_DEFINE_SEMANTIC_OPERATOR(/, ExtReal_s, Zero_s, ExtReal_s, ALLOW__VERIFY_OR_THROW) // NaN in the case of 0/0; throw
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, Zero_s, ExtReal_s, NaNExtReal_s, ALLOW__NO_CHECK) // NaN in the case of 0/0
// LVD_DEFINE_SEMANTIC_OPERATOR(/, Zero_s, ExtReal_s, ExtReal_s, ALLOW__VERIFY_OR_THROW) // NaN in the case of 0/0; throw

LVD_DEFINE_SEMANTIC_OPERATOR(+, add, ExtNonZero_s, Zero_s, ExtNonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(+, add, Zero_s, ExtNonZero_s, ExtNonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, ExtNonZero_s, Zero_s, ExtNonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, Zero_s, ExtNonZero_s, ExtNonZero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, ExtNonZero_s, Zero_s, NaNExtReal_s, ALLOW__NO_CHECK) // 0 * infinity = NaN
// LVD_DEFINE_SEMANTIC_OPERATOR(*, ExtNonZero_s, Zero_s, ExtNonZero_s, ALLOW__VERIFY_OR_THROW) // 0 * infinity = NaN; throw
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, Zero_s, ExtNonZero_s, NaNExtReal_s, ALLOW__NO_CHECK) // 0 * infinity = NaN
// LVD_DEFINE_SEMANTIC_OPERATOR(*, Zero_s, ExtNonZero_s, ExtNonZero_s, ALLOW__VERIFY_OR_THROW) // 0 * infinity = NaN; throw
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, ExtNonZero_s, Zero_s, ExtReal_s, ALLOW__NO_CHECK) // ?
// LVD_DEFINE_SEMANTIC_OPERATOR(/, ExtNonZero_s, Zero_s, ExtNonZero_s, ALLOW__VERIFY_OR_THROW) // NaN in the case of 0/0; throw
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, Zero_s, ExtNonZero_s, ExtReal_s, ALLOW__NO_CHECK) // ?
// LVD_DEFINE_SEMANTIC_OPERATOR(/, Zero_s, ExtNonZero_s, ExtNonZero_s, ALLOW__VERIFY_OR_THROW) // NaN in the case of 0/0; throw


// Trying to define the minimal set of operator overloads

LVD_DEFINE_SEMANTIC_OPERATOR(+, add, ExtNeg_s, ExtNeg_s, ExtNeg_s, ALLOW__NO_CHECK) // Overflow goes infinite
LVD_DEFINE_SEMANTIC_OPERATOR(+, add, ExtNeg_s, ExtPos_s, NaNExtReal_s, ALLOW__NO_CHECK) // Sum of inf and -inf is NaN
LVD_DEFINE_SEMANTIC_OPERATOR(+, add, ExtPos_s, ExtPos_s, ExtPos_s, ALLOW__NO_CHECK) // Overflow goes infinite
LVD_DEFINE_SEMANTIC_OPERATOR(+, add, ExtPos_s, ExtNeg_s, NaNExtReal_s, ALLOW__NO_CHECK) // Sum of inf and -inf is NaN

LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, ExtNeg_s, ExtNeg_s, NaNExtReal_s, ALLOW__NO_CHECK) // Difference of infinities is NaN
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, ExtNeg_s, ExtPos_s, ExtNeg_s, ALLOW__NO_CHECK) // Overflow goes infinite
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, ExtPos_s, ExtPos_s, NaNExtReal_s, ALLOW__NO_CHECK) // Difference of infinities is NaN
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, ExtPos_s, ExtNeg_s, ExtPos_s, ALLOW__NO_CHECK) // Overflow goes infinite

LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, ExtNeg_s, ExtNeg_s, ExtNonNeg_s, ALLOW__NO_CHECK) // Product of near-zero values can be 0.
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, ExtNeg_s, ExtPos_s, ExtNonPos_s, ALLOW__NO_CHECK) // Product of near-zero values can be 0.
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, ExtPos_s, ExtPos_s, ExtNonNeg_s, ALLOW__NO_CHECK) // Product of near-zero values can be 0.
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, ExtPos_s, ExtNeg_s, ExtNonPos_s, ALLOW__NO_CHECK) // Product of near-zero values can be 0.

LVD_DEFINE_SEMANTIC_OPERATOR(/, div, ExtNeg_s, ExtNeg_s, NaNExtReal_s, ALLOW__NO_CHECK) // Dividing infinities gives NaN
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, ExtNeg_s, ExtPos_s, NaNExtReal_s, ALLOW__NO_CHECK) // Dividing infinities gives NaN
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, ExtPos_s, ExtPos_s, NaNExtReal_s, ALLOW__NO_CHECK) // Dividing infinities gives NaN
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, ExtPos_s, ExtNeg_s, NaNExtReal_s, ALLOW__NO_CHECK) // Dividing infinities gives NaN


LVD_DEFINE_SEMANTIC_OPERATOR(+, add, Neg_s, Neg_s, Neg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(+, add, Neg_s, Pos_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(+, add, Pos_s, Pos_s, Pos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(+, add, Pos_s, Neg_s, ExtReal_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, Neg_s, Neg_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, Neg_s, Pos_s, Neg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, Pos_s, Pos_s, ExtReal_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, Pos_s, Neg_s, Pos_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, Neg_s, Neg_s, Pos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, Neg_s, Pos_s, Neg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, Pos_s, Pos_s, Pos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, Pos_s, Neg_s, Neg_s, ALLOW__NO_CHECK)

LVD_DEFINE_SEMANTIC_OPERATOR(/, div, Neg_s, Neg_s, Pos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, Neg_s, Pos_s, Neg_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, Pos_s, Pos_s, Pos_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, Pos_s, Neg_s, Neg_s, ALLOW__NO_CHECK)


LVD_DEFINE_SEMANTIC_OPERATOR(+, add, Zero_s, Zero_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(-, sub, Zero_s, Zero_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(*, mul, Zero_s, Zero_s, Zero_s, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_OPERATOR(/, div, Zero_s, Zero_s, NaN_s, ALLOW__NO_CHECK)

// // TODO: Set the CheckPolicy
// // Zero_s acts as additive identity.
// // inline decltype(auto) constexpr operator+ (Zero_s, ExtReal_s) { return std::pair(ExtReal_s{}, ALLOW__VERIFY_OR_THROW); }
// // inline decltype(auto) constexpr operator+ (ExtReal_s, Zero_s) { return std::pair(ExtReal_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (Zero_s, NonNeg_s) { return std::pair(NonNeg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (NonNeg_s, Zero_s) { return std::pair(NonNeg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (Zero_s, NonPos_s) { return std::pair(NonPos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (NonPos_s, Zero_s) { return std::pair(NonPos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (Zero_s, Neg_s) { return std::pair(Neg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (Neg_s, Zero_s) { return std::pair(Neg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (Zero_s, Pos_s) { return std::pair(Pos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (Pos_s, Zero_s) { return std::pair(Pos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (Zero_s, NegOne_s) { return std::pair(NegOne_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (NegOne_s, Zero_s) { return std::pair(NegOne_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (Zero_s, One_s) { return std::pair(One_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (One_s, Zero_s) { return std::pair(One_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator+ (Zero_s, Zero_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
//
// // Zero_s also has simple action with respect to subtraction
// // inline decltype(auto) constexpr operator- (Zero_s, ExtReal_s) { return std::pair(ExtReal_s{}, ALLOW__VERIFY_OR_THROW); }
// // inline decltype(auto) constexpr operator- (ExtReal_s, Zero_s) { return std::pair(ExtReal_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (Zero_s, NonNeg_s) { return std::pair(NonPos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (NonNeg_s, Zero_s) { return std::pair(NonNeg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (Zero_s, NonPos_s) { return std::pair(NonNeg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (NonPos_s, Zero_s) { return std::pair(NonPos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (Zero_s, Neg_s) { return std::pair(Pos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (Neg_s, Zero_s) { return std::pair(Neg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (Zero_s, Pos_s) { return std::pair(Neg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (Pos_s, Zero_s) { return std::pair(Pos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (Zero_s, NegOne_s) { return std::pair(One_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (NegOne_s, Zero_s) { return std::pair(NegOne_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (Zero_s, One_s) { return std::pair(NegOne_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (One_s, Zero_s) { return std::pair(One_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator- (Zero_s, Zero_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
//
// // Zero_s crushes everything multiplicatively.
// // inline decltype(auto) constexpr operator* (Zero_s, ExtReal_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// // inline decltype(auto) constexpr operator* (ExtReal_s, Zero_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (Zero_s, NonNeg_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (NonNeg_s, Zero_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (Zero_s, NonPos_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (NonPos_s, Zero_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (Zero_s, Neg_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (Neg_s, Zero_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (Zero_s, Pos_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (Pos_s, Zero_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (Zero_s, NegOne_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (NegOne_s, Zero_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (Zero_s, One_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (One_s, Zero_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (Zero_s, Zero_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
//
// // Zero_s causes problems through division.
// // inline decltype(auto) constexpr operator/ (Zero_s, ExtReal_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// // inline decltype(auto) constexpr operator/ (ExtReal_s, Zero_s) { return std::pair(NaN_s{}, ALLOW__VERIFY_OR_THROW); } // Should be NonFinite_s{}, ALLOW__NO_CHECK
// inline decltype(auto) constexpr operator/ (Zero_s, NonNeg_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (NonNeg_s, Zero_s) { return std::pair(Infinity_s{}, ALLOW__VERIFY_OR_THROW); } // Could also be NaN_s
// inline decltype(auto) constexpr operator/ (Zero_s, NonPos_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (NonPos_s, Zero_s) { return std::pair(NegInfinity_s{}, ALLOW__VERIFY_OR_THROW); } // Could also be NaN_s
// inline decltype(auto) constexpr operator/ (Zero_s, Neg_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (Neg_s, Zero_s) { return std::pair(NegInfinity_s{}, ALLOW__NO_CHECK); }
// inline decltype(auto) constexpr operator/ (Zero_s, Pos_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (Pos_s, Zero_s) { return std::pair(Infinity_s{}, ALLOW__NO_CHECK); }
// inline decltype(auto) constexpr operator/ (Zero_s, NegOne_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (NegOne_s, Zero_s) { return std::pair(NegInfinity_s{}, ALLOW__NO_CHECK); }
// inline decltype(auto) constexpr operator/ (Zero_s, One_s) { return std::pair(Zero_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (One_s, Zero_s) { return std::pair(Infinity_s{}, ALLOW__NO_CHECK); }
// inline decltype(auto) constexpr operator/ (Zero_s, Zero_s) { return std::pair(NaN_s{}, ALLOW__VERIFY_OR_THROW); }
//
// // One_s is multiplicative identity.
// inline decltype(auto) constexpr operator* (One_s, ExtReal_s) { return std::pair(ExtReal_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (ExtReal_s, One_s) { return std::pair(ExtReal_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (One_s, NonNeg_s) { return std::pair(NonNeg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (NonNeg_s, One_s) { return std::pair(NonNeg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (One_s, NonPos_s) { return std::pair(NonPos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (NonPos_s, One_s) { return std::pair(NonPos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (One_s, Neg_s) { return std::pair(Neg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (Neg_s, One_s) { return std::pair(Neg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (One_s, Pos_s) { return std::pair(Pos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (Pos_s, One_s) { return std::pair(Pos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (One_s, NegOne_s) { return std::pair(NegOne_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (NegOne_s, One_s) { return std::pair(NegOne_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator* (One_s, One_s) { return std::pair(One_s{}, ALLOW__VERIFY_OR_THROW); }
//
// // One_s also has a simple action with respect to division.
// inline decltype(auto) constexpr operator/ (One_s, ExtReal_s) { return std::pair(ExtReal_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (ExtReal_s, One_s) { return std::pair(ExtReal_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (One_s, NonNeg_s) { return std::pair(NonNeg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (NonNeg_s, One_s) { return std::pair(NonNeg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (One_s, NonPos_s) { return std::pair(NonPos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (NonPos_s, One_s) { return std::pair(NonPos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (One_s, Neg_s) { return std::pair(Neg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (Neg_s, One_s) { return std::pair(Neg_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (One_s, Pos_s) { return std::pair(Pos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (Pos_s, One_s) { return std::pair(Pos_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (One_s, NegOne_s) { return std::pair(NegOne_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (NegOne_s, One_s) { return std::pair(NegOne_s{}, ALLOW__VERIFY_OR_THROW); }
// inline decltype(auto) constexpr operator/ (One_s, One_s) { return std::pair(One_s{}, ALLOW__VERIFY_OR_THROW); }
//
// // MISC -- TODO: complete these
// inline decltype(auto) constexpr operator+ (NonNeg_s, NonNeg_s) { return std::pair(NonNeg_s{}, ALLOW__NO_CHECK); }
// inline decltype(auto) constexpr operator* (NonNeg_s, NonNeg_s) { return std::pair(NonNeg_s{}, ALLOW__NO_CHECK); }
// inline decltype(auto) constexpr operator/ (NonNeg_s, NonNeg_s) { return std::pair(NonNeg_s{}, ALLOW__NO_CHECK); }
//
// inline decltype(auto) constexpr operator+ (NonPos_s, NonPos_s) { return std::pair(NonPos_s{}, ALLOW__NO_CHECK); }
// inline decltype(auto) constexpr operator* (NonPos_s, NonPos_s) { return std::pair(NonNeg_s{}, ALLOW__NO_CHECK); }
// inline decltype(auto) constexpr operator/ (NonPos_s, NonPos_s) { return std::pair(NonNeg_s{}, ALLOW__VERIFY_OR_THROW); }

// Semantic value type factories.

#define DEFINE_SEMANTIC_VALUE_TYPE(BaseName) template <typename C_> using BaseName##_t = SV_t<BaseName##_s,C_>; using BaseName = BaseName##_t<double>;

DEFINE_SEMANTIC_VALUE_TYPE(NaNExtReal)
DEFINE_SEMANTIC_VALUE_TYPE(NaN)
DEFINE_SEMANTIC_VALUE_TYPE(ExtReal)
DEFINE_SEMANTIC_VALUE_TYPE(Infinite)
DEFINE_SEMANTIC_VALUE_TYPE(ExtNonZero)
DEFINE_SEMANTIC_VALUE_TYPE(ExtNonNeg)
DEFINE_SEMANTIC_VALUE_TYPE(ExtNonPos)
DEFINE_SEMANTIC_VALUE_TYPE(ExtPos)
DEFINE_SEMANTIC_VALUE_TYPE(ExtNeg)
DEFINE_SEMANTIC_VALUE_TYPE(Infinity)
DEFINE_SEMANTIC_VALUE_TYPE(NegInfinity)
DEFINE_SEMANTIC_VALUE_TYPE(Real)
DEFINE_SEMANTIC_VALUE_TYPE(NonZero)
DEFINE_SEMANTIC_VALUE_TYPE(NonNeg)
DEFINE_SEMANTIC_VALUE_TYPE(NonPos)
DEFINE_SEMANTIC_VALUE_TYPE(Pos)
DEFINE_SEMANTIC_VALUE_TYPE(Neg)
DEFINE_SEMANTIC_VALUE_TYPE(Zero)
DEFINE_SEMANTIC_VALUE_TYPE(One)
DEFINE_SEMANTIC_VALUE_TYPE(NegOne)

LVD_TEST_BEGIN(007__semantic_subtype__01)
    auto z1 = Zero{0};

    test_log << Log::dbg() << LVD_REFLECT(z1) << '\n';

    auto nn1 = NonNeg{0};
    auto nn2 = NonNeg{nn1};
    auto nn3 = NonNeg{1};
    auto nn4 = NonNeg{z1}; // Can construct from a sub-semantic-type.

    test_log << Log::dbg() << LVD_REFLECT(nn1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(nn2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(nn3) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(nn4) << '\n';
LVD_TEST_END

} // end namespace lvd

LVD_DEFINE_TYPE_STRING(lvd::NaNExtReal_s);
LVD_DEFINE_TYPE_STRING(lvd::NaN_s);
LVD_DEFINE_TYPE_STRING(lvd::ExtReal_s);
LVD_DEFINE_TYPE_STRING(lvd::Infinite_s);
LVD_DEFINE_TYPE_STRING(lvd::ExtNonZero_s);
LVD_DEFINE_TYPE_STRING(lvd::ExtNonNeg_s);
LVD_DEFINE_TYPE_STRING(lvd::ExtNonPos_s);
LVD_DEFINE_TYPE_STRING(lvd::ExtPos_s);
LVD_DEFINE_TYPE_STRING(lvd::ExtNeg_s);
LVD_DEFINE_TYPE_STRING(lvd::Infinity_s);
LVD_DEFINE_TYPE_STRING(lvd::NegInfinity_s);
LVD_DEFINE_TYPE_STRING(lvd::Real_s);
LVD_DEFINE_TYPE_STRING(lvd::NonZero_s);
LVD_DEFINE_TYPE_STRING(lvd::NonNeg_s);
LVD_DEFINE_TYPE_STRING(lvd::NonPos_s);
LVD_DEFINE_TYPE_STRING(lvd::Pos_s);
LVD_DEFINE_TYPE_STRING(lvd::Neg_s);
LVD_DEFINE_TYPE_STRING(lvd::Zero_s);
LVD_DEFINE_TYPE_STRING(lvd::One_s);
LVD_DEFINE_TYPE_STRING(lvd::NegOne_s);

namespace lvd {

LVD_TEST_BEGIN(007__semantic_subtype__03)
    auto p1 = Pos{0.5};
    auto p2 = Pos{0.75};

    test_log << Log::dbg() << LVD_REFLECT(p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 + p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 - p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 * p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 / p2) << '\n';

    auto n1 = Neg{-0.6};
    auto n2 = Neg{-0.55};

    test_log << Log::dbg() << LVD_REFLECT(n1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 + n2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 - n2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 * n2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 / n2) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p1 + n1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 - n1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 * n1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 / n1) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(n1 + p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 - p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 * p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 / p1) << '\n';

    auto z = Zero{0.0};

    test_log << Log::dbg() << LVD_REFLECT(z) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(z * p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z * p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z * n1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z * p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z * z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z / p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z / p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z / n1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z / p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(z / z) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p1 * z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p2 * z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 * z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n2 * z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p1 / z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p2 / z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n1 / z) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n2 / z) << '\n';

    auto mx = std::numeric_limits<double>::max();
    test_log << Log::dbg() << LVD_REFLECT(mx) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(mx+mx) << '\n';

    auto mn = std::numeric_limits<double>::min();
    auto subnormal = std::numeric_limits<double>::denorm_min();
    test_log << Log::dbg() << "mn = " << std::scientific << mn << '\n';
    test_log << Log::dbg() << LVD_REFLECT(std::isnormal(mn)) << '\n';
    test_log << Log::dbg() << "mn*mn = " << std::scientific << mn*mn << '\n';
    test_log << Log::dbg() << "`mn*mn == 0` = " << std::boolalpha << (mn*mn == 0) << '\n';
    test_log << Log::dbg() << "subnormal = " << std::scientific << subnormal << '\n';
    test_log << Log::dbg() << LVD_REFLECT(std::isnormal(subnormal)) << '\n';
    test_log << Log::dbg() << "subnormal*subnormal = " << std::scientific << subnormal*subnormal << '\n';
    test_log << Log::dbg() << "subnormal/subnormal = " << std::scientific << subnormal/subnormal << '\n';
    test_log << Log::dbg() << "1.0/subnormal = " << std::scientific << 1.0/subnormal << '\n';

    test_log << Log::dbg() << "mx/subnormal = " << std::scientific << mx/subnormal << '\n';
LVD_TEST_END

template <typename Lhs_, typename Rhs_>
void test_operators (req::Context &req_context, Lhs_ const &lhs, Rhs_ const &rhs) {
    auto &test_log = req_context.log();
    test_log << Log::dbg() << "testing " << lhs << " and " << rhs << '\n';
    IndentGuard ig(test_log);
    test_log << Log::trc() << lhs << " + " << rhs << " = " << (lhs + rhs) << '\n';
    test_log << Log::trc() << lhs << " - " << rhs << " = " << (lhs - rhs) << '\n';
    test_log << Log::trc() << lhs << " * " << rhs << " = " << (lhs * rhs) << '\n';
    test_log << Log::trc() << lhs << " / " << rhs << " = " << (lhs / rhs) << '\n';

    // Do explicit checks on each.
    (lhs + rhs).template check<ALLOW__VERIFY_OR_THROW>();
    (lhs - rhs).template check<ALLOW__VERIFY_OR_THROW>();
    (lhs * rhs).template check<ALLOW__VERIFY_OR_THROW>();
    (lhs / rhs).template check<ALLOW__VERIFY_OR_THROW>();
}

template <size_t LHS_INDEX_, typename... LhsArgs_, typename Rhs_>
void test_operators_lhs (req::Context &req_context, IndexedTuple_t<LHS_INDEX_,LhsArgs_...> const &lhs, Rhs_ const &rhs) {
    if constexpr (lhs.has_ended()) {
        // Base case; nothing to do.
    } else {
        // Inductive case.
        test_operators(req_context, lhs.current(), rhs);
        test_operators_lhs(req_context, lhs.incremented(), rhs);
    }
}

template <size_t LHS_INDEX_, typename... LhsArgs_, size_t RHS_INDEX_, typename... RhsArgs_>
void test_operators_lhs_rhs (req::Context &req_context, IndexedTuple_t<LHS_INDEX_,LhsArgs_...> const &lhs, IndexedTuple_t<RHS_INDEX_,RhsArgs_...> const &rhs) {
    if constexpr (rhs.has_ended()) {
        // Base case; nothing to do.
    } else {
        // Inductive case.
        test_operators_lhs(req_context, lhs, rhs.current());
        test_operators_lhs_rhs(req_context, lhs, rhs.incremented());
    }
}

LVD_TEST_BEGIN(007__semantic_subtype__04)
    auto ner0a = NaNExtReal{0};
    auto ner0b = NaNExtReal{-0};
    auto ner1a = NaNExtReal{std::numeric_limits<double>::denorm_min()};
    auto ner1b = NaNExtReal{-std::numeric_limits<double>::denorm_min()};
    auto ner2a = NaNExtReal{std::numeric_limits<double>::min()};
    auto ner2b = NaNExtReal{-std::numeric_limits<double>::min()};
    auto ner3a = NaNExtReal{1};
    auto ner3b = NaNExtReal{-1};
    auto ner4a = NaNExtReal{0.5};
    auto ner4b = NaNExtReal{-0.5};
    auto ner5a = NaNExtReal{2};
    auto ner5b = NaNExtReal{-2};
    auto ner6a = NaNExtReal{std::numeric_limits<double>::max()};
    auto ner6b = NaNExtReal{-std::numeric_limits<double>::max()};
    auto ner7a = NaNExtReal{std::numeric_limits<double>::infinity()};
    auto ner7b = NaNExtReal{-std::numeric_limits<double>::infinity()};
    auto ner8a = NaNExtReal{std::numeric_limits<double>::quiet_NaN()};
    auto ner8b = NaNExtReal{std::numeric_limits<double>::signaling_NaN()};

    auto nan8a = NaNExtReal{std::numeric_limits<double>::quiet_NaN()};
    auto nan8b = NaNExtReal{std::numeric_limits<double>::signaling_NaN()};

    auto er0a = ExtReal{0};
    auto er0b = ExtReal{-0};
    auto er1a = ExtReal{std::numeric_limits<double>::denorm_min()};
    auto er1b = ExtReal{-std::numeric_limits<double>::denorm_min()};
    auto er2a = ExtReal{std::numeric_limits<double>::min()};
    auto er2b = ExtReal{-std::numeric_limits<double>::min()};
    auto er3a = ExtReal{1};
    auto er3b = ExtReal{-1};
    auto er4a = ExtReal{0.5};
    auto er4b = ExtReal{-0.5};
    auto er5a = ExtReal{2};
    auto er5b = ExtReal{-2};
    auto er6a = ExtReal{std::numeric_limits<double>::max()};
    auto er6b = ExtReal{-std::numeric_limits<double>::max()};
    auto er7a = ExtReal{std::numeric_limits<double>::infinity()};
    auto er7b = ExtReal{-std::numeric_limits<double>::infinity()};

    auto infinite7a = NaNExtReal{std::numeric_limits<double>::infinity()};
    auto infinite7b = NaNExtReal{-std::numeric_limits<double>::infinity()};

    auto enz1a = ExtNonZero{std::numeric_limits<double>::denorm_min()};
    auto enz1b = ExtNonZero{-std::numeric_limits<double>::denorm_min()};
    auto enz2a = ExtNonZero{std::numeric_limits<double>::min()};
    auto enz2b = ExtNonZero{-std::numeric_limits<double>::min()};
    auto enz3a = ExtNonZero{1};
    auto enz3b = ExtNonZero{-1};
    auto enz4a = ExtNonZero{0.5};
    auto enz4b = ExtNonZero{-0.5};
    auto enz5a = ExtNonZero{2};
    auto enz5b = ExtNonZero{-2};
    auto enz6a = ExtNonZero{std::numeric_limits<double>::max()};
    auto enz6b = ExtNonZero{-std::numeric_limits<double>::max()};
    auto enz7a = ExtNonZero{std::numeric_limits<double>::infinity()};
    auto enz7b = ExtNonZero{-std::numeric_limits<double>::infinity()};

    auto enn0a = ExtNonNeg{0};
    auto enn1a = ExtNonNeg{std::numeric_limits<double>::denorm_min()};
    auto enn2a = ExtNonNeg{std::numeric_limits<double>::min()};
    auto enn3a = ExtNonNeg{1};
    auto enn4a = ExtNonNeg{0.5};
    auto enn5a = ExtNonNeg{2};
    auto enn6a = ExtNonNeg{std::numeric_limits<double>::max()};
    auto enn7a = ExtNonNeg{std::numeric_limits<double>::infinity()};

    auto enp0b = ExtNonPos{-0};
    auto enp1b = ExtNonPos{-std::numeric_limits<double>::denorm_min()};
    auto enp2b = ExtNonPos{-std::numeric_limits<double>::min()};
    auto enp3b = ExtNonPos{-1};
    auto enp4b = ExtNonPos{-0.5};
    auto enp5b = ExtNonPos{-2};
    auto enp6b = ExtNonPos{-std::numeric_limits<double>::max()};
    auto enp7b = ExtNonPos{-std::numeric_limits<double>::infinity()};

    auto ep1a = ExtPos{std::numeric_limits<double>::denorm_min()};
    auto ep2a = ExtPos{std::numeric_limits<double>::min()};
    auto ep3a = ExtPos{1};
    auto ep4a = ExtPos{0.5};
    auto ep5a = ExtPos{2};
    auto ep6a = ExtPos{std::numeric_limits<double>::max()};
    auto ep7a = ExtPos{std::numeric_limits<double>::infinity()};

    auto en1b = ExtNeg{-std::numeric_limits<double>::denorm_min()};
    auto en2b = ExtNeg{-std::numeric_limits<double>::min()};
    auto en3b = ExtNeg{-1};
    auto en4b = ExtNeg{-0.5};
    auto en5b = ExtNeg{-2};
    auto en6b = ExtNeg{-std::numeric_limits<double>::max()};
    auto en7b = ExtNeg{-std::numeric_limits<double>::infinity()};

    auto z0a = Zero{0};
    auto z0b = Zero{-0};

    auto all = std::tuple(
        ner0a, ner0b, ner1a, ner1b, ner2a, ner2b, ner3a, ner3b, ner4a, ner4b, ner5a, ner5b, ner6a, ner6b, ner7a, ner7b, ner8a, ner8b,
        nan8a, nan8b,
        er0a, er0b, er1a, er1b, er2a, er2b, er3a, er3b, er4a, er4b, er5a, er5b, er6a, er6b, er7a, er7b,
        infinite7a, infinite7b,
        enz1a, enz1b, enz2a, enz2b, enz3a, enz3b, enz4a, enz4b, enz5a, enz5b, enz6a, enz6b, enz7a, enz7b,
        enn0a, enn1a, enn2a, enn3a, enn4a, enn5a, enn6a, enn7a,
        enp0b, enp1b, enp2b, enp3b, enp4b, enp5b, enp6b, enp7b,
        ep1a, ep2a, ep3a, ep4a, ep5a, ep6a, ep7a,
        en1b, en2b, en3b, en4b, en5b, en6b, en7b,
        z0a, z0b
    );

    std::ignore = all;

    test_operators_lhs_rhs(req_context, begin_indexed_tuple(all), begin_indexed_tuple(all));
LVD_TEST_END

} // end namespace lvd
