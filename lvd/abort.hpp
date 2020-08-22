// 2020.08.22 - Victor Dods

#pragma once

#include "lvd/core.hpp"
#include "lvd/FiRange.hpp"
#include "lvd/g_log.hpp"
#include "lvd/Log.hpp"
#include <string>

namespace lvd {

[[noreturn]] inline void abort (
    std::string const &what_arg,
    FiRange const &firange,
    std::string const &file,
    int line,
    std::string const &func
)
{
    g_log << Log::crt() << file << ':' << line << " - " << func << ": " << (firange.is_valid() ? LVD_FMT(what_arg << "; relevant firange = " << firange) : LVD_FMT(what_arg)) << '\n';
    abort();
}

#define LVD_ABORT(what_arg) abort(what_arg, FiRange::INVALID, __FILE__, __LINE__, __func__)
#define LVD_ABORT_WITH_FIRANGE(what_arg, firange) abort(what_arg, firange, __FILE__, __LINE__, __func__)

} // end namespace lvd
