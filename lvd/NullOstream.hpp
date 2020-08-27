// 2020.08.26 - Victor Dods

#pragma once

#include <ostream>
#include <streambuf>

namespace lvd {

// This implements a std::ostream which ignores all output.
class NullOstream : private std::streambuf, public std::ostream {
public:

    NullOstream ()
        :   std::streambuf()
        ,   std::ostream(static_cast<std::streambuf*>(this))
    { }

    int overflow (int c) { return c; }
};

// Global singleton NullOstream, accessible by anyone.
extern NullOstream nout;

} // end namespace lvd
