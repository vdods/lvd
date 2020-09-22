// 2020.09.21 - Victor Dods

#include "lvd/Range_t.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include <sstream>

LVD_TEST_BEGIN(320__Range_t__00)
    std::ostringstream out;
    std::string s("HIPPO");
    for (auto c : lvd::Range_t<std::string::iterator>(s))
        out << c << ',';
    LVD_TEST_REQ_EQ(out.str(), "H,I,P,P,O,");
LVD_TEST_END

LVD_TEST_BEGIN(320__Range_t__01)
    std::ostringstream out;
    std::string s("HIPPO");
    for (auto c : lvd::Range_t<std::string::iterator>(s.begin(), s.end()))
        out << c << ',';
    LVD_TEST_REQ_EQ(out.str(), "H,I,P,P,O,");
LVD_TEST_END

LVD_TEST_BEGIN(320__Range_t__010)
    std::ostringstream out;
    std::string s("HIPPO");
    for (auto c : lvd::Range_t<std::string::iterator>(s.begin()+1, s.end()-1))
        out << c << ',';
    LVD_TEST_REQ_EQ(out.str(), "I,P,P,");
LVD_TEST_END

LVD_TEST_BEGIN(320__Range_t__02)
    std::ostringstream out;
    std::string s("HIPPO");
    for (auto c : lvd::range(s))
        out << c << ',';
    LVD_TEST_REQ_EQ(out.str(), "H,I,P,P,O,");
LVD_TEST_END

LVD_TEST_BEGIN(320__Range_t__03)
    std::ostringstream out;
    std::string s("HIPPO");
    for (auto c : lvd::range(s.begin(), s.end()))
        out << c << ',';
    LVD_TEST_REQ_EQ(out.str(), "H,I,P,P,O,");
LVD_TEST_END
