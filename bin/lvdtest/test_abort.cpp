// 2020.08.26 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/abort.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"

LVD_TEST_BEGIN(110__abort)
    return; // We just want to compile this test, not actually execute it.
    LVD_ABORT("for reasons");
LVD_TEST_END

LVD_TEST_BEGIN(110__abort_with_firange)
    return; // We just want to compile this test, not actually execute it.
    LVD_ABORT_WITH_FIRANGE("for reasons", lvd::FiRange("thingy", lvd::FiPos(1,2)));
LVD_TEST_END
