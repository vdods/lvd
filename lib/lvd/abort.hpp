// 2020.08.22 - Victor Dods

#pragma once

#include "lvd/FiLoc.hpp"
#include "lvd/FiRange.hpp"
#include "lvd/fmt.hpp"
#include "lvd/g_log.hpp"
#include "lvd/Log.hpp"
#include <string>

namespace lvd {

struct FiLocAndFuncName {
    FiLoc m_filoc;
    std::string m_func_name;

    FiLocAndFuncName () = delete;
    template <typename FiLoc_, typename FuncName_>
    FiLocAndFuncName (FiLoc_ &&filoc, FuncName_ &&func_name)
        :   m_filoc(std::forward<FiLoc_>(filoc))
        ,   m_func_name(std::forward<FuncName_>(func_name))
    { }
};

inline std::ostream &operator << (std::ostream &out, FiLocAndFuncName const &f) {
    return out << f.m_filoc << " - in function " << f.m_func_name;
}

#define LVD_CALL_SITE() lvd::FiLocAndFuncName(lvd::FiLoc(__FILE__, __LINE__), __func__)

[[noreturn]] inline void abort (
    std::string const &what_arg,
    FiRange const &firange,
    FiLocAndFuncName const &call_site
)
{
    g_log << Log::crt() << call_site << ": " << (firange.is_valid() ? LVD_FMT(what_arg << "; relevant firange = " << firange) : LVD_FMT(what_arg)) << '\n';
    ::abort();
}

#define LVD_ABORT(what_arg) abort(what_arg, lvd::FiRange::INVALID, LVD_CALL_SITE())
#define LVD_ABORT_WITH_FIRANGE(what_arg, firange) abort(what_arg, firange, LVD_CALL_SITE())

} // end namespace lvd
