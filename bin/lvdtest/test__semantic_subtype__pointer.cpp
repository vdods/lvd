// 2021.02.03 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/req.hpp"
#include "lvd/semantic_subtype.hpp"
#include "lvd/test.hpp"
#include "lvd/type.hpp"
#include "lvd/type_string_of.hpp"

namespace lvd {

struct NonNull_s : public Base_s {
    using Super_s = Base_s;

    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"NonNull"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return cv != nullptr;
    }
};

inline decltype(auto) constexpr check_policy_for__ctor_default (NonNull_s) { return value_v<CheckPolicy,PROHIBIT>; }

template <typename SV_, typename C_, typename T_>
inline decltype(auto) constexpr check_policy_for__ctor_copy_T (NonNull_s) {
    if constexpr (std::is_same_v<std::decay_t<T_>,nullptr_t>)
        return value_v<CheckPolicy,PROHIBIT>;
    else
        return check_policy_for__ctor_copy_T<SV_,C_,T_>(NonNull_s::Super_s{});
}

template <typename SV_, typename C_, typename T_>
inline decltype(auto) constexpr check_policy_for__ctor_move_T (NonNull_s) {
    if constexpr (std::is_same_v<std::decay_t<T_>,nullptr_t>)
        return value_v<CheckPolicy,PROHIBIT>;
    else
        return check_policy_for__ctor_move_T<SV_,C_,T_>(NonNull_s::Super_s{});
}

template <typename SV_, typename C_, typename First_, typename... Rest_>
inline decltype(auto) constexpr check_policy_for__ctor_variadic (NonNull_s) {
    if constexpr (sizeof...(Rest_) == 0 && std::is_same_v<std::decay_t<First_>,nullptr_t>)
        return value_v<CheckPolicy,PROHIBIT>;
    else
        return check_policy_for__ctor_variadic<SV_,C_,First_,Rest_...>(NonNull_s::Super_s{});
}

template <typename C_>
bool constexpr operator == (SV_t<NonNull_s,C_> const &, nullptr_t) {
    return false;
}
template <typename C_>
bool constexpr operator != (SV_t<NonNull_s,C_> const &, nullptr_t) {
    return true;
}

// Semantic value type factories.
template <typename C_> using NonNull_t = SV_t<NonNull_s,C_>;

// NonNull_s-specific frontends to make_sv and make_sv__with_collapse
template <typename C_> decltype(auto) make_non_null (C_ &&cv) { return make_sv<NonNull_s>(std::forward<C_>(cv)); }
template <typename C_> decltype(auto) make_non_null__with_collapse (C_ &&cv) { return make_sv__with_collapse<NonNull_s>(std::forward<C_>(cv)); }

struct FancyThing {
    int x;
    float y;

    bool operator== (FancyThing const &other) const { return x == other.x && y == other.y; }
};

std::ostream &operator<< (std::ostream &out, FancyThing const &ft) {
    return out << "FancyThing(x = " << ft.x << ", y = " << ft.y << ')';
}

class PointerLikeThing {
public:

    PointerLikeThing () = delete;
    PointerLikeThing (FancyThing *ptr) : m_ptr(ptr) { }

    bool operator == (PointerLikeThing const &other) const { return m_ptr == other.m_ptr; }
    bool operator == (nullptr_t) const { return m_ptr == nullptr; }

    bool operator != (PointerLikeThing const &other) const { return m_ptr != other.m_ptr; }
    bool operator != (nullptr_t) const { return m_ptr != nullptr; }

    FancyThing &operator* () const { return *m_ptr; }
    FancyThing *operator-> () const { return m_ptr; }

    FancyThing *ptr () const { return m_ptr; }

private:

    FancyThing *m_ptr;
};

inline std::ostream &operator<< (std::ostream &out, PointerLikeThing const &p) {
    return out << "PointerLikeThing(ptr = " << p.ptr() << ')';
}

LVD_TEST_BEGIN(007__semantic_subtype__02__non_null_00)
    // Uncommenting this should cause a compile error "static assertion failed: you tried to use a PROHIBIT'ed method; ..."
//     auto p_null = SV_t<NonNull_s,int*>{nullptr};
//     std::ignore = p_null;

    int a = 3;
    float b = 123.5f;

    auto p1 = make_sv<NonNull_s>(&a);
    auto p2 = make_sv<NonNull_s>(&b);

    test_log << Log::dbg() << LVD_REFLECT(p1.cv()) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*p1.cv()) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p2.cv()) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*p2.cv()) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p1) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*p1) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*p2) << '\n';

    LVD_TEST_REQ_IS_TRUE((is_SV_t<decltype(p1)>));
    LVD_TEST_REQ_IS_TRUE((is_SV_t<decltype(p2)>));
    LVD_TEST_REQ_EQ(p1.cv(), &a);
    LVD_TEST_REQ_EQ(p2.cv(), &b);
    LVD_TEST_REQ_EQ(*p1.cv(), a);
    LVD_TEST_REQ_EQ(*p2.cv(), b);
    test_log << Log::dbg() << "TODO: Need to implement operator== on nonhomogeneous types\n";
//     LVD_TEST_REQ_EQ(p1, &a);
//     LVD_TEST_REQ_EQ(p2, &b);
    LVD_TEST_REQ_EQ(*p1, a);
    LVD_TEST_REQ_EQ(*p2, b);

    auto ft1 = FancyThing{42, 808.25f};
    auto ft2 = FancyThing{111, -4.5f};

    auto p3 = make_non_null(&ft1);
    auto p4 = make_non_null(&ft2);

    test_log << Log::dbg() << LVD_REFLECT(p3.cv()) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*p3.cv()) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p4.cv()) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*p4.cv()) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p3) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*p3) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p3->x) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p3->y) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p4) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*p4) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p4->x) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p4->y) << '\n';

    LVD_TEST_REQ_IS_TRUE((is_SV_t<decltype(p3)>));
    LVD_TEST_REQ_IS_TRUE((is_SV_t<decltype(p4)>));
    LVD_TEST_REQ_EQ(p3.cv(), &ft1);
    LVD_TEST_REQ_EQ(p4.cv(), &ft2);
    LVD_TEST_REQ_EQ(*p3.cv(), ft1);
    LVD_TEST_REQ_EQ(*p4.cv(), ft2);
    test_log << Log::dbg() << "TODO: Need to implement operator== on nonhomogeneous types\n";
//     LVD_TEST_REQ_EQ(p3, &ft1);
//     LVD_TEST_REQ_EQ(p4, &ft2);
    LVD_TEST_REQ_EQ(*p3, ft1);
    LVD_TEST_REQ_EQ(*p4, ft2);
    LVD_TEST_REQ_EQ(p3->x, ft1.x);
    LVD_TEST_REQ_EQ(p3->y, ft1.y);
    LVD_TEST_REQ_EQ(p4->x, ft2.x);
    LVD_TEST_REQ_EQ(p4->y, ft2.y);

    auto plt1 = PointerLikeThing{&ft1};
    auto plt2 = PointerLikeThing{&ft2};

    auto p5 = make_non_null(plt1);
    auto p6 = make_non_null(plt2);

    test_log << Log::dbg() << LVD_REFLECT(p5.cv()) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*p5.cv()) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p6.cv()) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*p6.cv()) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p5) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*p5) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p5->x) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p5->y) << '\n';

    test_log << Log::dbg() << LVD_REFLECT(p6) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*p6) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p6->x) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(p6->y) << '\n';

    LVD_TEST_REQ_IS_TRUE((is_SV_t<decltype(p5)>));
    LVD_TEST_REQ_IS_TRUE((is_SV_t<decltype(p6)>));
    LVD_TEST_REQ_EQ(p5.cv(), plt1);
    LVD_TEST_REQ_EQ(p6.cv(), plt2);
    LVD_TEST_REQ_EQ(*p5.cv(), *plt1);
    LVD_TEST_REQ_EQ(*p6.cv(), *plt2);
    test_log << Log::dbg() << "TODO: Need to implement operator== on nonhomogeneous types\n";
//     LVD_TEST_REQ_EQ(p5, plt1);
//     LVD_TEST_REQ_EQ(p6, plt2);
    LVD_TEST_REQ_EQ(*p5, *plt1);
    LVD_TEST_REQ_EQ(*p6, *plt2);
    LVD_TEST_REQ_EQ(p5->x, plt1->x);
    LVD_TEST_REQ_EQ(p5->y, plt1->y);
    LVD_TEST_REQ_EQ(p6->x, plt2->x);
    LVD_TEST_REQ_EQ(p6->y, plt2->y);

    // This should cause a runtime error (and be caught)
    test::call_function_and_expect_exception<std::runtime_error>([](){
        make_non_null<int*>(nullptr);
    });

    // Try to nest NotNull_t, using NotNull_t<T> as the concrete type.
    auto n1 = make_non_null(make_non_null(&a));
    LVD_TEST_REQ_IS_TRUE((std::is_same_v<decltype(n1),SV_t<NonNull_s,SV_t<NonNull_s,int*>>>));
    LVD_TEST_REQ_EQ(n1.cv().cv(), &a);
    LVD_TEST_REQ_EQ(*n1, a);

    LVD_TEST_REQ_IS_TRUE((is_SV_t<decltype(n1)>));

    // Try the non-nesting make for NotNull_t.
    auto n2 = make_non_null__with_collapse(make_non_null(&a));
    LVD_TEST_REQ_IS_TRUE((std::is_same_v<decltype(n2),SV_t<NonNull_s,int*>>));

    test_log << Log::dbg() << LVD_REFLECT(n2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(*n2) << '\n';
    test_log << Log::dbg() << LVD_REFLECT(n2.cv()) << '\n';

    LVD_TEST_REQ_EQ(n2.cv(), &a);
    LVD_TEST_REQ_EQ(*n2, a);

    LVD_TEST_REQ_IS_TRUE((is_SV_t<decltype(n2)>));
LVD_TEST_END

} // end namespace lvd
