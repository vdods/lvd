// 2020.04.01 - Victor Dods

#pragma once

#include "lvd/core.hpp"
#include <cassert>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace lvd {
namespace req {

// TODO: Maybe throw an exception of type failed_requirement which derives from std::runtime_error.

//
// Use these macros to get nice, descriptive output.
//

#define LVD_REQ_CONDITION_1PARAM(cond, param) lvd::req::verify_condition_1param(cond, #cond, param, #param, __FILE__, __LINE__, __func__)
#define LVD_REQ_CONDITION_2PARAM(cond, param0, param1) lvd::req::verify_condition_2param(cond, #cond, param0, #param0, param1, #param1, __FILE__, __LINE__, __func__)
#define LVD_REQ_CONDITION_3PARAM(cond, param0, param1, param2) lvd::req::verify_condition_3param(cond, #cond, param0, #param0, param1, #param1, param2, #param2, __FILE__, __LINE__, __func__)
#define LVD_REQ_CONDITION_4PARAM(cond, param0, param1, param2, param3) lvd::req::verify_condition_4param(cond, #cond, param0, #param0, param1, #param1, param2, #param2, param3, #param3, __FILE__, __LINE__, __func__)

#define LVD_REQ_IS_TRUE(param) lvd::req::is_true(param, #param, __FILE__, __LINE__, __func__)
#define LVD_REQ_IS_FALSE(param) lvd::req::is_false(param, #param, __FILE__, __LINE__, __func__)

#define LVD_REQ_EQ_NULLPTR(param) lvd::req::eq_nullptr(param, #param, __FILE__, __LINE__, __func__)
#define LVD_REQ_NEQ_NULLPTR(param) lvd::req::neq_nullptr(param, #param, __FILE__, __LINE__, __func__)

#define LVD_REQ_EQ(lhs, rhs) lvd::req::eq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_REQ_NEQ(lhs, rhs) lvd::req::neq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_REQ_LT(lhs, rhs) lvd::req::lt(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_REQ_LEQ(lhs, rhs) lvd::req::leq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_REQ_GT(lhs, rhs) lvd::req::gt(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_REQ_GEQ(lhs, rhs) lvd::req::geq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)

#define LVD_REQ_COMPARE(lhs, rhs, expected) lvd::req::compare(lhs, rhs, expected, #lhs, #rhs, #expected, __FILE__, __LINE__, __func__)
#define LVD_REQ_COMPARE_IS_EQ(lhs, rhs) lvd::req::compare_is_eq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_REQ_COMPARE_IS_NEQ(lhs, rhs) lvd::req::compare_is_neq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_REQ_COMPARE_IS_LT(lhs, rhs) lvd::req::compare_is_lt(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_REQ_COMPARE_IS_LEQ(lhs, rhs) lvd::req::compare_is_leq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_REQ_COMPARE_IS_GT(lhs, rhs) lvd::req::compare_is_gt(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)
#define LVD_REQ_COMPARE_IS_GEQ(lhs, rhs) lvd::req::compare_is_geq(lhs, rhs, #lhs, #rhs, __FILE__, __LINE__, __func__)

template <typename Param_>
inline void verify_condition_1param (
    bool condition,
    std::string const &condition_description,
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func
) {
    if (!condition) {
        // Use a separate stream object because we're going to set the format flags on it and don't want
        // to screw up any existing state.
        std::ostringstream out;
        out << std::boolalpha; // Print boolean values as words.
        out << std::setprecision(17); // Enough to fully distinguish double values.
        out << file << ':' << line << ": error: in function \"" << func << "\":\n"
            << "    failed condition: " << condition_description << '\n'
            << "    " << param_description << " was " << param << " (at address " << &param << ")\n";
        std::cerr << out.str();
        ::abort();
    }
}

template <typename Param0_, typename Param1_>
inline void verify_condition_2param (
    bool condition,
    std::string const &condition_description,
    Param0_ const &param0,
    std::string const &param0_description,
    Param1_ const &param1,
    std::string const &param1_description,
    std::string const &file,
    int line,
    std::string const &func
) {
    if (!condition) {
        // Use a separate stream object because we're going to set the format flags on it and don't want
        // to screw up any existing state.
        std::ostringstream out;
        out << std::boolalpha; // Print boolean values as words.
        out << std::setprecision(17); // Enough to fully distinguish double values.
        out << file << ':' << line << ": error: in function \"" << func << "\":\n"
            << "    failed condition: " << condition_description << '\n'
            << "    " << param0_description << " was " << param0 << " (at address " << &param0 << ")\n"
            << "    " << param1_description << " was " << param1 << " (at address " << &param1 << ")\n";
        std::cerr << out.str();
        ::abort();
    }
}

template <typename Param0_, typename Param1_, typename Param2_>
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
    std::string const &func
) {
    if (!condition) {
        // Use a separate stream object because we're going to set the format flags on it and don't want
        // to screw up any existing state.
        std::ostringstream out;
        out << std::boolalpha; // Print boolean values as words.
        out << std::setprecision(17); // Enough to fully distinguish double values.
        out << file << ':' << line << ": error: in function \"" << func << "\":\n"
            << "    failed condition: " << condition_description << '\n'
            << "    " << param0_description << " was " << param0 << " (at address " << &param0 << ")\n"
            << "    " << param1_description << " was " << param1 << " (at address " << &param1 << ")\n"
            << "    " << param2_description << " was " << param2 << " (at address " << &param2 << ")\n";
        std::cerr << out.str();
        ::abort();
    }
}

template <typename Param0_, typename Param1_, typename Param2_, typename Param3_>
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
    std::string const &func
) {
    if (!condition) {
        // Use a separate stream object because we're going to set the format flags on it and don't want
        // to screw up any existing state.
        std::ostringstream out;
        out << std::boolalpha; // Print boolean values as words.
        out << std::setprecision(17); // Enough to fully distinguish double values.
        out << file << ':' << line << ": error: in function \"" << func << "\":\n"
            << "    failed condition: " << condition_description << '\n'
            << "    " << param0_description << " was " << param0 << " (at address " << &param0 << ")\n"
            << "    " << param1_description << " was " << param1 << " (at address " << &param1 << ")\n"
            << "    " << param2_description << " was " << param2 << " (at address " << &param2 << ")\n"
            << "    " << param3_description << " was " << param3 << " (at address " << &param3 << ")\n";
        std::cerr << out.str();
        ::abort();
    }
}

template <typename Param_>
inline void is_true (
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func
) {
    verify_condition_1param(bool(param), LVD_FMT("bool(" << param_description << ')'), param, param_description, file, line, func);
}

template <typename Param_>
inline void is_false (
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func
) {
    verify_condition_1param(!bool(param), LVD_FMT("!bool(" << param_description << ')'), param, param_description, file, line, func);
}

template <typename Param_>
inline void eq_nullptr (
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func
) {
    verify_condition_1param(param == nullptr, LVD_FMT(param_description << " == nullptr"), param, param_description, file, line, func);
}

template <typename Param_>
inline void neq_nullptr (
    Param_ const &param,
    std::string const &param_description,
    std::string const &file,
    int line,
    std::string const &func
) {
    verify_condition_1param(param != nullptr, LVD_FMT(param_description << " != nullptr"), param, param_description, file, line, func);
}

template <typename Lhs_, typename Rhs_>
inline void eq (
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func
) {
    verify_condition_2param(lhs == rhs, LVD_FMT(lhs_description << " == " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func);
}

template <typename Lhs_, typename Rhs_>
inline void neq (
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func
) {
    verify_condition_2param(lhs != rhs, LVD_FMT(lhs_description << " != " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func);
}

template <typename Lhs_, typename Rhs_>
inline void lt (
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func
) {
    verify_condition_2param(lhs < rhs, LVD_FMT(lhs_description << " < " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func);
}

template <typename Lhs_, typename Rhs_>
inline void leq (
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func
) {
    verify_condition_2param(lhs <= rhs, LVD_FMT(lhs_description << " <= " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func);
}

template <typename Lhs_, typename Rhs_>
inline void gt (
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func
) {
    verify_condition_2param(lhs > rhs, LVD_FMT(lhs_description << " > " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func);
}

template <typename Lhs_, typename Rhs_>
inline void geq (
    Lhs_ const &lhs,
    Rhs_ const &rhs,
    std::string const &lhs_description,
    std::string const &rhs_description,
    std::string const &file,
    int line,
    std::string const &func
) {
    verify_condition_2param(lhs >= rhs, LVD_FMT(lhs_description << " >= " << rhs_description), lhs, lhs_description, rhs, rhs_description, file, line, func);
}

} // end namespace req
} // end namespace lvd
