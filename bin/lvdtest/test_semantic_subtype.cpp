// 2021.01.28 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/req.hpp"
#include "lvd/semantic_subtype.hpp"
#include "lvd/test.hpp"

using namespace std::string_literals;

namespace lvd {

struct NonEmpty_s : public SemanticClassDefaultsBase {
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

using NonEmptyString = NonEmpty_t<std::string>;

NonEmptyString make_nonempty_string () {
    return NonEmptyString("OSTRICH");
}

LVD_TEST_BEGIN(007__semantic_subtype__00)
    // Arguably these aliases are not necessary, NonEmpty_t<std::string> is plenty terse.
//     using NonNegDouble = NonNeg_t<double>;
//     using PosDouble = Pos_t<double>;

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

struct NonNeg_s : public SemanticClassDefaultsBase {
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return cv >= C_(0);
    }
};

// Could inherit NonNeg_s
struct Pos_s : public SemanticClassDefaultsBase {
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return cv > C_(0);
    }
};

// Semantic value type factories.
template <typename C_> using NonNeg_t = SV_t<NonNeg_s,C_>;
template <typename C_> using Pos_t = SV_t<Pos_s,C_>;

} // end namespace lvd
