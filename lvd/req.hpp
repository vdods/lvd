// 2020.04.01 - Victor Dods

#pragma once

#include "lvd/core.hpp"
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace lvd {
namespace req {

class failure : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

enum class FailureBehavior : uint8_t {
    ABORT = 0,
    THROW = 1
};

// Must be defined in the app that uses this.
extern FailureBehavior g_failure_behavior;

//
// Use these macros to get nice, descriptive output.
//

#define LVD_REQ_CONDITION_1PARAM(cond, param, out) lvd::req::verify_condition_1param(cond, #cond, param, #param, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_CONDITION_2PARAM(cond, param0, param1, out) lvd::req::verify_condition_2param(cond, #cond, param0, #param0, param1, #param1, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_CONDITION_3PARAM(cond, param0, param1, param2, out) lvd::req::verify_condition_3param(cond, #cond, param0, #param0, param1, #param1, param2, #param2, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_CONDITION_4PARAM(cond, param0, param1, param2, param3, out) lvd::req::verify_condition_4param(cond, #cond, param0, #param0, param1, #param1, param2, #param2, param3, #param3, __FILE__, __LINE__, __func__, out)

#define LVD_REQ_IS_TRUE(param, out) lvd::req::is_true(param, #param, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_IS_FALSE(param, out) lvd::req::is_false(param, #param, __FILE__, __LINE__, __func__, out)

#define LVD_REQ_EQ_NULLPTR(param, out) lvd::req::eq_nullptr(param, #param, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_NEQ_NULLPTR(param, out) lvd::req::neq_nullptr(param, #param, __FILE__, __LINE__, __func__, out)

#define LVD_REQ_EQ(lhs, rhs, out) lvd::req::eq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_NEQ(lhs, rhs, out) lvd::req::neq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_LT(lhs, rhs, out) lvd::req::lt(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_LEQ(lhs, rhs, out) lvd::req::leq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_GT(lhs, rhs, out) lvd::req::gt(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_GEQ(lhs, rhs, out) lvd::req::geq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, out)

#define LVD_REQ_COMPARE(lhs, rhs, expected, out) lvd::req::compare(lhs, rhs, expected, #lhs, #rhs, #expected, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_COMPARE_IS_EQ(lhs, rhs, out) lvd::req::compare_is_eq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_COMPARE_IS_NEQ(lhs, rhs, out) lvd::req::compare_is_neq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_COMPARE_IS_LT(lhs, rhs, out) lvd::req::compare_is_lt(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_COMPARE_IS_LEQ(lhs, rhs, out) lvd::req::compare_is_leq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_COMPARE_IS_GT(lhs, rhs, out) lvd::req::compare_is_gt(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, out)
#define LVD_REQ_COMPARE_IS_GEQ(lhs, rhs, out) lvd::req::compare_is_geq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__, out)

template <typename Param_, typename Out_>
inline void verify_condition_1param (
    bool condition,
    std::string const &condition_description,
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func,
    Out_ &out
) {
    if (!condition) {
        // Use a separate stream object because we're going to set the format flags on it and don't want
        // to screw up any existing state.
        std::ostringstream str_out;
        str_out << std::boolalpha; // Print boolean values as words.
        str_out << std::setprecision(17); // Enough to fully distinguish double values.
        str_out << file << ':' << line << ": error: in function \"" << func << "\":\n"
            << "    failed condition: " << condition_description << '\n'
            << "    expression `" << param_description << "` had value `" << param << "` (at address " << &param << ")\n";
        if (g_failure_behavior == FailureBehavior::ABORT) {
            out << str_out.str();
            out.flush();
            ::abort();
        } else
            throw failure(str_out.str());
    }
}

template <typename Param0_, typename Param1_, typename Out_>
inline void verify_condition_2param (
    bool condition,
    std::string const &condition_description,
    Param0_ const &param0,
    std::string const &param0_description,
    Param1_ const &param1,
    std::string const &param1_description,
    std::string const &file,
    int line,
    std::string const &func,
    Out_ &out
) {
    if (!condition) {
        // Use a separate stream object because we're going to set the format flags on it and don't want
        // to screw up any existing state.
        std::ostringstream str_out;
        str_out << std::boolalpha; // Print boolean values as words.
        str_out << std::setprecision(17); // Enough to fully distinguish double values.
        str_out << file << ':' << line << ": error: in function \"" << func << "\":\n"
            << "    failed condition: " << condition_description << '\n'
            << "    expression `" << param0_description << "` had value `" << param0 << "` (at address " << &param0 << ")\n"
            << "    expression `" << param1_description << "` had value `" << param1 << "` (at address " << &param1 << ")\n";
        if (g_failure_behavior == FailureBehavior::ABORT) {
            out << str_out.str();
            out.flush();
            ::abort();
        } else
            throw failure(str_out.str());
    }
}

template <typename Param0_, typename Param1_, typename Param2_, typename Out_>
inline void verify_condition_3param (
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
    Out_ &out
) {
    if (!condition) {
        // Use a separate stream object because we're going to set the format flags on it and don't want
        // to screw up any existing state.
        std::ostringstream str_out;
        str_out << std::boolalpha; // Print boolean values as words.
        str_out << std::setprecision(17); // Enough to fully distinguish double values.
        str_out << file << ':' << line << ": error: in function \"" << func << "\":\n"
            << "    failed condition: " << condition_description << '\n'
            << "    expression `" << param0_description << "` had value `" << param0 << "` (at address " << &param0 << ")\n"
            << "    expression `" << param1_description << "` had value `" << param1 << "` (at address " << &param1 << ")\n"
            << "    expression `" << param2_description << "` had value `" << param2 << "` (at address " << &param2 << ")\n";
        if (g_failure_behavior == FailureBehavior::ABORT) {
            out << str_out.str();
            out.flush();
            ::abort();
        } else
            throw failure(str_out.str());
    }
}

template <typename Param0_, typename Param1_, typename Param2_, typename Param3_, typename Out_>
inline void verify_condition_4param (
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
    Out_ &out
) {
    if (!condition) {
        // Use a separate stream object because we're going to set the format flags on it and don't want
        // to screw up any existing state.
        std::ostringstream str_out;
        str_out << std::boolalpha; // Print boolean values as words.
        str_out << std::setprecision(17); // Enough to fully distinguish double values.
        str_out << file << ':' << line << ": error: in function \"" << func << "\":\n"
            << "    failed condition: " << condition_description << '\n'
            << "    expression `" << param0_description << "` had value `" << param0 << "` (at address " << &param0 << ")\n"
            << "    expression `" << param1_description << "` had value `" << param1 << "` (at address " << &param1 << ")\n"
            << "    expression `" << param2_description << "` had value `" << param2 << "` (at address " << &param2 << ")\n"
            << "    expression `" << param3_description << "` had value `" << param3 << "` (at address " << &param3 << ")\n";
        if (g_failure_behavior == FailureBehavior::ABORT) {
            out << str_out.str();
            out.flush();
            ::abort();
        } else
            throw failure(str_out.str());
    }
}

template <typename Param_, typename Out_>
inline void is_true (
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func,
    Out_ &out
) {
    verify_condition_1param(bool(param), LVD_FMT("bool(" << param_description << ')'), param, param_description, file, line, func, out);
}

template <typename Param_, typename Out_>
inline void is_false (
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func,
    Out_ &out
) {
    verify_condition_1param(!bool(param), LVD_FMT("!bool(" << param_description << ')'), param, param_description, file, line, func, out);
}

template <typename Param_, typename Out_>
inline void eq_nullptr (
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func,
    Out_ &out
) {
    verify_condition_1param(param == nullptr, LVD_FMT(param_description << " == nullptr"), param, param_description, file, line, func, out);
}

template <typename Param_, typename Out_>
inline void neq_nullptr (
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func,
    Out_ &out
) {
    verify_condition_1param(param != nullptr, LVD_FMT(param_description << " != nullptr"), param, param_description, file, line, func, out);
}

template <typename Lhs_, typename Rhs_, typename Out_>
inline void eq (
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func,
    Out_ &out
) {
    verify_condition_2param(lhs == rhs, LVD_FMT(lhs_description << " == " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func, out);
}

template <typename Lhs_, typename Rhs_, typename Out_>
inline void neq (
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func,
    Out_ &out
) {
    verify_condition_2param(lhs != rhs, LVD_FMT(lhs_description << " != " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func, out);
}

template <typename Lhs_, typename Rhs_, typename Out_>
inline void lt (
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func,
    Out_ &out
) {
    verify_condition_2param(lhs < rhs, LVD_FMT(lhs_description << " < " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func, out);
}

template <typename Lhs_, typename Rhs_, typename Out_>
inline void leq (
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func,
    Out_ &out
) {
    verify_condition_2param(lhs <= rhs, LVD_FMT(lhs_description << " <= " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func, out);
}

template <typename Lhs_, typename Rhs_, typename Out_>
inline void gt (
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func,
    Out_ &out
) {
    verify_condition_2param(lhs > rhs, LVD_FMT(lhs_description << " > " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func, out);
}

template <typename Lhs_, typename Rhs_, typename Out_>
inline void geq (
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func,
    Out_ &out
) {
    verify_condition_2param(lhs >= rhs, LVD_FMT(lhs_description << " >= " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func, out);
}

} // end namespace req
} // end namespace lvd
