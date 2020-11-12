// 2020.04.24 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/test.hpp"

#include "lvd/g_log.hpp"
#include "lvd/g_req_context.hpp"
#include <regex>

namespace lvd {
namespace test {

void print_help_message (std::string const &program_title, std::string const &argv_0) {
    g_log.set_log_level_threshold(LogLevel::NIL);
    g_log << program_title << "\n\n"
             "Usage: " << argv_0 << " [args]\n\n"
             "    Will return 0 upon success, or nonzero if any tests fail.\n\n"
             "Options:\n";
    g_log << IndentGuard()
          << "--log-level=<X>        : If present, sets the log level to <X>, where <X> must be one of:\n"
             "                         CRT, ERR, WRN, INF, DBG, TRC, NIL.  The value is case-insensitive.\n"
             "                         The default value is WRN.\n"
             "--print-failed         : If present, enables printing of failed tests, one per line, to stdout.\n"
             "                         Default behavior is to print nothing to stdout; errors are printed to stderr.\n"
             "--failure-behavior=<Y> : If present, sets the test failure behavior to <Y>, where <Y> must be one of:\n"
             "                         abort (halt program at point of failure), or throw (catch test failure and\n"
             "                         continue running rest of tests).  Default behavior is throw.\n"
             "--help                 : If present, prints this help message.\n"
             "<test-filter>          : If present, specifies the filter with which to decide which tests to run.\n";
}

// If return code is nonzero, the help message will be printed.
int basic_test_main (std::string const &program_title, int argc_, char **argv_)
{
    auto argv = std::vector<std::string>(argv_, argv_+argc_);

    // Default log level, if unspecified, is WRN.
    g_log.set_log_level_threshold(lvd::LogLevel::WRN);
    // Default filter is empty.
    std::string filter;
    // Indicates if failed tests should be printed to stdout.  Default is false.
    bool print_failed = false;
    // Determines behavior upon test failure.  Default is throw (and continue with remaining tests).
    auto failure_behavior = req::FailureBehavior::THROW;

    // Parse the arguments
    size_t i = 0;
    for (auto const &arg : argv) {
        if (i == 0) {
            ++i;
            continue; // Skip the first arg, since that's the program name.
        }

        if (arg == "--help") {
            print_help_message(program_title, argv[0]);
            return 0;
        } else if (arg.substr(0, 12) == "--log-level=") {
            auto val = arg.substr(12);

            // Convert to lowercase.
            for (auto &c : val)
                if ('A' <= c && c <= 'Z')
                    c += 'a' - 'A';

            if (val == "nil")      g_log.set_log_level_threshold(lvd::LogLevel::NIL);
            else if (val == "trc") g_log.set_log_level_threshold(lvd::LogLevel::TRC);
            else if (val == "dbg") g_log.set_log_level_threshold(lvd::LogLevel::DBG);
            else if (val == "inf") g_log.set_log_level_threshold(lvd::LogLevel::INF);
            else if (val == "wrn") g_log.set_log_level_threshold(lvd::LogLevel::WRN);
            else if (val == "err") g_log.set_log_level_threshold(lvd::LogLevel::ERR);
            else if (val == "crt") g_log.set_log_level_threshold(lvd::LogLevel::CRT);
            else {
                g_log.set_log_level_threshold(lvd::LogLevel::ERR);
                g_log << Log::err() << "invalid --log-level value \"" << arg << "\"\n";
                print_help_message(program_title, argv[0]);
                return 1;
            }
        } else if (arg == "--print-failed") {
            print_failed = true;
        } else if (arg.substr(0, 19) == "--failure-behavior=") {
            auto val = arg.substr(19);

            // Convert to lowercase.
            for (auto &c : val)
                if ('A' <= c && c <= 'Z')
                    c += 'a' - 'A';

            if (val == "abort") failure_behavior = req::FailureBehavior::ABORT;
            else if (val == "throw") failure_behavior = req::FailureBehavior::THROW;
            else {
                g_log.set_log_level_threshold(lvd::LogLevel::ERR);
                g_log << Log::err() << "unrecognized value for --failure-behavior \"" << val << "\"; must be abort or throw.\n";
                print_help_message(program_title, argv[0]);
                return 1;
            }
        } else if (arg.substr(0, 1) == "/") {
            filter = arg;
        } else {
            g_log.set_log_level_threshold(lvd::LogLevel::ERR);
            g_log << Log::err() << "unrecognized commandline argument \"" << arg << "\"\n";
            print_help_message(program_title, argv[0]);
            return 1;
        }

        ++i;
    }

    // Create the test Context and set its properties.
    auto test_context = lvd::test::Context(g_log)
        .with_failure_behavior(failure_behavior)
        .with_filter(filter);
    // Also set g_req_context FailureBehavior to the specified one.
    lvd::req::g_req_context.with_failure_behavior(failure_behavior);

    lvd::test::root_test_group_singleton().run(test_context);
    // If requested, print failure paths to stdout, so that e.g. another program could collect and run them separately.
    if (print_failed)
        for (auto const &failure_path : test_context.failure_paths())
            std::cout << failure_path << '\n';
    return test_context.failure_paths().size() > 0 ? 1 : 0;
}

//
// Node
//

std::vector<std::string> Node::path () const {
    std::vector<std::string> p;
    if (has_parent()) {
        p = parent().path();
        p.emplace_back(name());
    }
    return p;
}

bool Node::passes_filter (std::string const &filter) const {
    if (filter.empty())
        return true;

    // Determine full string path of this node, so that filtering can be done.
    auto node_path_string = LVD_FMT(*this);
    assert(!node_path_string.empty());
    assert(node_path_string[0] == '/');

    if (is_test_function()) {
        // node_path_string must have filter as a leading substring.
        return node_path_string.substr(0, filter.size()) == filter;
    } else if (is_test_group()) {
        // If node_path_string and filter aren't equal on their common length, then the filter isn't satisfied.
        auto common_size = std::min(node_path_string.size(), filter.size());
        return node_path_string.substr(0, common_size) == filter.substr(0, common_size);
    } else {
        assert(false && "this should be impossible");
        return false;
    }
}

void Node::print (std::ostream &out) const {
    out << *this << (is_test_group() ? " : Group\n" : " : Function\n");
}

Function const &Node::as_test_function () const & {
    if (!is_test_function())
        throw std::runtime_error("expected is_test_function() to be true");
    return dynamic_cast<Function const &>(*this);
}

Function &Node::as_test_function () & {
    if (!is_test_function())
        throw std::runtime_error("expected is_test_function() to be true");
    return dynamic_cast<Function &>(*this);
}

Group const &Node::as_test_group () const & {
    if (!is_test_group())
        throw std::runtime_error("expected is_test_group() to be true");
    return dynamic_cast<Group const &>(*this);
}

Group &Node::as_test_group () & {
    if (!is_test_group())
        throw std::runtime_error("expected is_test_group() to be true");
    return dynamic_cast<Group &>(*this);
}

std::ostream &operator << (std::ostream &out, Node const &node) {
    if (node.has_parent()) {
        out << node.parent() << node.name();
        if (node.is_test_group())
            out << '/';
    } else {
        out << '/';
    }
    return out;
}

//
// Function
//

void Function::run (Context &context) const {
    try {
        context.log() << Log::inf() << "Running testfunc " << *this << " ...\n";
        context.increment_test_count();
        m_evaluator(context.log(), context.req_context());
        context.log() << Log::inf() << "Passed: testfunc " << *this << "\n";
    } catch (std::exception const &e) {
        context.record_failure_path(LVD_FMT(*this));
        context.log() << Log::err() << "Failed: testfunc " << *this << " -- exception was:\n" << IndentGuard() << e.what() << '\n';
    } catch (...) {
        context.record_failure_path(LVD_FMT(*this));
        context.log() << Log::err() << "Failed: testfunc " << *this << " -- non-exception was thrown\n";
    }
}

//
// Group
//

void Group::run (Context &context) const {
    auto previous_test_count = context.test_count();
    auto previous_failure_path_count = context.failure_paths().size();

    context.log() << Log::inf() << "Running tests at " << *this;
    if (!context.filter().empty())
        context.log() << Log::inf() << " with filter \"" << context.filter() << '"';
    context.log() << Log::inf() << " ...\n";
    for (auto const &[name, node] : m_nodes) {
        assert(!name.empty());
        if (node->passes_filter(context.filter()))
            node->run(context);
    }

    auto current_test_count = context.test_count();
    auto current_failure_path_count = context.failure_paths().size();
    assert(current_test_count >= previous_test_count);
    assert(current_failure_path_count >= previous_failure_path_count);
    auto subordinate_test_count = current_test_count - previous_test_count;
    auto subordinate_test_failure_count = current_failure_path_count - previous_failure_path_count;
    if (subordinate_test_failure_count > 0)
        context.log() << Log::err() << "Failed: " << subordinate_test_failure_count << " out of " << subordinate_test_count << " tests under " << *this << '\n';
    else
        context.log() << Log::inf() << "Passed: all " << subordinate_test_count << " tests under " << *this << '\n';

    // If this is the root Group, print a summary.
    if (!has_parent()) {
        auto subordinate_test_success_count = subordinate_test_count - subordinate_test_failure_count;
        if (subordinate_test_failure_count > 0) {
            context.log() << Log::crt() << "Summary: " << subordinate_test_count << " test(s) were run.\n";

            IndentGuard ig0(context.log());
            context.log() << Log::crt() << subordinate_test_success_count << " test(s) passed.\n"
                                        << subordinate_test_failure_count << " test(s) failed (listed below):\n";

            IndentGuard ig1(context.log());
            for (auto const &failure_path : context.failure_paths()) {
                context.log() << Log::crt() << failure_path << '\n';
            }
        } else {
            context.log() << Log::inf() << "Summary: All " << subordinate_test_count << " test(s) passed.\n";
        }
    }
}

void Group::print (std::ostream &out) const {
    Node::print(out);
    for (auto const &[name, node] : m_nodes) {
        std::ignore = name;
        node->print(out);
    }
}

Group &Group::register_test (std::string const &test_function_path, Function &&test_function) {
    std::regex regex{"(__)"}; // The delimiter is __
    std::sregex_token_iterator it{test_function_path.begin(), test_function_path.end(), regex, -1};
    std::vector<std::string> test_function_path_vector{it, {}};
    if (test_function_path_vector.empty())
        throw std::runtime_error(LVD_FMT("invalid path \"" << test_function_path << "\""));
    test_function.set_name(std::move(test_function_path_vector.back()));
    test_function_path_vector.pop_back();
    return register_test_impl(test_function_path_vector.begin(), test_function_path_vector.end(), std::move(test_function));
}

Group &Group::register_test_impl (
    std::vector<std::string>::const_iterator node_path_begin,
    std::vector<std::string>::const_iterator node_path_end,
    Function &&test_function
) {
    if (node_path_begin == node_path_end) {
        // Base case
        auto it = m_nodes.find(test_function.name());
        if (it != m_nodes.end()) {
            // If there's already a node there...
            auto &matching_node = *it->second;
            // If that node is a Function, then this is a collision, which is an error.
            throw std::runtime_error(LVD_FMT("Can't register a Function at " << *this << '/' << test_function.name() << " -- there is already something registered there; registration location was " << matching_node.registration_location()));
        }
        // Add test_function.
        test_function.set_parent(this);
        // This is done because order of evaluation of params isn't guaranteed to be left-to-right.
        auto name = test_function.name();
        m_nodes.emplace(name, std::make_unique<Function>(std::move(test_function)));
    } else {
        // Recursive case
        auto const &first_node_name = *node_path_begin;
        assert(!first_node_name.empty());
        Group *test_group_ptr = nullptr;
        auto it = m_nodes.find(first_node_name);
        // If there's no Group there yet, create one.  Use registration_location of test_function.
        if (it == m_nodes.end()) {
            auto test_group_ptr_ = std::make_unique<Group>(first_node_name, test_function.registration_location(), this);
            test_group_ptr = test_group_ptr_.get();
            m_nodes.emplace(first_node_name, std::move(test_group_ptr_));
        } else {
            if (!it->second->is_test_group())
                throw std::runtime_error(LVD_FMT(*it->second << " is a Function, not a Group; could not recurse"));
            test_group_ptr = &it->second->as_test_group();
        }
        assert(test_group_ptr != nullptr);
        test_group_ptr->register_test_impl(node_path_begin+1, node_path_end, std::move(test_function));
    }
    return *this;
}

//
// Root Group singleton and lazy-initializing accessor
//

std::unique_ptr<Group> g_root_test_group_singleton = nullptr;

Group &root_test_group_singleton () {
    if (g_root_test_group_singleton == nullptr)
        g_root_test_group_singleton = std::make_unique<Group>();
    return *g_root_test_group_singleton;
}

} // end namespace test
} // end namespace lvd
