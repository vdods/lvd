// 2020.03.13 - Victor Dods

#include <cassert>
#include <iostream>
#include "lvd/core.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"

int main (int argc, char **argv) {
    auto test_context = lvd::test::Context(std::cout);
    if (argc >= 2)
        test_context.with_filter(argv[1]);
    lvd::test::root_test_group_singleton().run(test_context);
    return 0;
}
