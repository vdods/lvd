// 2020.08.24 - Victor Dods

#include "lvd/aliases.hpp"
#include "lvd/move_cast.hpp"
#include "lvd/not_null.hpp"
#include "lvd/req.hpp"
#include "lvd/Test.hpp"

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

LVD_REGISTER_TEST(210__move_cast__0__unique_ptr__0__static_move_cast, ([](){
    std::unique_ptr<B> b = std::make_unique<B>(123);
    LVD_REQ_NEQ_NULLPTR(b);
    LVD_REQ_EQ(*b, B{123});

    std::unique_ptr<A> a = lvd::static_move_cast<A>(b);
    LVD_REQ_NEQ_NULLPTR(a);
    LVD_REQ_EQ(*a, A{123});
    LVD_REQ_EQ_NULLPTR(b);
}));

LVD_REGISTER_TEST(210__move_cast__0__unique_ptr__1__dynamic_move_cast, ([](){
    std::unique_ptr<B> b = std::make_unique<B>(123);
    LVD_REQ_NEQ_NULLPTR(b);
    LVD_REQ_EQ(*b, B{123});

    std::unique_ptr<A> a = lvd::dynamic_move_cast<A>(b);
    LVD_REQ_NEQ_NULLPTR(a);
    LVD_REQ_EQ(*a, A{123});
    LVD_REQ_EQ_NULLPTR(b);

    std::unique_ptr<B> b2 = lvd::dynamic_move_cast<B>(a);
    LVD_REQ_NEQ_NULLPTR(b2);
    LVD_REQ_EQ(*b2, B{123});
    LVD_REQ_EQ_NULLPTR(a);
}));

LVD_REGISTER_TEST(210__move_cast__1__not_null_unique_ptr__0__static_move_cast, ([](){
    gsl::not_null<std::unique_ptr<B>> b = lvd::make_not_null_unique<B>(123);
    LVD_REQ_NEQ_NULLPTR(b);
    LVD_REQ_EQ(*b, B{123});

    gsl::not_null<std::unique_ptr<A>> a = lvd::static_move_cast<A>(b);
    LVD_REQ_NEQ_NULLPTR(a);
    LVD_REQ_EQ(*a, A{123});
    // Don't check b, it just has to be understood to be garbage at this point.
}));

LVD_REGISTER_TEST(210__move_cast__1__not_null_unique_ptr__0__dynamic_move_cast, ([](){
    gsl::not_null<std::unique_ptr<B>> b = lvd::make_not_null_unique<B>(123);
    LVD_REQ_NEQ_NULLPTR(b);
    LVD_REQ_EQ(*b, B{123});

    gsl::not_null<std::unique_ptr<A>> a = lvd::dynamic_move_cast<A>(b);
    LVD_REQ_NEQ_NULLPTR(a);
    LVD_REQ_EQ(*a, A{123});
    // Don't check b, it just has to be understood to be garbage at this point.

    gsl::not_null<std::unique_ptr<B>> b2 = lvd::dynamic_move_cast<B>(a);
    LVD_REQ_NEQ_NULLPTR(b2);
    LVD_REQ_EQ(*b2, B{123});
    // Don't check a, it just has to be understood to be garbage at this point.
}));
