// 2020.08.24 - Victor Dods

#include "lvd/Log.hpp"
#include "lvd/req.hpp"
#include "lvd/Test.hpp"
#include <sstream>

//
// All these *Thing classes and namespace stuff is to attempt to reproduce this really
// annoying compile error which happens when moving ostream operator<< code into a
// library and putting it inside a namespace.  In certain cases, this causes the overload
// resolution to freak out and not find the intended overload.
//

// To test out custom print to Log
struct HippoThing {
    int m;
    HippoThing (int m_) : m(m_) { }
};

inline lvd::Log &operator << (lvd::Log &out, HippoThing const &x)
{
    return out << "HippoThing{" << x.m << '}';
}

// For the above operator<< to work, the following needs a definition with value = true.
template <> struct lvd::HasCustomLogOutputOverload<HippoThing> : std::true_type { };

// To test out custom print to Log
struct OstrichThing {
    int m;
    OstrichThing (int m_) : m(m_) { }
};

inline std::ostream &operator << (std::ostream &out, OstrichThing const &x)
{
    return out << "OstrichThing{" << x.m << '}';
}

namespace aaa {

struct DonkeyThing {
    int m;
    DonkeyThing (int m_) : m(m_) { }
};

// Requires lvd::HasCustomLogOutputOverload<aaa::DonkeyThing> specialization.
inline lvd::Log &operator << (lvd::Log &out, DonkeyThing const &x)
{
    return out << "DonkeyThing[LogOverload]{" << x.m << '}';
}

inline std::ostream &operator << (std::ostream &out, DonkeyThing const &x) // Overload A
{
    return out << "DonkeyThing{" << x.m << '}';
}

} // end namespace aaa

// This is needed for the operator<< overload for Log to work.  This can't be inside a namespace.
template <> struct lvd::HasCustomLogOutputOverload<aaa::DonkeyThing> : std::true_type { };

// NOTE TO PROGRAMMER: This kind of overload (e.g. for aaa::DonkeyThing) HAS to go within the
// namespace in which the type is declared, which in this case is aaa.  In a `namespace aaa { ... }`
// block itself, and not just qualified with `aaa::`.  Thus if you were to comment out Overload A
// above and uncomment either Overload B or Overload C, it would fail to compile, and worse yet,
// the error that Overload C produces gives no hint as to what the problem is (the error for
// Overload B seems to give a direct explanation, at least in g++).
//
// inline std::ostream &aaa::operator << (std::ostream &out, DonkeyThing const &x) // Overload B
// {
//     return out << "DonkeyThing{" << x.m << '}';
// }
//
// inline std::ostream &operator << (std::ostream &out, aaa::DonkeyThing const &x) // Overload C
// {
//     return out << "DonkeyThing{" << x.m << '}';
// }

LVD_TEST_BEGIN(300__Log__00)
    std::ostringstream out;
    lvd::Log log(out);
    log << "blah\n" << lvd::IndentGuard() << "hippo\n";
    log << "thingy";
    LVD_TEST_REQ_EQ(out.str(), "blah\n    hippo\nthingy");
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__01)
    std::ostringstream out;
    lvd::Log log(out);
    HippoThing x{123};
    log << x;
    LVD_TEST_REQ_EQ(out.str(), "HippoThing{123}");
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__02)
    std::ostringstream out;
    lvd::Log log(out);
    OstrichThing x{123};
    log << x;
    LVD_TEST_REQ_EQ(out.str(), "OstrichThing{123}");
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__03)
    std::ostringstream out;
    aaa::DonkeyThing x{123};
    out << x;
    LVD_TEST_REQ_EQ(out.str(), "DonkeyThing{123}");
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__04)
    std::ostringstream out;
    lvd::Log log(out);
    aaa::DonkeyThing x{123};
    log << x;
    LVD_TEST_REQ_EQ(out.str(), "DonkeyThing[LogOverload]{123}");
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__nil)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::nil() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::NIL)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__trc)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::trc() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::TRC)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__dbg)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::dbg() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::DBG)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__inf)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::inf() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::INF)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__wrn)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::wrn() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::WRN)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__err)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::err() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::ERR)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__crt)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::crt() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::CRT)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__70__histogram)
    std::ostringstream out;
    lvd::Log log(out);
    log << lvd::Log::trc() << "blah\n";
    log << lvd::Log::dbg() << "thingy stuff\n";
    log << lvd::Log::dbg() << "uuustuff\n";
    log << lvd::Log::wrn() << "warning fancy face\n";
    LVD_TEST_REQ_EQ(log.log_level_histogram(), lvd::LogLevelHistogram(0,1,2,0,1,0,0));
LVD_TEST_END
