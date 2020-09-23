// 2020.03.30 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/OstreamDelegate.hpp"

namespace lvd {

OstreamDelegate::AnyPrinter OstreamDelegate::ms_any_printer = OstreamDelegate::default_any_printer;

} // end namespace lvd
