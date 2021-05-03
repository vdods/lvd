// 2020.08.24 - Copyright Victor Dods - Licensed under Apache 2.0

// This causes a violation of not_null to throw gsl::fail_fast.
// NOTE: The default behavior, if no define is provided, is to terminate.
#define GSL_THROW_ON_CONTRACT_VIOLATION

#include "lvd/aliases.hpp"
#include "lvd/not_null.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"

namespace std {

template <class P_, typename Deleter_>
ostream &operator<< (ostream &out, unique_ptr<P_,Deleter_> const &p) {
    return out << "std::unique_ptr<...>{" << p.get() << '}';
}

} // end namespace std

LVD_TEST_BEGIN(200__not_null__0__not_null_ptr)
    int x = 123;

    auto p1 = gsl::not_null<int*>(&x);
    LVD_TEST_REQ_NEQ_NULLPTR(p1);
    LVD_TEST_REQ_EQ(*p1, x);

    auto p2 = lvd::nn<int*>(&x);
    LVD_TEST_REQ_NEQ_NULLPTR(p2);
    LVD_TEST_REQ_EQ(*p2, x);

    int *p3 = nullptr;
    lvd::test::call_function_and_expect_exception<gsl::fail_fast>(
        [p3](){
            lvd::nn<int*>{p3};
        }
    );
LVD_TEST_END

LVD_TEST_BEGIN(200__not_null__1__not_null_unique_ptr)
    int x = 456;

    auto p1 = gsl::not_null<std::unique_ptr<int>>(std::make_unique<int>(x));
    LVD_TEST_REQ_NEQ_NULLPTR(p1);
    LVD_TEST_REQ_EQ(*p1, x);

    auto p2 = lvd::nn<std::unique_ptr<int>>(std::make_unique<int>(x));
    LVD_TEST_REQ_NEQ_NULLPTR(p2);
    LVD_TEST_REQ_EQ(*p2, x);

    std::unique_ptr<int> p3 = nullptr;
//         lvd::nn<std::unique_ptr<int>> x3(p3);
    lvd::test::call_function_and_expect_exception<gsl::fail_fast>(
        [&](){
            lvd::nn<std::unique_ptr<int>> blah(std::move(p3));
        }
    );

    // Test move semantics
    std::unique_ptr<int> p4 = std::move(p1).move_get();
    LVD_TEST_REQ_NEQ_NULLPTR(p4);
    LVD_TEST_REQ_EQ(*p4, x);
LVD_TEST_END

LVD_TEST_BEGIN(200__not_null__2__not_null_shared_ptr)
    int x = 456;

    auto p1 = gsl::not_null<std::shared_ptr<int>>(std::make_unique<int>(x));
    LVD_TEST_REQ_NEQ_NULLPTR(p1);
    LVD_TEST_REQ_EQ(*p1, x);

    auto p2 = lvd::nn<std::shared_ptr<int>>(std::make_unique<int>(x));
    LVD_TEST_REQ_NEQ_NULLPTR(p2);
    LVD_TEST_REQ_EQ(*p2, x);

    std::shared_ptr<int> p3 = nullptr;
    lvd::test::call_function_and_expect_exception<gsl::fail_fast>(
        [p3](){
            lvd::nn<std::shared_ptr<int>>{p3};
        }
    );

    // Test move semantics
    std::shared_ptr<int> p4 = std::move(p1).move_get();
    LVD_TEST_REQ_NEQ_NULLPTR(p4);
    LVD_TEST_REQ_EQ(*p4, x);
LVD_TEST_END
