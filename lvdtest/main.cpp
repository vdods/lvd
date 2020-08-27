// 2020.03.13 - Victor Dods

#include <cassert>
#include <iostream>
#include "lvd/core.hpp"
#include "lvd/Test.hpp"

int main (int argc, char **argv) {
    std::string filter;
    if (argc >= 2)
        filter = argv[1];
    // Temp hack to get around the fact that CLI parsing isn't present yet.
    auto test_output = filter.empty() ? lvd::TestOutput::TERSE : lvd::TestOutput::VERBOSE;
    lvd::root_test_group_singleton().run(std::cout, test_output, filter);
    return 0;
}
