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

    // Adding to a nonempty string can't make it empty.
    inline static ResultPolicy constexpr __add_eq_SV__ = CAST_TO_SV__NO_CHECK;
    template <typename SV_, typename C_, typename T_> static ResultPolicy constexpr __add_eq_T__ () { return CAST_TO_SV__NO_CHECK; }
    // Sum of nonempty string with anything else is nonempty string.
    inline static ResultPolicy constexpr __add_SV_SV__ = CAST_TO_SV__NO_CHECK;
    template <typename SV_, typename C_, typename T_> static ResultPolicy constexpr __add_SV_T__ () { return CAST_TO_SV__NO_CHECK; }
    template <typename SV_, typename C_, typename T_> static ResultPolicy constexpr __add_T_SV__ () { return CAST_TO_SV__NO_CHECK; }
};

// Disallow default construction, since that would be an empty string.
inline decltype(auto) constexpr check_policy_for__ctor_default (NonEmpty_s) { return value_v<CheckPolicy,PROHIBIT>; }

// LVD_DEFINE_SEMANTIC_BIN_OP(==, eq, NonEmpty_s, NonEmpty_s, TODO start here.. does this make sense?, ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP(+, add, NonEmpty_s, NonEmpty_s, NonEmpty_s, ALLOW__NO_CHECK)

// Semantic value type factories.
template <typename C_> using NonEmpty_t = SV_t<NonEmpty_s,C_>;

// Arguably this alias is not necessary, NonEmpty_t<std::string> is plenty terse.
using NonEmptyString = NonEmpty_t<std::string>;

NonEmptyString make_nonempty_string () {
    return NonEmptyString("OSTRICH");
}

LVD_TEST_BEGIN(007__semantic_subtype__00__string__00)
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
//     std::ignore = bad;

    test::call_function_and_expect_exception<std::runtime_error>([](){
        NonEmptyString{""};
    });

    LVD_TEST_REQ_EQ(NonEmptyString{"blah"}, NonEmptyString{"blah"});
    LVD_TEST_REQ_NEQ(NonEmptyString{"blah"}, NonEmptyString{"thing"});

    auto s12 = NonEmptyString{"abc"};

    LVD_TEST_REQ_IS_TRUE((std::is_same_v<decltype(s12[0]),char>));

    LVD_TEST_REQ_EQ(s12[0], 'a');
    LVD_TEST_REQ_EQ(s12[1], 'b');
    LVD_TEST_REQ_EQ(s12[2], 'c');
LVD_TEST_END

} // end namespace lvd
