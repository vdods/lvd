// 2020.04.24 - Victor Dods

#pragma once

#include <cassert>
#include <functional>
#include "lvd/core.hpp"
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

namespace lvd {

// TODO: Use FiLoc instead
struct FileLocation {
    std::string m_file;
    int m_line;
};

inline std::ostream &operator << (std::ostream &out, FileLocation const &filoc) {
    return out << filoc.m_file << ':' << filoc.m_line;
}

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
    FileLocation const &registration_location () const { return m_registration_location; }
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
    FileLocation m_registration_location;
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

    void operator() () const { m_evaluator(); }

    virtual bool is_test_function () const override { return true; }

private:

    std::function<void()> m_evaluator;
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
    void run (std::ostream &out, std::string const &filter = std::string()) const;
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

#ifdef LVD_COMPILE_TESTS

// Retrieve the root TestGroup singleton, creating it if necessary.
TestGroup &root_test_group_singleton ();

// #define LVD_STRINGIFY(x) #x
// #define LVD_FILE_LOCATION_AS_STRING __FILE__ "__" LVD_STRINGIFY(__LINE__)
#define LVD_FILE_LOCATION() lvd::FileLocation{__FILE__, __LINE__}
// #define LVD_TEST_FUNCTION(name, evaluator) lvd::TestFunction(name, LVD_FILE_LOCATION(), evaluator)
// #define LVD_REGISTER_TEST(path, name, evaluator) lvd::root_test_group_singleton().register_test(path, LVD_TEST_FUNCTION(name, evaluator))
#define LVD_TEST_FUNCTION(evaluator) lvd::TestFunction(LVD_FILE_LOCATION(), evaluator)
#define LVD_REGISTER_TEST(path, evaluator) namespace { \
    struct __##path { \
        __##path () { \
            lvd::root_test_group_singleton().register_test(#path, LVD_TEST_FUNCTION(evaluator)); \
        } \
    }; \
    __##path __##path##__SINGLETON; \
}

#endif

} // end namespace lvd
