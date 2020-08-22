// 2020.03.13 - Victor Dods

#pragma once

namespace lvd {

//
// Convenience macros for formatting strings.
//

#define LVD_REFLECT(x) #x << " = " << (x)
#define LVD_FMT(expr) static_cast<std::ostringstream &>(std::ostringstream().flush() << expr).str()
#define LVD_LOG_FMT(expr) static_cast<std::ostringstream &>((lvd::Log(std::ostringstream().flush()).as_reference() << expr).out()).str()

} // end namespace lvd
