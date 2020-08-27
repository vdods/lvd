// 2020.04.24 - Victor Dods

#include "lvd/Test.hpp"

#include "lvd/NullOstream.hpp"
#include <regex>

namespace lvd {

//
// TestNode
//

std::vector<std::string> TestNode::path () const {
    std::vector<std::string> p;
    if (has_parent()) {
        p = parent().path();
        p.emplace_back(name());
    }
    return p;
}

bool TestNode::passes_filter (std::string const &filter) const {
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

void TestNode::print (std::ostream &out) const {
    out << *this << (is_test_group() ? " : TestGroup\n" : " : TestFunction\n");
}

TestFunction const &TestNode::as_test_function () const & {
    if (!is_test_function())
        throw std::runtime_error("expected is_test_function() to be true");
    return dynamic_cast<TestFunction const &>(*this);
}

TestFunction &TestNode::as_test_function () & {
    if (!is_test_function())
        throw std::runtime_error("expected is_test_function() to be true");
    return dynamic_cast<TestFunction &>(*this);
}

TestGroup const &TestNode::as_test_group () const & {
    if (!is_test_group())
        throw std::runtime_error("expected is_test_group() to be true");
    return dynamic_cast<TestGroup const &>(*this);
}

TestGroup &TestNode::as_test_group () & {
    if (!is_test_group())
        throw std::runtime_error("expected is_test_group() to be true");
    return dynamic_cast<TestGroup &>(*this);
}

std::ostream &operator << (std::ostream &out, TestNode const &node) {
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
// TestGroup
//

void TestGroup::run (std::ostream &out, TestOutput test_output, std::string const &filter) const {
    out << "Running tests at " << *this;
    if (!filter.empty())
        out << " with filter \"" << filter << '"';
    out << " ...\n";
    for (auto const &[name, node] : m_nodes) {
        assert(!name.empty());
        if (!node->passes_filter(filter))
            continue;

        if (node->is_test_function()) {
            out << "Running testfunc " << *node << " ...\n";
            auto const &test_function = node->as_test_function();
            test_function(test_output == TestOutput::VERBOSE ? out : nout);
            out << "PASSED: testfunc " << *node << "\n";
        } else {
            assert(node->is_test_group());
            // Otherwise that node is a TestGroup, so just recurse to that.
            auto const &test_group = node->as_test_group();
            test_group.run(out, test_output, filter);
        }
    }
    out << "PASSED: tests at " << *this << " ...\n";
}

TestGroup &TestGroup::register_test (std::string const &test_function_path, TestFunction &&test_function) {
    std::regex regex{"(__)"}; // The delimiter is __
    std::sregex_token_iterator it{test_function_path.begin(), test_function_path.end(), regex, -1};
    std::vector<std::string> test_function_path_vector{it, {}};
    if (test_function_path_vector.empty())
        throw std::runtime_error(LVD_FMT("invalid path \"" << test_function_path << "\""));
    test_function.set_name(std::move(test_function_path_vector.back()));
    test_function_path_vector.pop_back();
    return register_test_impl(test_function_path_vector.begin(), test_function_path_vector.end(), std::move(test_function));
}

TestGroup &TestGroup::register_test_impl (
    std::vector<std::string>::const_iterator node_path_begin,
    std::vector<std::string>::const_iterator node_path_end,
    TestFunction &&test_function
) {
    if (node_path_begin == node_path_end) {
        // Base case
        auto it = m_nodes.find(test_function.name());
        if (it != m_nodes.end()) {
            // If there's already a node there...
            auto &matching_node = *it->second;
            // If that node is a TestFunction, then this is a collision, which is an error.
            throw std::runtime_error(LVD_FMT("Can't register a TestFunction at " << *this << '/' << test_function.name() << " -- there is already something registered there; registration location was " << matching_node.registration_location()));
        }
        // Add test_function.
        test_function.set_parent(this);
        // This is done because order of evaluation of params isn't guaranteed to be left-to-right.
        auto name = test_function.name();
        m_nodes.emplace(name, std::make_unique<TestFunction>(std::move(test_function)));
    } else {
        // Recursive case
        auto const &first_node_name = *node_path_begin;
        assert(!first_node_name.empty());
        TestGroup *test_group_ptr = nullptr;
        auto it = m_nodes.find(first_node_name);
        // If there's no TestGroup there yet, create one.  Use registration_location of test_function.
        if (it == m_nodes.end()) {
            auto test_group_ptr_ = std::make_unique<TestGroup>(first_node_name, test_function.registration_location(), this);
            test_group_ptr = test_group_ptr_.get();
            m_nodes.emplace(first_node_name, std::move(test_group_ptr_));
        } else {
            if (!it->second->is_test_group())
                throw std::runtime_error(LVD_FMT(*it->second << " is a TestFunction, not a TestGroup; could not recurse"));
            test_group_ptr = &it->second->as_test_group();
        }
        assert(test_group_ptr != nullptr);
        test_group_ptr->register_test_impl(node_path_begin+1, node_path_end, std::move(test_function));
    }
    return *this;
}

void TestGroup::print (std::ostream &out) const {
    TestNode::print(out);
    for (auto const &[name, node] : m_nodes) {
        std::ignore = name;
        node->print(out);
    }
}

std::unique_ptr<TestGroup> g_root_test_group_singleton = nullptr;

TestGroup &root_test_group_singleton () {
    if (g_root_test_group_singleton == nullptr)
        g_root_test_group_singleton = std::make_unique<TestGroup>();
    return *g_root_test_group_singleton;
}

} // end namespace lvd
