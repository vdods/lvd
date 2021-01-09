// 2020.09.22 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/g_req_context.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"

LVD_TEST_BEGIN(010__req__0)
    LVD_G_REQ_EQ(123, 123);

    // Default FailureBehavior for lvd::req::g_req_context is THROW
    lvd::test::call_function_and_expect_exception<lvd::req::Failure>([](){
        LVD_G_REQ_EQ(1, 2);
    });
LVD_TEST_END

LVD_TEST_BEGIN(010__req__1)
    LVD_G_REQ_EQ(123, 123);

    // Default FailureBehavior for lvd::req::g_req_context is THROW
    lvd::test::call_function_and_expect_and_process_exception<lvd::req::Failure>(
        [](){ LVD_G_REQ_EQ(1, 2); },
        [&test_log](lvd::req::Failure const &e){ test_log << lvd::Log::trc() << "indeed caught lvd::req::Failure\n"; }
    );
LVD_TEST_END
