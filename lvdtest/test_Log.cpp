// 2020.08.24 - Victor Dods

#include "lvd/Log.hpp"
#include "lvd/req.hpp"
#include "lvd/Test.hpp"
#include <sstream>

LVD_REGISTER_TEST(300__Log__0, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    log << "blah\n" << lvd::IndentGuard() << "hippo\n";
    log << "thingy";
    LVD_REQ_EQ(out.str(), "blah\n    hippo\nthingy");
}));

LVD_REGISTER_TEST(300__Log__1__crt, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::crt() << message;
    LVD_REQ_EQ(out.str(), lvd::Log::CRT_PREFIX+message);
}));

LVD_REGISTER_TEST(300__Log__2__err, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::err() << message;
    LVD_REQ_EQ(out.str(), lvd::Log::ERR_PREFIX+message);
}));

LVD_REGISTER_TEST(300__Log__3__wrn, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::wrn() << message;
    LVD_REQ_EQ(out.str(), lvd::Log::WRN_PREFIX+message);
}));

LVD_REGISTER_TEST(300__Log__4__inf, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::inf() << message;
    LVD_REQ_EQ(out.str(), lvd::Log::INF_PREFIX+message);
}));

LVD_REGISTER_TEST(300__Log__5__dbg, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::dbg() << message;
    LVD_REQ_EQ(out.str(), lvd::Log::DBG_PREFIX+message);
}));

LVD_REGISTER_TEST(300__Log__6__trc, ([](){
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::trc() << message;
    LVD_REQ_EQ(out.str(), lvd::Log::TRC_PREFIX+message);
}));

