// 2021.01.28 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/req.hpp"
#include "lvd/sst/mutation.hpp"
#include "lvd/sst/SV_t.hpp"
#include "lvd/test.hpp"
#include "lvd/type.hpp"
#include "lvd/type_string_of.hpp"
#include "print.hpp"

using namespace std::string_literals;

namespace lvd {

struct NonEmpty_s : public sst::Base_s {
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"NonEmpty"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return !cv.empty();
    }
};

// Disallow default construction, since that would be an empty string.
inline decltype(auto) constexpr check_policy_for__ctor_default (NonEmpty_s) { return sst::value_v<sst::CheckPolicy,sst::PROHIBIT>; }

// Adding to a nonempty string can't make it empty.
LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(NonEmpty_s, add_eq, sst::ALLOW__NO_CHECK)

// Sum of nonempty string with anything else is nonempty string.
LVD_DEFINE_SEMANTIC_BIN_OP(+, add, NonEmpty_s, NonEmpty_s, NonEmpty_s, sst::ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_RESULT_POLICY_COMMUTATIVE__SV_T(add, NonEmpty_s, sst::CAST_TO_SV__NO_CHECK)

// Semantic value type factories.
template <typename C_> using NonEmpty_t = sst::SV_t<NonEmpty_s,C_>;

// Arguably this alias is not necessary, NonEmpty_t<std::string> is plenty terse.
using NonEmptyString = NonEmpty_t<std::string>;

NonEmptyString make_nonempty_string () {
    return NonEmptyString("OSTRICH");
}

LVD_TEST_BEGIN(007__sst__00__string__00)
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

template <typename T_>
using NonEmptyVector_t = NonEmpty_t<std::vector<T_>>;

// Also test containers
LVD_TEST_BEGIN(007__sst__00__string__01__vector__00)

    // Uncommenting this should cause a compile error "static assertion failed: you tried to use a PROHIBIT'ed method; ..."
//     auto v1 = NonEmptyVector_t<int>{};
//     std::ignore = v1;

    auto src_vec_0 = std::vector<int>{{4,5,6}};
    auto src_vec_1 = std::vector<int>{{4,5,6,7}};

    auto v2 = NonEmptyVector_t<int>{{4,5,6}};
    test_log << Log::dbg() << LVD_REFLECT(v2.cv()) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(v2) << '\n';

    LVD_TEST_REQ_EQ(v2.cv(), src_vec_0);

    {
        test_log << Log::dbg() << "begin mutating v2...\n";
        auto m2 = sst::mutation(v2);
        // v2 should be considered inaccessible until m2 is destroyed.

        test_log << Log::dbg() << LVD_REFLECT(m2.cv()) << '\n';

        LVD_TEST_REQ_EQ(m2.cv(), src_vec_0);

        // Mutate in place
        m2.cv().push_back(7);
        LVD_TEST_REQ_EQ(m2.cv(), src_vec_1);

        // When m2 is destroyed, it will execute the appropriate assign-move check.
        test_log << Log::dbg() << "done mutating v2...\n";
    }
    test_log << Log::dbg() << LVD_REFLECT(v2.cv()) << '\n';
    LVD_TEST_REQ_IS_FALSE(v2.cv().empty());
    LVD_TEST_REQ_EQ(v2.cv(), src_vec_1);
LVD_TEST_END

LVD_TEST_BEGIN(007__sst__00__string__01__vector__1)
    auto src_vec_0 = std::vector<int>{{4,5,6}};
    auto src_vec_1 = std::vector<int>{{4,5,6,7}};

    auto v3 = NonEmptyVector_t<int>{{4,5,6}};
    test_log << Log::dbg() << LVD_REFLECT(v3.cv()) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(v3) << '\n';

    // Manually mutating involves a copy and then an assign-move.

    test_log << Log::dbg() << "begin mutating v3...\n";
    std::vector<int> c3 = v3.cv();

    test_log << Log::dbg() << LVD_REFLECT(c3) << '\n';

    LVD_TEST_REQ_EQ(c3, src_vec_0);

    // Mutate in place in such a way that violates the validity, so that the check
    // fails when this code block ends and the Mutation_t is destroyed.
    c3.push_back(7);
    LVD_TEST_REQ_EQ(c3, src_vec_1);
    LVD_TEST_REQ_IS_TRUE(NonEmpty_s::is_valid(c3));

    test_log << Log::dbg() << LVD_REFLECT(c3) << '\n';

    test_log << Log::dbg() << "done mutating v3...\n";
    v3 = std::move(c3);
    test_log << Log::dbg() << LVD_REFLECT(v3) << '\n';
LVD_TEST_END

LVD_TEST_BEGIN(007__sst__00__string__01__vector__2)
    auto src_vec_0 = std::vector<int>{{4,5,6}};
    auto src_vec_1 = std::vector<int>{{4,5,6,7}};

    auto v3 = NonEmptyVector_t<int>{{4,5,6}};
    test_log << Log::dbg() << LVD_REFLECT(v3.cv()) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(v3) << '\n';

    // Manually mutating involves a copy and then an assign-move.
    test::call_function_and_expect_exception<std::runtime_error>([&](){
        test_log << Log::dbg() << "begin mutating v3... badly...\n";
        std::vector<int> c3 = v3.cv();

        test_log << Log::dbg() << LVD_REFLECT(c3) << '\n';

        LVD_TEST_REQ_EQ(c3, src_vec_0);

        // Mutate in place in such a way that violates the validity, so that the check
        // fails when this code block ends and the Mutation_t is destroyed.
        c3.clear();
        LVD_TEST_REQ_IS_TRUE(c3.empty());
        LVD_TEST_REQ_IS_FALSE(NonEmpty_s::is_valid(c3));

        test_log << Log::dbg() << LVD_REFLECT(c3) << '\n';

        test_log << Log::dbg() << "done mutating v3... badly...\n";
        v3 = std::move(c3);
    });
LVD_TEST_END

} // end namespace lvd
