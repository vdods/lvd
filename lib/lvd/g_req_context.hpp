// 2020.09.22 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/req.hpp"

namespace lvd {
namespace req {

extern Context g_req_context;

//
// These are analogs to LVD_REQ_* except that they use the lvd::req::g_req_context object.
// NOTE: `, ##__VA_ARGS__` is a g++ extension, but maybe it works for other compilers.
//

#define LVD_G_REQ_CONDITION_1PARAM(cond, param, ...) lvd::req::verify_condition_1param(lvd::req::g_req_context, cond, #cond, param, #param, LVD_CALL_SITE(), ##__VA_ARGS__)
#define LVD_G_REQ_CONDITION_2PARAM(cond, param0, param1, ...) lvd::req::verify_condition_2param(lvd::req::g_req_context, cond, #cond, param0, #param0, param1, #param1, LVD_CALL_SITE(), ##__VA_ARGS__)
#define LVD_G_REQ_CONDITION_3PARAM(cond, param0, param1, param2, ...) lvd::req::verify_condition_3param(lvd::req::g_req_context, cond, #cond, param0, #param0, param1, #param1, param2, #param2, LVD_CALL_SITE(), ##__VA_ARGS__)
#define LVD_G_REQ_CONDITION_4PARAM(cond, param0, param1, param2, param3, ...) lvd::req::verify_condition_4param(lvd::req::g_req_context, cond, #cond, param0, #param0, param1, #param1, param2, #param2, param3, #param3, LVD_CALL_SITE(), ##__VA_ARGS__)

#define LVD_G_REQ_IS_TRUE(param, ...) lvd::req::is_true(lvd::req::g_req_context, param, #param, LVD_CALL_SITE(), ##__VA_ARGS__)
#define LVD_G_REQ_IS_FALSE(param, ...) lvd::req::is_false(lvd::req::g_req_context, param, #param, LVD_CALL_SITE(), ##__VA_ARGS__)

#define LVD_G_REQ_EQ_NULLPTR(param, ...) lvd::req::eq_nullptr(lvd::req::g_req_context, param, #param, LVD_CALL_SITE(), ##__VA_ARGS__)
#define LVD_G_REQ_NEQ_NULLPTR(param, ...) lvd::req::neq_nullptr(lvd::req::g_req_context, param, #param, LVD_CALL_SITE(), ##__VA_ARGS__)

#define LVD_G_REQ_EQ(lhs, rhs, ...) lvd::req::eq(lvd::req::g_req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define LVD_G_REQ_NEQ(lhs, rhs, ...) lvd::req::neq(lvd::req::g_req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define LVD_G_REQ_LT(lhs, rhs, ...) lvd::req::lt(lvd::req::g_req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define LVD_G_REQ_LEQ(lhs, rhs, ...) lvd::req::leq(lvd::req::g_req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define LVD_G_REQ_GT(lhs, rhs, ...) lvd::req::gt(lvd::req::g_req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)
#define LVD_G_REQ_GEQ(lhs, rhs, ...) lvd::req::geq(lvd::req::g_req_context, lhs, rhs, #lhs, #rhs, LVD_CALL_SITE(), ##__VA_ARGS__)

} // end namespace req
} // end namespace lvd
