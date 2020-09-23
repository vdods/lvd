// 2020.08.26 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/NullOstream.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"

LVD_TEST_BEGIN(120__NullOstream)
    lvd::NullOstream nout;
    nout << "blah";
    nout << 123;
    nout << std::endl;
    nout.flush();
LVD_TEST_END
