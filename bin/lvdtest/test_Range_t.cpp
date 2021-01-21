// 2020.09.21 - Copyright Victor Dods - Licensed under Apache 2.0

#include <iterator>
#include "lvd/Range_t.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include <sstream>
#include <string>

LVD_TEST_BEGIN(320__Range_t__000)
    std::ostringstream out;
    std::string s("HIPPO");
    for (auto c : lvd::Range_t<std::string::iterator>(s))
        out << c << ',';
    LVD_TEST_REQ_EQ(out.str(), "H,I,P,P,O,");
LVD_TEST_END

LVD_TEST_BEGIN(320__Range_t__001)
    std::ostringstream out;
    std::string s("HIPPO");
    for (auto c : lvd::Range_t<std::string::iterator>(s.begin(), s.end()))
        out << c << ',';
    LVD_TEST_REQ_EQ(out.str(), "H,I,P,P,O,");
LVD_TEST_END

LVD_TEST_BEGIN(320__Range_t__002)
    std::ostringstream out;
    std::string s("HIPPO");
    for (auto c : lvd::Range_t<std::string::iterator>(s.begin()+1, s.end()-1))
        out << c << ',';
    LVD_TEST_REQ_EQ(out.str(), "I,P,P,");
LVD_TEST_END

LVD_TEST_BEGIN(320__Range_t__003)
    std::ostringstream out;
    std::string s("HIPPO");
    auto p = std::pair(s.begin()+1, s.end()-1);
    for (auto c : lvd::Range_t<std::string::iterator>(p))
        out << c << ',';
    LVD_TEST_REQ_EQ(out.str(), "I,P,P,");
LVD_TEST_END

LVD_TEST_BEGIN(320__Range_t__010)
    std::ostringstream out;
    std::string s("HIPPO");
    for (auto c : lvd::range(s))
        out << c << ',';
    LVD_TEST_REQ_EQ(out.str(), "H,I,P,P,O,");
LVD_TEST_END

LVD_TEST_BEGIN(320__Range_t__011)
    std::ostringstream out;
    std::string s("HIPPO");
    for (auto c : lvd::range(s.begin(), s.end()))
        out << c << ',';
    LVD_TEST_REQ_EQ(out.str(), "H,I,P,P,O,");
LVD_TEST_END

LVD_TEST_BEGIN(320__Range_t__012)
    std::ostringstream out;
    std::string s("HIPPO");
    for (auto c : lvd::range(std::pair(s.begin(), s.end())))
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

LVD_TEST_BEGIN(320__Range_t__020)
    std::string s("HIPPO");
    auto range_on_s = [&](size_t begin_index, size_t end_index){
        LVD_TEST_REQ_LEQ(begin_index, s.size());
        LVD_TEST_REQ_LEQ(end_index, s.size());
        return lvd::range(s.begin()+begin_index, s.begin()+end_index);
    };

    LVD_TEST_REQ_EQ(range_on_s(0,0), range_on_s(0,0));
    LVD_TEST_REQ_LEQ(range_on_s(0,0), range_on_s(0,0));

    LVD_TEST_REQ_LEQ(range_on_s(0,0), range_on_s(0,1));
    LVD_TEST_REQ_LT(range_on_s(0,0), range_on_s(0,1));

    LVD_TEST_REQ_LEQ(range_on_s(0,1), range_on_s(1,1));
    LVD_TEST_REQ_LT(range_on_s(0,1), range_on_s(1,1));

    LVD_TEST_REQ_LEQ(range_on_s(0,1), range_on_s(1,2));
    LVD_TEST_REQ_LT(range_on_s(0,1), range_on_s(1,2));

    LVD_TEST_REQ_LEQ(range_on_s(0,1), range_on_s(2,3));
    LVD_TEST_REQ_LT(range_on_s(0,1), range_on_s(2,3));

    LVD_TEST_REQ_NEQ(range_on_s(0,2), range_on_s(1,3));
LVD_TEST_END

LVD_TEST_BEGIN(320__Range_t__021)
    std::string s("HIPPOS AND HIPPAS");
    auto range_on_s = [&](size_t begin_index, size_t end_index){
        LVD_TEST_REQ_LEQ(begin_index, s.size());
        LVD_TEST_REQ_LEQ(end_index, s.size());
        return lvd::range(s.begin()+begin_index, s.begin()+end_index);
    };

    LVD_TEST_REQ_EQ(range_on_s(0,0).unioned_with(range_on_s(0,0)), range_on_s(0,0));

    LVD_TEST_REQ_EQ(range_on_s(0,1).unioned_with(range_on_s(0,0)), range_on_s(0,1));
    LVD_TEST_REQ_EQ(range_on_s(1,1).unioned_with(range_on_s(0,0)), range_on_s(0,1));
    // This is a half-hearted test of negatively oriented Range_t
    LVD_TEST_REQ_EQ(range_on_s(1,0).unioned_with(range_on_s(0,0)), range_on_s(0,0));

    LVD_TEST_REQ_EQ(range_on_s(0,0).unioned_with(range_on_s(0,1)), range_on_s(0,1));
    LVD_TEST_REQ_EQ(range_on_s(0,0).unioned_with(range_on_s(1,1)), range_on_s(0,1));
    LVD_TEST_REQ_EQ(range_on_s(0,0).unioned_with(range_on_s(1,0)), range_on_s(0,0));

    LVD_TEST_REQ_EQ(range_on_s(0,0).intersectioned_with(range_on_s(0,0)), range_on_s(0,0));

    LVD_TEST_REQ_EQ(range_on_s(0,1).intersectioned_with(range_on_s(0,0)), range_on_s(0,0));
    LVD_TEST_REQ_EQ(range_on_s(1,1).intersectioned_with(range_on_s(0,0)), range_on_s(0,0));
    // This is a half-hearted test of negatively oriented Range_t
    LVD_TEST_REQ_EQ(range_on_s(1,0).intersectioned_with(range_on_s(0,0)), range_on_s(0,0));

    LVD_TEST_REQ_EQ(range_on_s(0,0).intersectioned_with(range_on_s(0,1)), range_on_s(0,0));
    LVD_TEST_REQ_EQ(range_on_s(0,0).intersectioned_with(range_on_s(1,1)), range_on_s(0,0));
    LVD_TEST_REQ_EQ(range_on_s(0,0).intersectioned_with(range_on_s(1,0)), range_on_s(0,0));

    LVD_TEST_REQ_EQ(range_on_s(2,10).unioned_with(range_on_s(2,10)), range_on_s(2,10));
    LVD_TEST_REQ_EQ(range_on_s(2,10).unioned_with(range_on_s(3,9)), range_on_s(2,10));
    LVD_TEST_REQ_EQ(range_on_s(2,10).unioned_with(range_on_s(1,9)), range_on_s(1,10));
    LVD_TEST_REQ_EQ(range_on_s(2,10).unioned_with(range_on_s(3,11)), range_on_s(2,11));
    LVD_TEST_REQ_EQ(range_on_s(2,10).unioned_with(range_on_s(0,0)), range_on_s(0,10));
    LVD_TEST_REQ_EQ(range_on_s(0,0).unioned_with(range_on_s(1,9)), range_on_s(0,9));
    LVD_TEST_REQ_EQ(range_on_s(2,4).unioned_with(range_on_s(3,5)), range_on_s(2,5));
    LVD_TEST_REQ_EQ(range_on_s(2,4).unioned_with(range_on_s(4,6)), range_on_s(2,6));
    LVD_TEST_REQ_EQ(range_on_s(2,4).unioned_with(range_on_s(5,7)), range_on_s(2,7));

    LVD_TEST_REQ_EQ(range_on_s(2,10).intersectioned_with(range_on_s(2,10)), range_on_s(2,10));
    LVD_TEST_REQ_EQ(range_on_s(2,10).intersectioned_with(range_on_s(3,9)), range_on_s(3,9));
    LVD_TEST_REQ_EQ(range_on_s(2,10).intersectioned_with(range_on_s(1,9)), range_on_s(2,9));
    LVD_TEST_REQ_EQ(range_on_s(2,10).intersectioned_with(range_on_s(3,11)), range_on_s(3,10));
    LVD_TEST_REQ_EQ(range_on_s(2,10).intersectioned_with(range_on_s(0,0)), range_on_s(0,0));
    LVD_TEST_REQ_EQ(range_on_s(0,0).intersectioned_with(range_on_s(1,9)), range_on_s(0,0));
    LVD_TEST_REQ_EQ(range_on_s(2,4).intersectioned_with(range_on_s(3,5)), range_on_s(3,4));
    LVD_TEST_REQ_EQ(range_on_s(2,4).intersectioned_with(range_on_s(4,6)), range_on_s(4,4));
    LVD_TEST_REQ_EQ(range_on_s(2,4).intersectioned_with(range_on_s(5,7)), range_on_s(4,4));
LVD_TEST_END

inline void fancy_function (auto &&r) {
    static_assert(lvd::is_Range_t<std::decay_t<decltype(r)>>);
}

LVD_TEST_BEGIN(320__Range_t__03)
    LVD_TEST_REQ_IS_TRUE(lvd::is_Range_t<lvd::Range_t<char*>>);
    LVD_TEST_REQ_IS_TRUE(lvd::is_Range_t<lvd::Range_t<int>>);
    LVD_TEST_REQ_IS_TRUE(!lvd::is_Range_t<int>);
    fancy_function(lvd::range(1,10));
LVD_TEST_END
