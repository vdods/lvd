// 2020.03.13 - Victor Dods

#include <cassert>
#include <iostream>
#include "lvd/fmt.hpp"
#include "lvd/g_log.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"

int main (int argc, char **argv) {
    auto test_context = lvd::test::Context(lvd::g_log);
    if (argc >= 2)
        test_context.with_filter(argv[1]);
//     test_context.with_failure_behavior(lvd::req::FailureBehavior::ABORT); // TODO: Make configurable via CLI
    lvd::test::root_test_group_singleton().run(test_context);
//     // Print failure paths to stdout, so that e.g. another program could collect and run them separately.
//     // TODO: Make this configurable via CLI.
//     for (auto const &failure_path : test_context.failure_paths())
//         std::cout << failure_path << '\n';
    return test_context.failure_paths().size() > 0 ? 1 : 0;
}
