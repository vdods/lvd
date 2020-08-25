// 2020.08.24 - Victor Dods

#include "lvd/Log.hpp"
#include "lvd/req.hpp"
#include "lvd/Test.hpp"
#include <sstream>

// To test out custom print to Log
struct HippoThing {
    int m;
    HippoThing (int m_) : m(m_) { }
};

inline lvd::Log &operator << (lvd::Log &out, HippoThing const &h)
{
    return out << "HippoThing{" << h.m << '}';
}

// For the above operator<< to work, the following needs a definition with value = true.
template <> struct lvd::HasCustomLogOutputOverload<HippoThing> { static constexpr bool value = true; };

// To test out custom print to Log
struct OstrichThing {
    int m;
    OstrichThing (int m_) : m(m_) { }
};

inline std::ostream &operator << (std::ostream &out, OstrichThing const &o)
{
    return out << "OstrichThing{" << o.m << '}';
}

LVD_REGISTER_TEST(300__Log__00, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    log << "blah\n" << lvd::IndentGuard() << "hippo\n";
    log << "thingy";
    LVD_REQ_EQ(out.str(), "blah\n    hippo\nthingy");
}));

LVD_REGISTER_TEST(300__Log__01, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    HippoThing h{123};
    log << h;
    LVD_REQ_EQ(out.str(), "HippoThing{123}");
}));

LVD_REGISTER_TEST(300__Log__02, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    OstrichThing o{123};
    log << o;
    LVD_REQ_EQ(out.str(), "OstrichThing{123}");
}));

LVD_REGISTER_TEST(300__Log__10__crt, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::crt() << message;
    LVD_REQ_EQ(out.str(), lvd::Log::CRT_PREFIX+message);
}));

LVD_REGISTER_TEST(300__Log__20__err, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::err() << message;
    LVD_REQ_EQ(out.str(), lvd::Log::ERR_PREFIX+message);
}));

LVD_REGISTER_TEST(300__Log__30__wrn, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::wrn() << message;
    LVD_REQ_EQ(out.str(), lvd::Log::WRN_PREFIX+message);
}));

LVD_REGISTER_TEST(300__Log__40__inf, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::inf() << message;
    LVD_REQ_EQ(out.str(), lvd::Log::INF_PREFIX+message);
}));

LVD_REGISTER_TEST(300__Log__50__dbg, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::dbg() << message;
    LVD_REQ_EQ(out.str(), lvd::Log::DBG_PREFIX+message);
}));

LVD_REGISTER_TEST(300__Log__60__trc, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::trc() << message;
    LVD_REQ_EQ(out.str(), lvd::Log::TRC_PREFIX+message);
}));

