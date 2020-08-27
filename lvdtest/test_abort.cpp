// 2020.08.26 - Victor Dods

#include "lvd/abort.hpp"
#include "lvd/req.hpp"
#include "lvd/Test.hpp"

LVD_REGISTER_TEST(110__abort, ([](){
    return; // We just want to compile this test, not actually execute it.
    LVD_ABORT("for reasons");
}));

LVD_REGISTER_TEST(110__abort_with_firange, ([](){
    return; // We just want to compile this test, not actually execute it.
    LVD_ABORT_WITH_FIRANGE("for reasons", lvd::FiRange("thingy", lvd::FiPos(1,2)));
}));
