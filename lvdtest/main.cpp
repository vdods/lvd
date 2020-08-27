// 2020.03.13 - Victor Dods

#include <cassert>
#include <iostream>
#include "lvd/core.hpp"
#include "lvd/Test.hpp"

int main (int argc, char **argv) {
    std::string filter;
    if (argc >= 2)
        filter = argv[1];
    lvd::root_test_group_singleton().run(std::cout, filter);
    return 0;
}
