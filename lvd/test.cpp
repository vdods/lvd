// 2020.04.24 - Victor Dods

#include "lvd/test.hpp"

#include <regex>

namespace lvd {
namespace test {

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
        context.out() << Log::inf() << "Running testfunc " << *this << " ...\n";
        context.increment_test_count();
        m_evaluator(context.out(), context.req_context());
        context.out() << Log::inf() << "Passed: testfunc " << *this << "\n";
    } catch (std::exception const &e) {
        context.record_failure_path(LVD_FMT(*this));
        context.out() << Log::err() << "Failed: testfunc " << *this << " -- exception was:\n" << IndentGuard() << e.what() << '\n';
    } catch (...) {
        context.record_failure_path(LVD_FMT(*this));
        context.out() << Log::err() << "Failed: testfunc " << *this << " -- non-exception was thrown\n";
    }
}

//
// Group
//

void Group::run (Context &context) const {
    auto previous_test_count = context.test_count();
    auto previous_failure_path_count = context.failure_paths().size();

    context.out() << Log::inf() << "Running tests at " << *this;
    if (!context.filter().empty())
        context.out() << " with filter \"" << context.filter() << '"';
    context.out() << " ...\n";
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
        context.out() << Log::err() << "Failed: " << subordinate_test_failure_count << " out of " << subordinate_test_count << " tests under " << *this << '\n';
    else
        context.out() << Log::inf() << "Passed: all " << subordinate_test_count << " tests under " << *this << '\n';

    // If this is the root Group, print a summary.
    if (!has_parent()) {
        if (subordinate_test_failure_count > 0) {
            context.out() << Log::err() << "\nSummary: " << subordinate_test_failure_count << " out of " << subordinate_test_count << " tests failed:\n";
            IndentGuard ig(context.out());
            for (auto const &failure_path : context.failure_paths()) {
                context.out() << Log::err() << failure_path << '\n';
            }
        } else {
            context.out() << Log::inf() << "\nSummary: all " << subordinate_test_count << " tests passed.\n";
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
