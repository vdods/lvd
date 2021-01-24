// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/fmt.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include "lvd/Type_t.hpp"
#include "lvd/TypeString_t.hpp"

namespace lvd {

LVD_TEST_BEGIN(005__Type_t__00)
    auto &i = type_v<int>;
    auto &f = type_v<float>;

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
    LVD_TEST_REQ_EQ(type_v<bool>(123), bool(123));
    LVD_TEST_REQ_EQ(type_v<int>(123), int(123));
    LVD_TEST_REQ_EQ(type_v<double>(123), double(123));
LVD_TEST_END

LVD_TEST_BEGIN(005__Type_t__02)
    LVD_TEST_REQ_EQ(LVD_FMT(type_v<bool>), "bool");
    LVD_TEST_REQ_EQ(LVD_FMT(type_v<int32_t>), "int32_t");
LVD_TEST_END

class A { };
class B : public A { };
class C : public A { };
class D : public B, public C { };

LVD_TEST_BEGIN(005__Type_t__03)
    auto &a = type_v<A>;
    auto &b = type_v<B>;
    auto &c = type_v<C>;
    auto &d = type_v<D>;
    auto &i = type_v<int>;

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
