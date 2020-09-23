// 2020.08.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/call_site.hpp"
#include "lvd/FiRange.hpp"
#include "lvd/fmt.hpp"
#include "lvd/g_log.hpp"
#include "lvd/Log.hpp"
#include <string>

namespace lvd {

[[noreturn]] inline void abort (
    std::string const &what_arg,
    FiRange const &firange,
    CallSite const &call_site
)
{
    g_log << Log::crt() << call_site << ": " << (firange.is_valid() ? LVD_FMT(what_arg << "; relevant firange = " << firange) : LVD_FMT(what_arg)) << '\n';
    ::abort();
}

#define LVD_ABORT(what_arg) abort(what_arg, lvd::FiRange::INVALID, LVD_CALL_SITE())
#define LVD_ABORT_WITH_FIRANGE(what_arg, firange) abort(what_arg, firange, LVD_CALL_SITE())

} // end namespace lvd
