// 2020.04.24 - Victor Dods

#pragma once

#include <cassert>
#include <functional>
#include "lvd/core.hpp"
#include "lvd/FiLoc.hpp"
#include "lvd/req.hpp"
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

class TestContext {
public:

    TestContext () = delete;
    TestContext (TestContext const &) = delete;
    TestContext (TestContext &&) = delete;
    // filter is an optional prefix to filter test names on.  E.g. "/595" or "/595/" or "/595/serialization"
    TestContext (std::ostream &out, req::FailureBehavior failure_behavior, std::string const &filter = std::string())
        :   m_req_context(out, failure_behavior)
        ,   m_filter(filter)
    { }

    bool operator = (TestContext const &) = delete;
    bool operator = (TestContext &&) = delete;

    std::ostream &out () { return m_req_context.out(); }
    req::FailureBehavior failure_behavior () const { return m_req_context.failure_behavior(); }
    std::string const &filter () const { return m_filter; }

    req::Context &req_context () { return m_req_context; }

private:

    req::Context m_req_context;
    std::string m_filter;
};

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
    { }

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
    virtual void run (TestContext &test_context) const = 0;
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

    virtual bool is_test_function () const override { return true; }
    virtual void run (TestContext &test_context) const override;

private:

    std::function<void(std::ostream&,req::Context&)> m_evaluator;
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

    virtual bool is_test_group () const override { return true; }
    virtual void run (TestContext &test_context) const override;
    virtual void print (std::ostream &out) const override;

    TestGroup &register_test (std::string const &test_function_path, TestFunction &&test_function);
    TestGroup &register_test_impl (
        std::vector<std::string>::const_iterator node_path_begin,
        std::vector<std::string>::const_iterator node_path_end,
        TestFunction &&test_function
    );

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
#define LVD_TEST_BEGIN(path) namespace { lvd::TestRegistrar __##path{LVD_FILOC(), #path, [](std::ostream &test_out, lvd::req::Context &req_context){
#define LVD_TEST_END }}; }

//
// For use with lvd/req.hpp -- these bind test_out to the out parameter of the LVD_REQ_* macros.
//

#define LVD_TEST_REQ_CONDITION_1PARAM(cond, param) lvd::req::verify_condition_1param(req_context, cond, #cond, param, #param, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_CONDITION_2PARAM(cond, param0, param1) lvd::req::verify_condition_2param(req_context, cond, #cond, param0, #param0, param1, #param1, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_CONDITION_3PARAM(cond, param0, param1, param2) lvd::req::verify_condition_3param(req_context, cond, #cond, param0, #param0, param1, #param1, param2, #param2, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_CONDITION_4PARAM(cond, param0, param1, param2, param3) lvd::req::verify_condition_4param(req_context, cond, #cond, param0, #param0, param1, #param1, param2, #param2, param3, #param3, __FILE__, __LINE__, __func__)

#define LVD_TEST_REQ_IS_TRUE(param) lvd::req::is_true(req_context, param, #param, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_IS_FALSE(param) lvd::req::is_false(req_context, param, #param, __FILE__, __LINE__, __func__)

#define LVD_TEST_REQ_EQ_NULLPTR(param) lvd::req::eq_nullptr(req_context, param, #param, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_NEQ_NULLPTR(param) lvd::req::neq_nullptr(req_context, param, #param, __FILE__, __LINE__, __func__)

#define LVD_TEST_REQ_EQ(lhs, rhs) lvd::req::eq(req_context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_NEQ(lhs, rhs) lvd::req::neq(req_context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_LT(lhs, rhs) lvd::req::lt(req_context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_LEQ(lhs, rhs) lvd::req::leq(req_context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_GT(lhs, rhs) lvd::req::gt(req_context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_GEQ(lhs, rhs) lvd::req::geq(req_context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)

#define LVD_TEST_REQ_COMPARE(lhs, rhs, expected) lvd::req::compare(req_context, lhs, rhs, expected, #lhs, #rhs, #expected, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_COMPARE_IS_EQ(lhs, rhs) lvd::req::compare_is_eq(req_context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_COMPARE_IS_NEQ(lhs, rhs) lvd::req::compare_is_neq(req_context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_COMPARE_IS_LT(lhs, rhs) lvd::req::compare_is_lt(req_context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_COMPARE_IS_LEQ(lhs, rhs) lvd::req::compare_is_leq(req_context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_COMPARE_IS_GT(lhs, rhs) lvd::req::compare_is_gt(req_context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_TEST_REQ_COMPARE_IS_GEQ(lhs, rhs) lvd::req::compare_is_geq(req_context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)

// For verifying that a piece of code throws a particular exception type.
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
