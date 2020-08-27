// 2020.04.24 - Victor Dods

#pragma once

#include <cassert>
#include <functional>
#include "lvd/core.hpp"
#include "lvd/FiLoc.hpp"
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace lvd {

enum class TestOutput : uint8_t {
    TERSE = 0,
    VERBOSE
};

class TestFunction;
class TestGroup;

class TestNode {
public:

    TestNode () : m_parent(nullptr) { }
    TestNode (TestNode const &) = default;
    TestNode (TestNode &&) noexcept = default;

    virtual ~TestNode () { }

    template <typename Name_, typename RegLoc_>
    TestNode (Name_ &&name, RegLoc_ &&reg_loc, TestGroup *parent)
    :   m_name(std::forward<Name_>(name))
    ,   m_registration_location(std::forward<RegLoc_>(reg_loc))
    ,   m_parent(parent)
    {
//         assert(!m_name.empty());
    }

    std::string const &name () const { return m_name; }
    template <typename... Args_>
    void set_name (Args_&&... args) { m_name = std::string(std::forward<Args_>(args)...); }
    FiLoc const &registration_location () const { return m_registration_location; }
    bool has_parent () const { return m_parent != nullptr; }
    TestGroup const &parent () const { return *m_parent; }
    void set_parent (TestGroup *parent) { m_parent = parent; }
    std::vector<std::string> path () const;
    bool passes_filter (std::string const &filter) const;

    virtual bool is_test_function () const { return false; }
    virtual bool is_test_group () const { return false; }

    virtual void print (std::ostream &out) const;

    TestFunction const &as_test_function () const &;
    TestFunction &as_test_function () &;
    TestGroup const &as_test_group () const &;
    TestGroup &as_test_group () &;

private:

    std::string m_name;
    FiLoc m_registration_location;
    TestGroup *m_parent;
};

std::ostream &operator << (std::ostream &out, TestNode const &node);

class TestFunction : public TestNode {
public:

    TestFunction () = delete;
    TestFunction (TestFunction const &) = default;
    TestFunction (TestFunction &&) noexcept = default;

    virtual ~TestFunction () { }

    template <typename RegLoc_, typename Evaluator_>
    TestFunction (RegLoc_ &&reg_loc, Evaluator_ &&evaluator)
    :   TestNode("", std::forward<RegLoc_>(reg_loc), nullptr)
    ,   m_evaluator(std::forward<Evaluator_>(evaluator))
    {
        assert(m_evaluator != nullptr);
    }
    template <typename Name_, typename RegLoc_, typename Evaluator_>
    TestFunction (Name_ &&name, RegLoc_ &&reg_loc, Evaluator_ &&evaluator)
    :   TestNode(std::forward<Name_>(name), std::forward<RegLoc_>(reg_loc), nullptr)
    ,   m_evaluator(std::forward<Evaluator_>(evaluator))
    {
        assert(m_evaluator != nullptr);
    }

    void operator() (std::ostream &out) const { m_evaluator(out); }

    virtual bool is_test_function () const override { return true; }

private:

    std::function<void(std::ostream&)> m_evaluator;
};

class TestGroup : public TestNode {
public:

    TestGroup () = default;
    TestGroup (TestGroup const &) = delete;
    TestGroup (TestGroup &&) = delete;

    virtual ~TestGroup () { }

    template <typename Name_, typename RegLoc_>
    TestGroup (Name_ &&name, RegLoc_ &&reg_loc, TestGroup *parent)
    :   TestNode(std::forward<Name_>(name), std::forward<RegLoc_>(reg_loc), parent)
    { }

    // filter is an optional prefix to filter test names on.  E.g. "/595" or "/595/" or "/595/serialization"
    void run (std::ostream &out, TestOutput test_output, std::string const &filter = std::string()) const;
    TestGroup &register_test (std::string const &test_function_path, TestFunction &&test_function);
    TestGroup &register_test_impl (
        std::vector<std::string>::const_iterator node_path_begin,
        std::vector<std::string>::const_iterator node_path_end,
        TestFunction &&test_function
    );

    virtual bool is_test_group () const override { return true; }
    virtual void print (std::ostream &out) const override;

private:

    using TestNodePtr = std::unique_ptr<TestNode>;

    std::map<std::string,TestNodePtr> m_nodes;
};

// Retrieve the root TestGroup singleton, creating it if necessary.
TestGroup &root_test_group_singleton ();

// Instantiating a singleton of this class is what registers a test, but use the
// LVD_TEST_BEGIN and LVD_TEST_END macros to do this, for example:
//
//     LVD_TEST_BEGIN(numerics__matrix__det)
//         auto m = Matrix<double,3,3>::Identity();
//         LVD_REQ_EQ(m.det(), 1.0);
//     LVD_TEST_END
class TestRegistrar {
public:

    template <typename... Args_>
    TestRegistrar (FiLoc &&reg_loc, std::string const &test_function_path, Args_&&... args) {
        root_test_group_singleton().register_test(
            test_function_path,
            TestFunction(std::move(reg_loc), std::forward<Args_>(args)...)
        );
    }
};

// Use these macros to define a test.  LVD_TEST_BEGIN opens a lambda which is used as the test body
// and LVD_TEST_END closes the lambda.
#define LVD_TEST_BEGIN(path) namespace { lvd::TestRegistrar __##path{LVD_FILOC(), #path, [](std::ostream &test_out){
#define LVD_TEST_END }}; }

template <typename ExceptionType_>
void call_function_and_expect_exception (std::function<void()> func) {
    try {
        func();
    } catch (ExceptionType_ const &) {
        // Caught expected exception type.
        return;
    } catch (std::exception const &e) {
        throw std::runtime_error(LVD_FMT("error: did not catch the expected exception type, but instead caught: " << e.what() ));
    } catch (...) {
        throw std::runtime_error(LVD_FMT("error: did not catch the expected exception type, but caught a non-exception type instead"));
    }
    throw std::runtime_error("error: no exception occured, but one was expected");
}

} // end namespace lvd
