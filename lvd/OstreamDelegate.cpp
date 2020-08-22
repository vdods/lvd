// 2020.03.30 - Victor Dods

#include "lvd/OstreamDelegate.hpp"

namespace lvd {

OstreamDelegate::AnyPrinter OstreamDelegate::ms_any_printer = OstreamDelegate::default_any_printer;

} // end namespace lvd
