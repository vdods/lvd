// 2020.04.01 - Victor Dods

#pragma once

#include "lvd/fmt.hpp"
#include "lvd/literal.hpp"
#include "lvd/Log.hpp"
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace lvd {
namespace req {

class Failure : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

enum class FailureBehavior : uint8_t {
    ABORT = 0,
    THROW = 1
};

class Context {
public:

    Context () = delete;
    Context (Context const &) = delete;
    Context (Context &&) = default;
    Context (Log &log)
        :   m_log(log)
        ,   m_failure_behavior(FailureBehavior::THROW)
    { }

    Context &operator = (Context const &) = delete;
    Context &operator = (Context &&) = default;

    //
    // Builder pattern methods
    //

    Context &with_failure_behavior (FailureBehavior failure_behavior) & {
        m_failure_behavior = failure_behavior;
        return *this;
    }
    Context with_failure_behavior (FailureBehavior failure_behavior) && {
        m_failure_behavior = failure_behavior;
        return std::move(*this);
    }

    //
    // End builder pattern methods
    //

    Log &log () { return m_log; }
    FailureBehavior failure_behavior () const { return m_failure_behavior; }

private:

    Log &m_log;
    FailureBehavior m_failure_behavior;
};

//
// Use these macros to get nice, descriptive output.
// NOTE: `,##__VA_ARGS__` is a g++ extension, but maybe it works for other compilers.
//

#define LVD_REQ_CONDITION_1PARAM(context, cond, param, ...) lvd::req::verify_condition_1param(context, cond, #cond, param, #param, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LVD_REQ_CONDITION_2PARAM(context, cond, param0, param1, ...) lvd::req::verify_condition_2param(context, cond, #cond, param0, #param0, param1, #param1, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LVD_REQ_CONDITION_3PARAM(context, cond, param0, param1, param2, ...) lvd::req::verify_condition_3param(context, cond, #cond, param0, #param0, param1, #param1, param2, #param2, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LVD_REQ_CONDITION_4PARAM(context, cond, param0, param1, param2, param3, ...) lvd::req::verify_condition_4param(context, cond, #cond, param0, #param0, param1, #param1, param2, #param2, param3, #param3, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define LVD_REQ_IS_TRUE(context, param, ...) lvd::req::is_true(context, param, #param, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LVD_REQ_IS_FALSE(context, param, ...) lvd::req::is_false(context, param, #param, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define LVD_REQ_EQ_NULLPTR(context, param, ...) lvd::req::eq_nullptr(context, param, #param, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LVD_REQ_NEQ_NULLPTR(context, param, ...) lvd::req::neq_nullptr(context, param, #param, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

#define LVD_REQ_EQ(context, lhs, rhs, ...) lvd::req::eq(context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LVD_REQ_NEQ(context, lhs, rhs, ...) lvd::req::neq(context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LVD_REQ_LT(context, lhs, rhs, ...) lvd::req::lt(context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LVD_REQ_LEQ(context, lhs, rhs, ...) lvd::req::leq(context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LVD_REQ_GT(context, lhs, rhs, ...) lvd::req::gt(context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, ##__VA_ARGS__)
#define LVD_REQ_GEQ(context, lhs, rhs, ...) lvd::req::geq(context, lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, ##__VA_ARGS__)

template <typename Param_>
inline void verify_condition_1param (
    Context &context,
    bool condition,
    std::string const &condition_description,
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    if (!condition) {
        // Use a separate stream object because we're going to set the format flags on it and don't want
        // to screw up any existing state.
        std::ostringstream str_out;
        str_out << std::boolalpha; // Print boolean values as words.
        str_out << std::setprecision(17); // Enough to fully distinguish double values.
        str_out << file << ':' << line << ": error: in function \"" << func << "\":\n"
            << "    failed condition: " << condition_description << '\n'
            << "    expression `" << param_description << "` had value " << literal_of(param) << " (at address " << &param << ")\n";
        if (!explanation.empty())
            str_out << "    explanation: " << explanation;
        if (context.failure_behavior() == FailureBehavior::ABORT) {
            context.log() << Log::crt() << IndentGuard() << str_out.str() << '\n';
            context.log().flush();
            ::abort();
        } else
            throw Failure(str_out.str());
    }
}

template <typename Param0_, typename Param1_>
inline void verify_condition_2param (
    Context &context,
    bool condition,
    std::string const &condition_description,
    Param0_ const &param0,
    std::string const &param0_description,
    Param1_ const &param1,
    std::string const &param1_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    if (!condition) {
        // Use a separate stream object because we're going to set the format flags on it and don't want
        // to screw up any existing state.
        std::ostringstream str_out;
        str_out << std::boolalpha; // Print boolean values as words.
        str_out << std::setprecision(17); // Enough to fully distinguish double values.
        str_out << file << ':' << line << ": error: in function \"" << func << "\":\n"
            << "    failed condition: " << condition_description << '\n'
            << "    expression `" << param0_description << "` had value " << literal_of(param0) << " (at address " << &param0 << ")\n"
            << "    expression `" << param1_description << "` had value " << literal_of(param1) << " (at address " << &param1 << ")\n";
        if (!explanation.empty())
            str_out << "    explanation: " << explanation;
        if (context.failure_behavior() == FailureBehavior::ABORT) {
            context.log() << Log::crt() << IndentGuard() << str_out.str() << '\n';
            context.log().flush();
            ::abort();
        } else
            throw Failure(str_out.str());
    }
}

template <typename Param0_, typename Param1_, typename Param2_>
inline void verify_condition_3param (
    Context &context,
    bool condition,
    std::string const &condition_description,
    Param0_ const &param0,
    std::string const &param0_description,
    Param1_ const &param1,
    std::string const &param1_description,
    Param2_ const &param2,
    std::string const &param2_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    if (!condition) {
        // Use a separate stream object because we're going to set the format flags on it and don't want
        // to screw up any existing state.
        std::ostringstream str_out;
        str_out << std::boolalpha; // Print boolean values as words.
        str_out << std::setprecision(17); // Enough to fully distinguish double values.
        str_out << file << ':' << line << ": error: in function \"" << func << "\":\n"
            << "    failed condition: " << condition_description << '\n'
            << "    expression `" << param0_description << "` had value " << literal_of(param0) << " (at address " << &param0 << ")\n"
            << "    expression `" << param1_description << "` had value " << literal_of(param1) << " (at address " << &param1 << ")\n"
            << "    expression `" << param2_description << "` had value " << literal_of(param2) << " (at address " << &param2 << ")\n";
        if (!explanation.empty())
            str_out << "    explanation: " << explanation;
        if (context.failure_behavior() == FailureBehavior::ABORT) {
            context.log() << Log::crt() << IndentGuard() << str_out.str() << '\n';
            context.log().flush();
            ::abort();
        } else
            throw Failure(str_out.str());
    }
}

template <typename Param0_, typename Param1_, typename Param2_, typename Param3_>
inline void verify_condition_4param (
    Context &context,
    bool condition,
    std::string const &condition_description,
    Param0_ const &param0,
    std::string const &param0_description,
    Param1_ const &param1,
    std::string const &param1_description,
    Param2_ const &param2,
    std::string const &param2_description,
    Param3_ const &param3,
    std::string const &param3_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    if (!condition) {
        // Use a separate stream object because we're going to set the format flags on it and don't want
        // to screw up any existing state.
        std::ostringstream str_out;
        str_out << std::boolalpha; // Print boolean values as words.
        str_out << std::setprecision(17); // Enough to fully distinguish double values.
        str_out << file << ':' << line << ": error: in function \"" << func << "\":\n"
            << "    failed condition: " << condition_description << '\n'
            << "    expression `" << param0_description << "` had value " << literal_of(param0) << " (at address " << &param0 << ")\n"
            << "    expression `" << param1_description << "` had value " << literal_of(param1) << " (at address " << &param1 << ")\n"
            << "    expression `" << param2_description << "` had value " << literal_of(param2) << " (at address " << &param2 << ")\n"
            << "    expression `" << param3_description << "` had value " << literal_of(param3) << " (at address " << &param3 << ")\n";
        if (!explanation.empty())
            str_out << "    explanation: " << explanation;
        if (context.failure_behavior() == FailureBehavior::ABORT) {
            context.log() << Log::crt() << IndentGuard() << str_out.str() << '\n';
            context.log().flush();
            ::abort();
        } else
            throw Failure(str_out.str());
    }
}

template <typename Param_>
inline void is_true (
    Context &context,
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    verify_condition_1param(context, bool(param), LVD_FMT("bool(" << param_description << ") == true"), param, param_description, file, line, func, explanation);
}

template <typename Param_>
inline void is_false (
    Context &context,
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    verify_condition_1param(context, !bool(param), LVD_FMT("bool(" << param_description << ") == false"), param, param_description, file, line, func, explanation);
}

template <typename Param_>
inline void eq_nullptr (
    Context &context,
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    verify_condition_1param(context, param == nullptr, LVD_FMT(param_description << " == nullptr"), param, param_description, file, line, func, explanation);
}

template <typename Param_>
inline void neq_nullptr (
    Context &context,
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    verify_condition_1param(context, param != nullptr, LVD_FMT(param_description << " != nullptr"), param, param_description, file, line, func, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void eq (
    Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    verify_condition_2param(context, lhs == rhs, LVD_FMT(lhs_description << " == " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void neq (
    Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    verify_condition_2param(context, lhs != rhs, LVD_FMT(lhs_description << " != " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void lt (
    Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    verify_condition_2param(context, lhs < rhs, LVD_FMT(lhs_description << " < " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void leq (
    Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    verify_condition_2param(context, lhs <= rhs, LVD_FMT(lhs_description << " <= " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void gt (
    Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    verify_condition_2param(context, lhs > rhs, LVD_FMT(lhs_description << " > " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func, explanation);
}

template <typename Lhs_, typename Rhs_>
inline void geq (
    Context &context,
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func,
    std::string const &explanation = std::string()
) {
    verify_condition_2param(context, lhs >= rhs, LVD_FMT(lhs_description << " >= " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func, explanation);
}

} // end namespace req
} // end namespace lvd
