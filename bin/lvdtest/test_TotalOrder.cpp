// 2020.12.29 - Copyright Victor Dods - Licensed under Apache 2.0

#include <array>
#include "lvd/TotalOrder.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"

namespace lvd {

// TODO: Make a natural_total_order_test_case, where it checks natural_total_order(x,y) and natural_total_order(y,x)

LVD_TEST_BEGIN(328__TotalOrder__00__natural_total_order__00)
    LVD_TEST_REQ_EQ(natural_total_order(0, 0), TotalOrder::EQUAL);
    LVD_TEST_REQ_EQ(natural_total_order(-1, -1), TotalOrder::EQUAL);
    LVD_TEST_REQ_EQ(natural_total_order(3, 3), TotalOrder::EQUAL);

    LVD_TEST_REQ_EQ(natural_total_order(0, -1), TotalOrder::GREATER_THAN);
    LVD_TEST_REQ_EQ(natural_total_order(3, -1), TotalOrder::GREATER_THAN);
    LVD_TEST_REQ_EQ(natural_total_order(3, 0), TotalOrder::GREATER_THAN);

    LVD_TEST_REQ_EQ(natural_total_order(-1, 0), TotalOrder::LESS_THAN);
    LVD_TEST_REQ_EQ(natural_total_order(-1, 3), TotalOrder::LESS_THAN);
    LVD_TEST_REQ_EQ(natural_total_order(0, 3), TotalOrder::LESS_THAN);
LVD_TEST_END

LVD_TEST_BEGIN(328__TotalOrder__00__natural_total_order__01)
    auto a0 = std::array<int,3>{4,6,8};
    auto a1 = std::array<int,3>{4,7,8};
    auto a2 = std::array<int,3>{3,7,8};
    auto a3 = std::array<int,2>{4,6};
    auto a4 = std::array<int,2>{3,6};
    auto a5 = std::array<int,2>{4,7};
    auto a6 = std::array<int,0>{};

    LVD_TEST_REQ_EQ(natural_total_order(a0, a0), TotalOrder::EQUAL);
    LVD_TEST_REQ_EQ(natural_total_order(a0, a1), TotalOrder::LESS_THAN);
    LVD_TEST_REQ_EQ(natural_total_order(a0, a2), TotalOrder::GREATER_THAN);

    LVD_TEST_REQ_EQ(natural_total_order(a1, a0), TotalOrder::GREATER_THAN);
    LVD_TEST_REQ_EQ(natural_total_order(a1, a1), TotalOrder::EQUAL);
    LVD_TEST_REQ_EQ(natural_total_order(a1, a2), TotalOrder::GREATER_THAN);

    LVD_TEST_REQ_EQ(natural_total_order(a2, a0), TotalOrder::LESS_THAN);
    LVD_TEST_REQ_EQ(natural_total_order(a2, a1), TotalOrder::LESS_THAN);
    LVD_TEST_REQ_EQ(natural_total_order(a2, a2), TotalOrder::EQUAL);

    LVD_TEST_REQ_EQ(natural_total_order(a3, a3), TotalOrder::EQUAL);
    LVD_TEST_REQ_EQ(natural_total_order(a3, a4), TotalOrder::GREATER_THAN);
    LVD_TEST_REQ_EQ(natural_total_order(a3, a5), TotalOrder::LESS_THAN);

    LVD_TEST_REQ_EQ(natural_total_order(a4, a3), TotalOrder::LESS_THAN);
    LVD_TEST_REQ_EQ(natural_total_order(a4, a4), TotalOrder::EQUAL);
    LVD_TEST_REQ_EQ(natural_total_order(a4, a5), TotalOrder::LESS_THAN);

    LVD_TEST_REQ_EQ(natural_total_order(a5, a3), TotalOrder::GREATER_THAN);
    LVD_TEST_REQ_EQ(natural_total_order(a5, a4), TotalOrder::GREATER_THAN);
    LVD_TEST_REQ_EQ(natural_total_order(a5, a5), TotalOrder::EQUAL);

    LVD_TEST_REQ_EQ(natural_total_order(a6, a6), TotalOrder::EQUAL);
LVD_TEST_END

} // end namespace lvd
