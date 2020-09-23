// 2020.03.29 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <any>
#include <functional>
#include <ostream>

namespace lvd {

// Not sure why I didn't think of this years ago.  If you provide a cast operator
// method `operator lvd::OstreamDelegate` for your class which returns something
// of the form
//
//     lvd::OstreamDelegate::OutFunc([this](std::ostream &out) {
//         <print this to out>
//     }
//
// then you don't have to bother overloading the global operator<< function to print
// your class, as OstreamDelegate has its own overload defined.
//
// Note that OstreamDelegate has to handle printing of std::any itself, but that
// printing functionality can be customized (unfortunately using a static variable)
// by calling the static function OstreamDelegate::set_any_printer.  I tried using
// a template parameter to specify the AnyPrinter function, but then the automatic
// conversion of std::any to OstreamDelegate (which is necessary for handling printing
// of std::any) didn't work.
class OstreamDelegate {
public:

    // Don't bother with returning anything, OstreamDelegate's operator<< overload
    // will return the stream being printed to, just as expected.
    using OutFunc = std::function<void(std::ostream&)>;
    // Use this for overriding the way that OstreamDelegate will print std::any.
    // NOTE: This printer is stored statically, meaning that the built-in std::any
    // printing facility can't have separate instances of customization.
    using AnyPrinter = std::function<void(std::ostream &out, std::any const &a)>;

    OstreamDelegate () = delete;
    OstreamDelegate (OstreamDelegate const &other) : m_out_func(other.m_out_func) { }
    OstreamDelegate (OstreamDelegate &&other) : m_out_func(std::move(other.m_out_func)) { }
    OstreamDelegate (OutFunc const &out_func) : m_out_func(out_func) { }
    OstreamDelegate (OutFunc &&out_func) : m_out_func(std::move(out_func)) { }
    OstreamDelegate (std::any const &a) : m_out_func([a](std::ostream &out){ ms_any_printer(out, a); }) { }

    OutFunc const &out_func () const { return m_out_func; }

    static AnyPrinter get_any_printer () { return ms_any_printer; }
    static void set_any_printer (AnyPrinter const &ap) { ms_any_printer = ap; }
    static void default_any_printer (std::ostream &out, std::any const &a) {
        if (!a.has_value())
            out << "any<novalue>";
        else
            out << "any<type=" << a.type().name() << '>';
    }

private:

    OutFunc m_out_func;

    static AnyPrinter ms_any_printer;
};

inline std::ostream &operator << (std::ostream &out, OstreamDelegate const &od) {
    od.out_func()(out);
    return out;
}

} // end namespace lvd

// If this is not done, then classes declared outside the lvd namespace which provide an
// `operator lvd::OstreamDelegate` method won't have the correct operator<< overload available.
using lvd::operator<<;
