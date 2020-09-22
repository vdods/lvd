// 2020.08.22 - Victor Dods

#pragma once

#include "lvd/FiLoc.hpp"
#include <string>

namespace lvd {

struct CallSite {
    FiLoc m_filoc;
    std::string m_func_name;

    CallSite () = delete;
    template <typename FiLoc_, typename FuncName_>
    CallSite (FiLoc_ &&filoc, FuncName_ &&func_name)
        :   m_filoc(std::forward<FiLoc_>(filoc))
        ,   m_func_name(std::forward<FuncName_>(func_name))
    { }
};

inline std::ostream &operator << (std::ostream &out, CallSite const &f) {
    return out << f.m_filoc << " - in function " << f.m_func_name;
}

#define LVD_CALL_SITE() lvd::CallSite(lvd::FiLoc(__FILE__, __LINE__), __func__)

} // end namespace lvd
