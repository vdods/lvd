// 2020.12.29 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/PartialOrder.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"

namespace lvd {

LVD_TEST_BEGIN(329__PartialOrder__00)
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string(""), std::string("")), PartialOrder::EQUAL);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("a"), std::string("a")), PartialOrder::EQUAL);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("ab"), std::string("ab")), PartialOrder::EQUAL);

    LVD_TEST_REQ_EQ(prefix_partial_order(std::string(""), std::string("abc")), PartialOrder::LESS_THAN);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string(""), std::string("abcd")), PartialOrder::LESS_THAN);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("abc"), std::string("")), PartialOrder::GREATER_THAN);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("abcd"), std::string("")), PartialOrder::GREATER_THAN);

    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("a"), std::string("abc")), PartialOrder::LESS_THAN);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("a"), std::string("abcd")), PartialOrder::LESS_THAN);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("abc"), std::string("a")), PartialOrder::GREATER_THAN);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("abcd"), std::string("a")), PartialOrder::GREATER_THAN);

    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("ab"), std::string("abc")), PartialOrder::LESS_THAN);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("ab"), std::string("abcd")), PartialOrder::LESS_THAN);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("abc"), std::string("ab")), PartialOrder::GREATER_THAN);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("abcd"), std::string("ab")), PartialOrder::GREATER_THAN);

    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("aX"), std::string("ab")), PartialOrder::INCOMPARABLE);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("aXc"), std::string("abc")), PartialOrder::INCOMPARABLE);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("aXc"), std::string("abcd")), PartialOrder::INCOMPARABLE);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("aXcd"), std::string("abc")), PartialOrder::INCOMPARABLE);
    LVD_TEST_REQ_EQ(prefix_partial_order(std::string("aXcd"), std::string("abcd")), PartialOrder::INCOMPARABLE);
LVD_TEST_END

} // end namespace lvd
