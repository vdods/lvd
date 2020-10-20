// 2020.10.19 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/FiPos.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"

LVD_TEST_BEGIN(220__FiPos)
    lvd::FiPos invalid;
    lvd::FiPos x(10, 1);
    lvd::FiPos y(10, 2);
    lvd::FiPos z(11, 1);

    LVD_TEST_REQ_LT(invalid, x);
    LVD_TEST_REQ_LT(invalid, y);
    LVD_TEST_REQ_LT(invalid, z);
    LVD_TEST_REQ_LT(x, y);
    LVD_TEST_REQ_LT(x, z);
    LVD_TEST_REQ_LT(y, z);

    LVD_TEST_REQ_LEQ(invalid, invalid);
    LVD_TEST_REQ_LEQ(invalid, x);
    LVD_TEST_REQ_LEQ(invalid, y);
    LVD_TEST_REQ_LEQ(invalid, z);
    LVD_TEST_REQ_LEQ(x, x);
    LVD_TEST_REQ_LEQ(x, y);
    LVD_TEST_REQ_LEQ(x, z);
    LVD_TEST_REQ_LEQ(y, y);
    LVD_TEST_REQ_LEQ(y, z);
    LVD_TEST_REQ_LEQ(z, z);

    LVD_TEST_REQ_EQ(invalid, invalid);
    LVD_TEST_REQ_EQ(x, x);
    LVD_TEST_REQ_EQ(y, y);
    LVD_TEST_REQ_EQ(z, z);

    LVD_TEST_REQ_IS_FALSE(invalid < invalid);
    LVD_TEST_REQ_IS_FALSE(x < invalid);
    LVD_TEST_REQ_IS_FALSE(y < invalid);
    LVD_TEST_REQ_IS_FALSE(z < invalid);
    LVD_TEST_REQ_IS_FALSE(x < x);
    LVD_TEST_REQ_IS_FALSE(y < x);
    LVD_TEST_REQ_IS_FALSE(z < x);
    LVD_TEST_REQ_IS_FALSE(y < y);
    LVD_TEST_REQ_IS_FALSE(z < y);
    LVD_TEST_REQ_IS_FALSE(z < z);

    LVD_TEST_REQ_IS_FALSE(x <= invalid);
    LVD_TEST_REQ_IS_FALSE(y <= invalid);
    LVD_TEST_REQ_IS_FALSE(z <= invalid);
    LVD_TEST_REQ_IS_FALSE(y <= x);
    LVD_TEST_REQ_IS_FALSE(z <= x);
    LVD_TEST_REQ_IS_FALSE(z <= y);
LVD_TEST_END
