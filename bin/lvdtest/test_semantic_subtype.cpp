// 2021.01.28 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/req.hpp"
#include "lvd/semantic_subtype.hpp"
#include "lvd/test.hpp"
#include "lvd/type.hpp"
#include "lvd/type_string_of.hpp"

using namespace std::string_literals;

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

// NOTE: This sort of thing could go in semantic_subtype.hpp itself.
struct Numeric_s : public Base_s {
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"Numeric"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return true;
    }

    // Cast arithmetic operations back to SV.

    #define DEFINE_NUMERIC_OPERATOR_PROPERTIES(opname, result_policy) \
    inline static ResultPolicy constexpr __##opname##_eq_SV__ = result_policy; \
    template <typename SV_, typename C_, typename T_> static ResultPolicy constexpr __##opname##_eq_T__ () { return result_policy; } \
    inline static ResultPolicy constexpr __##opname##_SV_SV__ = result_policy; \
    template <typename SV_, typename C_, typename T_> static ResultPolicy constexpr __##opname##_SV_T__ () { return result_policy; } \
    template <typename SV_, typename C_, typename T_> static ResultPolicy constexpr __##opname##_T_SV__ () { return result_policy; }

    DEFINE_NUMERIC_OPERATOR_PROPERTIES(add, CAST_TO_SV__NO_CHECK)
    DEFINE_NUMERIC_OPERATOR_PROPERTIES(sub, CAST_TO_SV__NO_CHECK)
    DEFINE_NUMERIC_OPERATOR_PROPERTIES(mul, CAST_TO_SV__NO_CHECK)
    DEFINE_NUMERIC_OPERATOR_PROPERTIES(div, CAST_TO_SV__NO_CHECK)
    DEFINE_NUMERIC_OPERATOR_PROPERTIES(mod, CAST_TO_SV__NO_CHECK)
};

struct NonZero_s : public Numeric_s {
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"NonZero"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return cv != C_(0);
    }
};

struct NonNeg_s : public Numeric_s {
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"NonNeg"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return cv >= C_(0);
    }

    // Override some behavior -- NonNeg is not necessarily closed under some operations.
    DEFINE_NUMERIC_OPERATOR_PROPERTIES(sub, CAST_TO_SV__VERIFY_OR_THROW)
};

struct NonPos_s : public Numeric_s {
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"NonPos"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return cv <= C_(0);
    }

    // Override some behavior -- NonNeg is not necessarily closed under some operations.
    DEFINE_NUMERIC_OPERATOR_PROPERTIES(sub, CAST_TO_SV__VERIFY_OR_THROW)
    // Somehow define mul and div and mod
//     DEFINE_NUMERIC_OPERATOR_PROPERTIES(mul, CAST_TO_SV__VERIFY_OR_THROW)
};

struct Zero_s : public NonNeg_s, public NonPos_s {
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"Zero"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return cv == C_(0);
    }
};

struct Pos_s : public NonNeg_s {
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"Pos"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return cv > C_(0);
    }
};

struct Neg_s : public NonPos_s {
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"Neg"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return cv < C_(0);
    }
};

struct One_s : public Pos_s {
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"One"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return cv == C_(1);
    }
};

struct NegOne_s : public Neg_s {
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"NegOne"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return cv == C_(-1);
    }
};

// Catch-all
inline decltype(auto) operator+ (Numeric_s, Numeric_s) { return Numeric_s{}; }

// Zero_s acts as additive identity.
inline decltype(auto) operator+ (Zero_s, Numeric_s) { return Numeric_s{}; }
inline decltype(auto) operator+ (Numeric_s, Zero_s) { return Numeric_s{}; }
inline decltype(auto) operator+ (Zero_s, NonNeg_s) { return NonNeg_s{}; }
inline decltype(auto) operator+ (NonNeg_s, Zero_s) { return NonNeg_s{}; }
inline decltype(auto) operator+ (Zero_s, NonPos_s) { return NonPos_s{}; }
inline decltype(auto) operator+ (NonPos_s, Zero_s) { return NonPos_s{}; }
inline decltype(auto) operator+ (Zero_s, Neg_s) { return Neg_s{}; }
inline decltype(auto) operator+ (Neg_s, Zero_s) { return Neg_s{}; }
inline decltype(auto) operator+ (Zero_s, Pos_s) { return Pos_s{}; }
inline decltype(auto) operator+ (Pos_s, Zero_s) { return Pos_s{}; }
inline decltype(auto) operator+ (Zero_s, NegOne_s) { return NegOne_s{}; }
inline decltype(auto) operator+ (NegOne_s, Zero_s) { return NegOne_s{}; }
inline decltype(auto) operator+ (Zero_s, One_s) { return One_s{}; }
inline decltype(auto) operator+ (One_s, Zero_s) { return One_s{}; }
inline decltype(auto) operator+ (Zero_s, Zero_s) { return Zero_s{}; }

// Zero_s also has simple action with respect to subtraction
inline decltype(auto) operator- (Zero_s, Numeric_s) { return Numeric_s{}; }
inline decltype(auto) operator- (Numeric_s, Zero_s) { return Numeric_s{}; }
inline decltype(auto) operator- (Zero_s, NonNeg_s) { return NonPos_s{}; }
inline decltype(auto) operator- (NonNeg_s, Zero_s) { return NonNeg_s{}; }
inline decltype(auto) operator- (Zero_s, NonPos_s) { return NonNeg_s{}; }
inline decltype(auto) operator- (NonPos_s, Zero_s) { return NonPos_s{}; }
inline decltype(auto) operator- (Zero_s, Neg_s) { return Pos_s{}; }
inline decltype(auto) operator- (Neg_s, Zero_s) { return Neg_s{}; }
inline decltype(auto) operator- (Zero_s, Pos_s) { return Neg_s{}; }
inline decltype(auto) operator- (Pos_s, Zero_s) { return Pos_s{}; }
inline decltype(auto) operator- (Zero_s, NegOne_s) { return One_s{}; }
inline decltype(auto) operator- (NegOne_s, Zero_s) { return NegOne_s{}; }
inline decltype(auto) operator- (Zero_s, One_s) { return NegOne_s{}; }
inline decltype(auto) operator- (One_s, Zero_s) { return One_s{}; }
inline decltype(auto) operator- (Zero_s, Zero_s) { return Zero_s{}; }

// One_s is multiplicative identity.
inline decltype(auto) operator* (One_s, Numeric_s) { return Numeric_s{}; }
inline decltype(auto) operator* (Numeric_s, One_s) { return Numeric_s{}; }
inline decltype(auto) operator* (One_s, NonNeg_s) { return NonNeg_s{}; }
inline decltype(auto) operator* (NonNeg_s, One_s) { return NonNeg_s{}; }
inline decltype(auto) operator* (One_s, NonPos_s) { return NonPos_s{}; }
inline decltype(auto) operator* (NonPos_s, One_s) { return NonPos_s{}; }
inline decltype(auto) operator* (One_s, Neg_s) { return Neg_s{}; }
inline decltype(auto) operator* (Neg_s, One_s) { return Neg_s{}; }
inline decltype(auto) operator* (One_s, Pos_s) { return Pos_s{}; }
inline decltype(auto) operator* (Pos_s, One_s) { return Pos_s{}; }
inline decltype(auto) operator* (One_s, NegOne_s) { return NegOne_s{}; }
inline decltype(auto) operator* (NegOne_s, One_s) { return NegOne_s{}; }
inline decltype(auto) operator* (One_s, One_s) { return One_s{}; }

// One_s also has a simple action with respect to division.
inline decltype(auto) operator/ (One_s, Numeric_s) { return Numeric_s{}; }
inline decltype(auto) operator/ (Numeric_s, One_s) { return Numeric_s{}; }
inline decltype(auto) operator/ (One_s, NonNeg_s) { return NonNeg_s{}; }
inline decltype(auto) operator/ (NonNeg_s, One_s) { return NonNeg_s{}; }
inline decltype(auto) operator/ (One_s, NonPos_s) { return NonPos_s{}; }
inline decltype(auto) operator/ (NonPos_s, One_s) { return NonPos_s{}; }
inline decltype(auto) operator/ (One_s, Neg_s) { return Neg_s{}; }
inline decltype(auto) operator/ (Neg_s, One_s) { return Neg_s{}; }
inline decltype(auto) operator/ (One_s, Pos_s) { return Pos_s{}; }
inline decltype(auto) operator/ (Pos_s, One_s) { return Pos_s{}; }
inline decltype(auto) operator/ (One_s, NegOne_s) { return NegOne_s{}; }
inline decltype(auto) operator/ (NegOne_s, One_s) { return NegOne_s{}; }
inline decltype(auto) operator/ (One_s, One_s) { return One_s{}; }

// TODO: operator%

// Semantic value type factories.
template <typename C_> using Zero_t = SV_t<Zero_s,C_>;
template <typename C_> using NonNeg_t = SV_t<NonNeg_s,C_>;
template <typename C_> using Pos_t = SV_t<Pos_s,C_>;
template <typename C_> using NonPos_t = SV_t<NonPos_s,C_>;
template <typename C_> using Neg_t = SV_t<Neg_s,C_>;

using Zero = Zero_t<double>;
using NonNeg = NonNeg_t<double>;
using Pos = Pos_t<double>;
using NonPos = NonPos_t<double>;
using Neg = Neg_t<double>;

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

LVD_DEFINE_TYPE_STRING(lvd::Numeric_s);
LVD_DEFINE_TYPE_STRING(lvd::Zero_s);
LVD_DEFINE_TYPE_STRING(lvd::NonNeg_s);
LVD_DEFINE_TYPE_STRING(lvd::NonPos_s);
LVD_DEFINE_TYPE_STRING(lvd::Neg_s);
LVD_DEFINE_TYPE_STRING(lvd::Pos_s);
LVD_DEFINE_TYPE_STRING(lvd::NegOne_s);
LVD_DEFINE_TYPE_STRING(lvd::One_s);

namespace lvd {

LVD_TEST_BEGIN(007__semantic_subtype__02)
    auto num = Numeric_s{};
    auto zero = Zero_s{};
    auto nonneg = NonNeg_s{};
    auto nonpos = NonPos_s{};
    auto neg = Neg_s{};
    auto pos = Pos_s{};
    auto one = One_s{};
    auto neg_one = NegOne_s{};

    LVD_TEST_REQ_EQ(type_of(num + num), type_of(num));

    LVD_TEST_REQ_EQ(type_of(zero + num), type_of(num));
    LVD_TEST_REQ_EQ(type_of(num + zero), type_of(num));
    LVD_TEST_REQ_EQ(type_of(zero + nonneg), type_of(nonneg));
    LVD_TEST_REQ_EQ(type_of(nonneg + zero), type_of(nonneg));
    LVD_TEST_REQ_EQ(type_of(zero + nonpos), type_of(nonpos));
    LVD_TEST_REQ_EQ(type_of(nonpos + zero), type_of(nonpos));
    LVD_TEST_REQ_EQ(type_of(zero + neg), type_of(neg));
    LVD_TEST_REQ_EQ(type_of(neg + zero), type_of(neg));
    LVD_TEST_REQ_EQ(type_of(zero + pos), type_of(pos));
    LVD_TEST_REQ_EQ(type_of(pos + zero), type_of(pos));
    LVD_TEST_REQ_EQ(type_of(zero + neg_one), type_of(neg_one));
    LVD_TEST_REQ_EQ(type_of(neg_one + zero), type_of(neg_one));
    LVD_TEST_REQ_EQ(type_of(zero + one), type_of(one));
    LVD_TEST_REQ_EQ(type_of(one + zero), type_of(one));
    LVD_TEST_REQ_EQ(type_of(zero + zero), type_of(zero));
LVD_TEST_END

} // end namespace lvd
