// 2020.08.24 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/aliases.hpp"
#include "lvd/move_cast.hpp"
#include "lvd/not_null.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"

struct A {
    int m;
    A (int m_) : m(m_) { }
    virtual ~A () { }
    bool operator == (A const &other) const { return this->m == other.m; }
};

struct B : public A {
    using A::A;
    using A::operator==;
};

struct C : public A {
    using A::A;
    using A::operator==;
};

//
// These ostream functions are needed for LVD_REQ_*
//

template <typename T_, typename Deleter_>
std::ostream &operator << (std::ostream &out, std::unique_ptr<T_,Deleter_> const &p) {
    return out << p.get();
}

std::ostream &operator << (std::ostream &out, A const &x) {
    return out << "A{" << x.m << '}';
}

LVD_TEST_BEGIN(210__move_cast__00__not_null_ptr__0__static_move_cast)
    B b(123);
    gsl::not_null<B*> p_b = &b;
    LVD_TEST_REQ_EQ(p_b.get(), &b);

    gsl::not_null<A*> p_a = lvd::static_move_cast<A>(p_b);
    LVD_TEST_REQ_EQ(p_a.get(), static_cast<A*>(&b));
    LVD_TEST_REQ_EQ(static_cast<A*>(p_b.get()), p_a.get());
LVD_TEST_END

LVD_TEST_BEGIN(210__move_cast__00__not_null_ptr__1__dynamic_move_cast)
    B b(123);
    gsl::not_null<B*> p_b = &b;
    LVD_TEST_REQ_EQ(p_b.get(), &b);

    gsl::not_null<A*> p_a = lvd::dynamic_move_cast<A>(p_b);
    LVD_TEST_REQ_EQ(p_a.get(), dynamic_cast<A*>(&b));
    LVD_TEST_REQ_EQ(dynamic_cast<A*>(p_b.get()), p_a.get());

    gsl::not_null<B*> p_b2 = lvd::dynamic_move_cast<B>(p_a);
    LVD_TEST_REQ_EQ(p_b2.get(), p_b.get());
LVD_TEST_END

LVD_TEST_BEGIN(210__move_cast__10__unique_ptr__0__static_move_cast)
    std::unique_ptr<B> b = std::make_unique<B>(123);
    LVD_TEST_REQ_NEQ_NULLPTR(b);
    LVD_TEST_REQ_EQ(*b, B{123});

    std::unique_ptr<A> a = lvd::static_move_cast<A>(b);
    LVD_TEST_REQ_NEQ_NULLPTR(a);
    LVD_TEST_REQ_EQ(*a, A{123});
    LVD_TEST_REQ_EQ_NULLPTR(b);
LVD_TEST_END

LVD_TEST_BEGIN(210__move_cast__10__unique_ptr__1__dynamic_move_cast)
    std::unique_ptr<B> b = std::make_unique<B>(123);
    LVD_TEST_REQ_NEQ_NULLPTR(b);
    LVD_TEST_REQ_EQ(*b, B{123});

    std::unique_ptr<A> a = lvd::dynamic_move_cast<A>(b);
    LVD_TEST_REQ_NEQ_NULLPTR(a);
    LVD_TEST_REQ_EQ(*a, A{123});
    LVD_TEST_REQ_EQ_NULLPTR(b);

    std::unique_ptr<B> b2 = lvd::dynamic_move_cast<B>(a);
    LVD_TEST_REQ_NEQ_NULLPTR(b2);
    LVD_TEST_REQ_EQ(*b2, B{123});
    LVD_TEST_REQ_EQ_NULLPTR(a);
LVD_TEST_END

LVD_TEST_BEGIN(210__move_cast__11__not_null_unique_ptr__0__static_move_cast)
    gsl::not_null<std::unique_ptr<B>> b = lvd::make_not_null_unique<B>(123);
    LVD_TEST_REQ_NEQ_NULLPTR(b);
    LVD_TEST_REQ_EQ(*b, B{123});

    gsl::not_null<std::unique_ptr<A>> a = lvd::static_move_cast<A>(b);
    LVD_TEST_REQ_NEQ_NULLPTR(a);
    LVD_TEST_REQ_EQ(*a, A{123});
    // Don't check b, it just has to be understood to be garbage at this point.
LVD_TEST_END

LVD_TEST_BEGIN(210__move_cast__11__not_null_unique_ptr__0__dynamic_move_cast)
    gsl::not_null<std::unique_ptr<B>> b = lvd::make_not_null_unique<B>(123);
    LVD_TEST_REQ_NEQ_NULLPTR(b);
    LVD_TEST_REQ_EQ(*b, B{123});

    gsl::not_null<std::unique_ptr<A>> a = lvd::dynamic_move_cast<A>(b);
    LVD_TEST_REQ_NEQ_NULLPTR(a);
    LVD_TEST_REQ_EQ(*a, A{123});
    // Don't check b, it just has to be understood to be garbage at this point.

    gsl::not_null<std::unique_ptr<B>> b2 = lvd::dynamic_move_cast<B>(a);
    LVD_TEST_REQ_NEQ_NULLPTR(b2);
    LVD_TEST_REQ_EQ(*b2, B{123});
    // Don't check a, it just has to be understood to be garbage at this point.
LVD_TEST_END
