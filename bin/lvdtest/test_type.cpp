// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/fmt.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include "lvd/type.hpp"
#include "lvd/type_string_of.hpp"

namespace lvd {

LVD_TEST_BEGIN(005__Type_t__00)
    auto &i = ty<int>;
    auto &f = ty<float>;

    LVD_TEST_REQ_IS_TRUE(i == i);
    LVD_TEST_REQ_IS_FALSE(i == f);
    LVD_TEST_REQ_IS_FALSE(i != i);
    LVD_TEST_REQ_IS_TRUE(i != f);

    LVD_TEST_REQ_EQ(i, i);
    LVD_TEST_REQ_NEQ(i, f);
    LVD_TEST_REQ_NEQ(f, i);
    LVD_TEST_REQ_EQ(f, f);
LVD_TEST_END

LVD_TEST_BEGIN(005__Type_t__01)
    LVD_TEST_REQ_EQ(ty<bool>(123), bool(123));
    LVD_TEST_REQ_EQ(ty<int>(123), int(123));
    LVD_TEST_REQ_EQ(ty<double>(123), double(123));
LVD_TEST_END

LVD_TEST_BEGIN(005__Type_t__02)
    LVD_TEST_REQ_EQ(LVD_FMT(ty<bool>), "bool");
    LVD_TEST_REQ_EQ(LVD_FMT(ty<int32_t>), "int32_t");
LVD_TEST_END

class A { };
class B : public A { };
class C : public A { };
class D : public B, public C { };

} // end namespace lvd

LVD_DEFINE_TYPE_STRING_EXPLICIT(lvd::A, "A")
LVD_DEFINE_TYPE_STRING_EXPLICIT(lvd::B, "B")
LVD_DEFINE_TYPE_STRING_EXPLICIT(lvd::C, "C")
LVD_DEFINE_TYPE_STRING_EXPLICIT(lvd::D, "D")

namespace lvd {

LVD_TEST_BEGIN(005__Type_t__03)
    auto &a = ty<A>;
    auto &b = ty<B>;
    auto &c = ty<C>;
    auto &d = ty<D>;
    auto &i = ty<int>;

    LVD_TEST_REQ_EQ(a, a);
    LVD_TEST_REQ_NEQ(a, b);
    LVD_TEST_REQ_NEQ(a, c);
    LVD_TEST_REQ_NEQ(a, d);
    LVD_TEST_REQ_NEQ(a, i);
    LVD_TEST_REQ_EQ(b, b);
    LVD_TEST_REQ_NEQ(b, c);
    LVD_TEST_REQ_NEQ(b, d);
    LVD_TEST_REQ_NEQ(b, i);
    LVD_TEST_REQ_EQ(c, c);
    LVD_TEST_REQ_NEQ(c, d);
    LVD_TEST_REQ_NEQ(c, i);
    LVD_TEST_REQ_EQ(d, d);
    LVD_TEST_REQ_NEQ(d, i);
    LVD_TEST_REQ_EQ(i, i);

    LVD_TEST_REQ_IS_TRUE(a.is_supertype_of(a));
    LVD_TEST_REQ_IS_TRUE(a.is_supertype_of(b));
    LVD_TEST_REQ_IS_TRUE(a.is_supertype_of(c));
    LVD_TEST_REQ_IS_TRUE(a.is_supertype_of(d));
    LVD_TEST_REQ_IS_TRUE(!a.is_supertype_of(i));

    LVD_TEST_REQ_IS_TRUE(!b.is_supertype_of(a));
    LVD_TEST_REQ_IS_TRUE(b.is_supertype_of(b));
    LVD_TEST_REQ_IS_TRUE(!b.is_supertype_of(c));
    LVD_TEST_REQ_IS_TRUE(b.is_supertype_of(d));
    LVD_TEST_REQ_IS_TRUE(!b.is_supertype_of(i));

    LVD_TEST_REQ_IS_TRUE(!c.is_supertype_of(a));
    LVD_TEST_REQ_IS_TRUE(!c.is_supertype_of(b));
    LVD_TEST_REQ_IS_TRUE(c.is_supertype_of(c));
    LVD_TEST_REQ_IS_TRUE(c.is_supertype_of(d));
    LVD_TEST_REQ_IS_TRUE(!c.is_supertype_of(i));

    LVD_TEST_REQ_IS_TRUE(!d.is_supertype_of(a));
    LVD_TEST_REQ_IS_TRUE(!d.is_supertype_of(b));
    LVD_TEST_REQ_IS_TRUE(!d.is_supertype_of(c));
    LVD_TEST_REQ_IS_TRUE(d.is_supertype_of(d));
    LVD_TEST_REQ_IS_TRUE(!d.is_supertype_of(i));

    LVD_TEST_REQ_IS_TRUE(!i.is_supertype_of(a));
    LVD_TEST_REQ_IS_TRUE(!i.is_supertype_of(b));
    LVD_TEST_REQ_IS_TRUE(!i.is_supertype_of(c));
    LVD_TEST_REQ_IS_TRUE(!i.is_supertype_of(d));
    LVD_TEST_REQ_IS_TRUE(i.is_supertype_of(i));


    LVD_TEST_REQ_IS_TRUE(!a.is_strict_supertype_of(a));
    LVD_TEST_REQ_IS_TRUE(a.is_strict_supertype_of(b));
    LVD_TEST_REQ_IS_TRUE(a.is_strict_supertype_of(c));
    LVD_TEST_REQ_IS_TRUE(a.is_strict_supertype_of(d));
    LVD_TEST_REQ_IS_TRUE(!a.is_strict_supertype_of(i));

    LVD_TEST_REQ_IS_TRUE(!b.is_strict_supertype_of(a));
    LVD_TEST_REQ_IS_TRUE(!b.is_strict_supertype_of(b));
    LVD_TEST_REQ_IS_TRUE(!b.is_strict_supertype_of(c));
    LVD_TEST_REQ_IS_TRUE(b.is_strict_supertype_of(d));
    LVD_TEST_REQ_IS_TRUE(!b.is_strict_supertype_of(i));

    LVD_TEST_REQ_IS_TRUE(!c.is_strict_supertype_of(a));
    LVD_TEST_REQ_IS_TRUE(!c.is_strict_supertype_of(b));
    LVD_TEST_REQ_IS_TRUE(!c.is_strict_supertype_of(c));
    LVD_TEST_REQ_IS_TRUE(c.is_strict_supertype_of(d));
    LVD_TEST_REQ_IS_TRUE(!c.is_strict_supertype_of(i));

    LVD_TEST_REQ_IS_TRUE(!d.is_strict_supertype_of(a));
    LVD_TEST_REQ_IS_TRUE(!d.is_strict_supertype_of(b));
    LVD_TEST_REQ_IS_TRUE(!d.is_strict_supertype_of(c));
    LVD_TEST_REQ_IS_TRUE(!d.is_strict_supertype_of(d));
    LVD_TEST_REQ_IS_TRUE(!d.is_strict_supertype_of(i));

    LVD_TEST_REQ_IS_TRUE(!i.is_strict_supertype_of(a));
    LVD_TEST_REQ_IS_TRUE(!i.is_strict_supertype_of(b));
    LVD_TEST_REQ_IS_TRUE(!i.is_strict_supertype_of(c));
    LVD_TEST_REQ_IS_TRUE(!i.is_strict_supertype_of(d));
    LVD_TEST_REQ_IS_TRUE(!i.is_strict_supertype_of(i));


    LVD_TEST_REQ_IS_TRUE(a.is_subtype_of(a));
    LVD_TEST_REQ_IS_TRUE(!a.is_subtype_of(b));
    LVD_TEST_REQ_IS_TRUE(!a.is_subtype_of(c));
    LVD_TEST_REQ_IS_TRUE(!a.is_subtype_of(d));
    LVD_TEST_REQ_IS_TRUE(!a.is_subtype_of(i));

    LVD_TEST_REQ_IS_TRUE(b.is_subtype_of(a));
    LVD_TEST_REQ_IS_TRUE(b.is_subtype_of(b));
    LVD_TEST_REQ_IS_TRUE(!b.is_subtype_of(c));
    LVD_TEST_REQ_IS_TRUE(!b.is_subtype_of(d));
    LVD_TEST_REQ_IS_TRUE(!b.is_subtype_of(i));

    LVD_TEST_REQ_IS_TRUE(c.is_subtype_of(a));
    LVD_TEST_REQ_IS_TRUE(!c.is_subtype_of(b));
    LVD_TEST_REQ_IS_TRUE(c.is_subtype_of(c));
    LVD_TEST_REQ_IS_TRUE(!c.is_subtype_of(d));
    LVD_TEST_REQ_IS_TRUE(!c.is_subtype_of(i));

    LVD_TEST_REQ_IS_TRUE(d.is_subtype_of(a));
    LVD_TEST_REQ_IS_TRUE(d.is_subtype_of(b));
    LVD_TEST_REQ_IS_TRUE(d.is_subtype_of(c));
    LVD_TEST_REQ_IS_TRUE(d.is_subtype_of(d));
    LVD_TEST_REQ_IS_TRUE(!d.is_subtype_of(i));

    LVD_TEST_REQ_IS_TRUE(!i.is_subtype_of(a));
    LVD_TEST_REQ_IS_TRUE(!i.is_subtype_of(b));
    LVD_TEST_REQ_IS_TRUE(!i.is_subtype_of(c));
    LVD_TEST_REQ_IS_TRUE(!i.is_subtype_of(d));
    LVD_TEST_REQ_IS_TRUE(i.is_subtype_of(i));


    LVD_TEST_REQ_IS_TRUE(!a.is_strict_subtype_of(a));
    LVD_TEST_REQ_IS_TRUE(!a.is_strict_subtype_of(b));
    LVD_TEST_REQ_IS_TRUE(!a.is_strict_subtype_of(c));
    LVD_TEST_REQ_IS_TRUE(!a.is_strict_subtype_of(d));
    LVD_TEST_REQ_IS_TRUE(!a.is_strict_subtype_of(i));

    LVD_TEST_REQ_IS_TRUE(b.is_strict_subtype_of(a));
    LVD_TEST_REQ_IS_TRUE(!b.is_strict_subtype_of(b));
    LVD_TEST_REQ_IS_TRUE(!b.is_strict_subtype_of(c));
    LVD_TEST_REQ_IS_TRUE(!b.is_strict_subtype_of(d));
    LVD_TEST_REQ_IS_TRUE(!b.is_strict_subtype_of(i));

    LVD_TEST_REQ_IS_TRUE(c.is_strict_subtype_of(a));
    LVD_TEST_REQ_IS_TRUE(!c.is_strict_subtype_of(b));
    LVD_TEST_REQ_IS_TRUE(!c.is_strict_subtype_of(c));
    LVD_TEST_REQ_IS_TRUE(!c.is_strict_subtype_of(d));
    LVD_TEST_REQ_IS_TRUE(!c.is_strict_subtype_of(i));

    LVD_TEST_REQ_IS_TRUE(d.is_strict_subtype_of(a));
    LVD_TEST_REQ_IS_TRUE(d.is_strict_subtype_of(b));
    LVD_TEST_REQ_IS_TRUE(d.is_strict_subtype_of(c));
    LVD_TEST_REQ_IS_TRUE(!d.is_strict_subtype_of(d));
    LVD_TEST_REQ_IS_TRUE(!d.is_strict_subtype_of(i));

    LVD_TEST_REQ_IS_TRUE(!i.is_strict_subtype_of(a));
    LVD_TEST_REQ_IS_TRUE(!i.is_strict_subtype_of(b));
    LVD_TEST_REQ_IS_TRUE(!i.is_strict_subtype_of(c));
    LVD_TEST_REQ_IS_TRUE(!i.is_strict_subtype_of(d));
    LVD_TEST_REQ_IS_TRUE(!i.is_strict_subtype_of(i));


    LVD_TEST_REQ_IS_TRUE(a.is_related_to(a));
    LVD_TEST_REQ_IS_TRUE(a.is_related_to(b));
    LVD_TEST_REQ_IS_TRUE(a.is_related_to(c));
    LVD_TEST_REQ_IS_TRUE(a.is_related_to(d));
    LVD_TEST_REQ_IS_TRUE(!a.is_related_to(i));

    LVD_TEST_REQ_IS_TRUE(b.is_related_to(a));
    LVD_TEST_REQ_IS_TRUE(b.is_related_to(b));
    LVD_TEST_REQ_IS_TRUE(!b.is_related_to(c));
    LVD_TEST_REQ_IS_TRUE(b.is_related_to(d));
    LVD_TEST_REQ_IS_TRUE(!b.is_related_to(i));

    LVD_TEST_REQ_IS_TRUE(c.is_related_to(a));
    LVD_TEST_REQ_IS_TRUE(!c.is_related_to(b));
    LVD_TEST_REQ_IS_TRUE(c.is_related_to(c));
    LVD_TEST_REQ_IS_TRUE(c.is_related_to(d));
    LVD_TEST_REQ_IS_TRUE(!c.is_related_to(i));

    LVD_TEST_REQ_IS_TRUE(d.is_related_to(a));
    LVD_TEST_REQ_IS_TRUE(d.is_related_to(b));
    LVD_TEST_REQ_IS_TRUE(d.is_related_to(c));
    LVD_TEST_REQ_IS_TRUE(d.is_related_to(d));
    LVD_TEST_REQ_IS_TRUE(!d.is_related_to(i));

    LVD_TEST_REQ_IS_TRUE(!i.is_related_to(a));
    LVD_TEST_REQ_IS_TRUE(!i.is_related_to(b));
    LVD_TEST_REQ_IS_TRUE(!i.is_related_to(c));
    LVD_TEST_REQ_IS_TRUE(!i.is_related_to(d));
    LVD_TEST_REQ_IS_TRUE(i.is_related_to(i));


    LVD_TEST_REQ_IS_TRUE(!a.is_unrelated_to(a));
    LVD_TEST_REQ_IS_TRUE(!a.is_unrelated_to(b));
    LVD_TEST_REQ_IS_TRUE(!a.is_unrelated_to(c));
    LVD_TEST_REQ_IS_TRUE(!a.is_unrelated_to(d));
    LVD_TEST_REQ_IS_TRUE(a.is_unrelated_to(i));

    LVD_TEST_REQ_IS_TRUE(!b.is_unrelated_to(a));
    LVD_TEST_REQ_IS_TRUE(!b.is_unrelated_to(b));
    LVD_TEST_REQ_IS_TRUE(b.is_unrelated_to(c));
    LVD_TEST_REQ_IS_TRUE(!b.is_unrelated_to(d));
    LVD_TEST_REQ_IS_TRUE(b.is_unrelated_to(i));

    LVD_TEST_REQ_IS_TRUE(!c.is_unrelated_to(a));
    LVD_TEST_REQ_IS_TRUE(c.is_unrelated_to(b));
    LVD_TEST_REQ_IS_TRUE(!c.is_unrelated_to(c));
    LVD_TEST_REQ_IS_TRUE(!c.is_unrelated_to(d));
    LVD_TEST_REQ_IS_TRUE(c.is_unrelated_to(i));

    LVD_TEST_REQ_IS_TRUE(!d.is_unrelated_to(a));
    LVD_TEST_REQ_IS_TRUE(!d.is_unrelated_to(b));
    LVD_TEST_REQ_IS_TRUE(!d.is_unrelated_to(c));
    LVD_TEST_REQ_IS_TRUE(!d.is_unrelated_to(d));
    LVD_TEST_REQ_IS_TRUE(d.is_unrelated_to(i));

    LVD_TEST_REQ_IS_TRUE(i.is_unrelated_to(a));
    LVD_TEST_REQ_IS_TRUE(i.is_unrelated_to(b));
    LVD_TEST_REQ_IS_TRUE(i.is_unrelated_to(c));
    LVD_TEST_REQ_IS_TRUE(i.is_unrelated_to(d));
    LVD_TEST_REQ_IS_TRUE(!i.is_unrelated_to(i));
LVD_TEST_END

} // end namespace lvd
