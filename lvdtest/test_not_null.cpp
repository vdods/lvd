// 2020.08.24 - Victor Dods

// This causes a violation of not_null to throw gsl::fail_fast.
// NOTE: The default behavior, if no define is provided, is to terminate.
#define GSL_THROW_ON_CONTRACT_VIOLATION

#include "lvd/aliases.hpp"
#include "lvd/not_null.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"

LVD_TEST_BEGIN(200__not_null)
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
