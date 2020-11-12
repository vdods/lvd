// 2020.09.21 - Copyright Victor Dods - Licensed under Apache 2.0

#include <iterator>
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

namespace lvd {

// This is needed only so that LVD_TEST_REQ_* can generate failure messages.  It could just as easily
// be a dummy output function.  The real solution is to have LVD_TEST_REQ_* allow types that have no
// output functions.
template <typename Iterator_>
std::ostream &operator << (std::ostream &out, Range_t<Iterator_> const &r) {
    std::string s;
    std::copy(r.begin(), r.end(), std::back_insert_iterator(s));
    return out << "Range_t(" << lvd::literal_of(s) << ')';
}

} // end namespace lvd

LVD_TEST_BEGIN(320__Range_t__04)
    std::ostringstream out;
    std::string s("HIPPO");
    LVD_TEST_REQ_EQ(lvd::range(s.begin()+0, s.begin()+0), lvd::range(s.begin()+0, s.begin()+0));
    LVD_TEST_REQ_LEQ(lvd::range(s.begin()+0, s.begin()+0), lvd::range(s.begin()+0, s.begin()+0));

    LVD_TEST_REQ_LEQ(lvd::range(s.begin()+0, s.begin()+0), lvd::range(s.begin()+0, s.begin()+1));
    LVD_TEST_REQ_LT(lvd::range(s.begin()+0, s.begin()+0), lvd::range(s.begin()+0, s.begin()+1));

    LVD_TEST_REQ_LEQ(lvd::range(s.begin()+0, s.begin()+1), lvd::range(s.begin()+1, s.begin()+1));
    LVD_TEST_REQ_LT(lvd::range(s.begin()+0, s.begin()+1), lvd::range(s.begin()+1, s.begin()+1));

    LVD_TEST_REQ_LEQ(lvd::range(s.begin()+0, s.begin()+1), lvd::range(s.begin()+1, s.begin()+2));
    LVD_TEST_REQ_LT(lvd::range(s.begin()+0, s.begin()+1), lvd::range(s.begin()+1, s.begin()+2));

    LVD_TEST_REQ_LEQ(lvd::range(s.begin()+0, s.begin()+1), lvd::range(s.begin()+2, s.begin()+3));
    LVD_TEST_REQ_LT(lvd::range(s.begin()+0, s.begin()+1), lvd::range(s.begin()+2, s.begin()+3));

    LVD_TEST_REQ_NEQ(lvd::range(s.begin()+0, s.begin()+2), lvd::range(s.begin()+1, s.begin()+3));
LVD_TEST_END
