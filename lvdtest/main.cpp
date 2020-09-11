// 2020.03.13 - Victor Dods

#include <cassert>
#include <iostream>
#include "lvd/core.hpp"
#include "lvd/req.hpp"
#include "lvd/Test.hpp"

int main (int argc, char **argv) {
    std::string filter;
    if (argc >= 2)
        filter = argv[1];
    auto test_context = lvd::TestContext(std::cout, lvd::req::FailureBehavior::THROW, filter);
    lvd::root_test_group_singleton().run(test_context);
    return 0;
}
