// 2021.01.10 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include "lvd/variant.hpp"

LVD_TEST_BEGIN(020__variant__00)
    LVD_TEST_REQ_EQ(std::variant_npos, size_t(-1));

    LVD_TEST_REQ_EQ(lvd::variant_type_index<uint8_t>(lvd::Hypothetical_t<std::variant<uint8_t,uint16_t,uint32_t>>()), size_t(0));
    LVD_TEST_REQ_EQ(lvd::variant_type_index<uint16_t>(lvd::Hypothetical_t<std::variant<uint8_t,uint16_t,uint32_t>>()), size_t(1));
    LVD_TEST_REQ_EQ(lvd::variant_type_index<uint32_t>(lvd::Hypothetical_t<std::variant<uint8_t,uint16_t,uint32_t>>()), size_t(2));
    LVD_TEST_REQ_EQ(lvd::variant_type_index<uint64_t>(lvd::Hypothetical_t<std::variant<uint8_t,uint16_t,uint32_t>>()), std::variant_npos);
LVD_TEST_END

LVD_TEST_BEGIN(020__variant__01)
    LVD_TEST_REQ_EQ(LVD_GET_32TH_VARIADIC_ARGUMENT(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33), 32);
    LVD_TEST_REQ_EQ(LVD_GET_32TH_VARIADIC_ARGUMENT(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32), 32);

    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31, a32), 32);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30, a31), 31);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29, a30), 30);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28, a29), 29);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27, a28), 28);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26, a27), 27);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25, a26), 26);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24, a25), 25);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23, a24), 24);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22, a23), 23);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21, a22), 22);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20, a21), 21);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19, a20), 20);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18, a19), 19);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17, a18), 18);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16, a17), 17);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15, a16), 16);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14, a15), 15);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13, a14), 14);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12, a13), 13);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12), 12);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11), 11);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9, a10), 10);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8, a9), 9);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7, a8), 8);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6, a7), 7);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5, a6), 6);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4, a5), 5);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3, a4), 4);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2, a3), 3);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1, a2), 2);
    LVD_TEST_REQ_EQ(LVD_COUNT_VARIADIC_ARGUMENTS(a1), 1);
LVD_TEST_END

LVD_TEST_BEGIN(020__variant__02)
    using Variant = std::variant<int,float>;
    int expected_int = 3;
    float expected_float = 3.45f;
    Variant vnt = expected_int;
    std::string result;

    // Testing without comma after default code handler argument
    result.clear();
    LVD_HANDLE_VARIANT_WITH_DEFAULT(
        vnt, value,
        default, { result = "default"; }
    )
    LVD_TEST_REQ_EQ(result, "default");

    // Testing with comma after.
    result.clear();
    LVD_HANDLE_VARIANT_WITH_DEFAULT(
        vnt, value,
        default, { result = "default"; },
    )
    LVD_TEST_REQ_EQ(result, "default");

    // Testing without comma after.
    result.clear();
    LVD_HANDLE_VARIANT_WITH_DEFAULT(
        vnt, value,
        default, { result = "default"; },
        int, {
            result = "int";
            LVD_TEST_REQ_EQ(value, expected_int);
        },
        float, { result = "float"; }
    )
    LVD_TEST_REQ_EQ(result, "int");

    // Testing with comma after.
    result.clear();
    LVD_HANDLE_VARIANT_WITH_DEFAULT(
        vnt, value,
        default, { result = "default"; },
        int, {
            result = "int";
            LVD_TEST_REQ_EQ(value, expected_int);
        },
        float, { result = "float"; },
    )
    LVD_TEST_REQ_EQ(result, "int");

    result.clear();
    LVD_HANDLE_VARIANT_WITH_DEFAULT(
        vnt, value,
        default, { result = "default"; },
        float, { result = "float"; },
    )
    LVD_TEST_REQ_EQ(result, "default");

    result.clear();
    LVD_HANDLE_VARIANT_WITH_DEFAULT(
        vnt, value,
        default, { result = "default"; },
        int, {
            result = "int";
            LVD_TEST_REQ_EQ(value, expected_int);
        },
    )
    LVD_TEST_REQ_EQ(result, "int");

    // Uncommenting this should result in an error about duplicate case value (two int cases)
//     LVD_HANDLE_VARIANT_WITH_DEFAULT(
//         vnt, value,
//         default, { },
//         float, { },
//         int, { },
//         int, { },
//     )

    // Uncommenting this should result in an error about duplicate case value (std::string isn't part of the std::variant types)
//     LVD_HANDLE_VARIANT_WITH_DEFAULT(
//         vnt, value,
//         default, { },
//         float, { },
//         int, { },
//         std::string, { }
//     )

    // Change vnt to carry a float.
    vnt = expected_float;

    result.clear();
    LVD_HANDLE_VARIANT_WITH_DEFAULT(
        vnt, value,
        default, { result = "default"; },
    )
    LVD_TEST_REQ_EQ(result, "default");

    result.clear();
    LVD_HANDLE_VARIANT_WITH_DEFAULT(
        vnt, value,
        default, { result = "default"; },
        int, { result = "int"; },
        float, {
            result = "float";
            LVD_TEST_REQ_EQ(value, expected_float);
        },
    )
    LVD_TEST_REQ_EQ(result, "float");

    result.clear();
    LVD_HANDLE_VARIANT_WITH_DEFAULT(
        vnt, value,
        default, { result = "default"; },
        int, { result = "int"; },
    )
    LVD_TEST_REQ_EQ(result, "default");

    // Different order.
    result.clear();
    LVD_HANDLE_VARIANT_WITH_DEFAULT(
        vnt, value,
        default, { result = "default"; },
        float, {
            result = "float";
            LVD_TEST_REQ_EQ(value, expected_float);
        },
        int, { result = "int"; },
    )
    LVD_TEST_REQ_EQ(result, "float");
LVD_TEST_END

LVD_TEST_BEGIN(020__variant__03)
    using Variant = std::variant<int,float>;
    int expected_int = 3;
    float expected_float = 3.45f;
    Variant vnt = expected_int;
    std::string result;

    // Testing without comma after default code handler argument
    result.clear();
    LVD_HANDLE_VARIANT(
        vnt, value
    )
    LVD_TEST_REQ_EQ(result, "");

    // Testing with comma after.
    result.clear();
    LVD_HANDLE_VARIANT(
        vnt, value,
    )
    LVD_TEST_REQ_EQ(result, "");

    // Testing without comma after.
    result.clear();
    LVD_HANDLE_VARIANT(
        vnt, value,
        int, {
            result = "int";
            LVD_TEST_REQ_EQ(value, expected_int);
        },
        float, { result = "float"; }
    )
    LVD_TEST_REQ_EQ(result, "int");

    // Testing with comma after.
    result.clear();
    LVD_HANDLE_VARIANT(
        vnt, value,
        int, {
            result = "int";
            LVD_TEST_REQ_EQ(value, expected_int);
        },
        float, { result = "float"; },
    )
    LVD_TEST_REQ_EQ(result, "int");

    result.clear();
    LVD_HANDLE_VARIANT(
        vnt, value,
        float, { result = "float"; },
    )
    LVD_TEST_REQ_EQ(result, "");

    result.clear();
    LVD_HANDLE_VARIANT(
        vnt, value,
        int, {
            result = "int";
            LVD_TEST_REQ_EQ(value, expected_int);
        },
    )
    LVD_TEST_REQ_EQ(result, "int");

    // Uncommenting this should result in an error about duplicate case value (two int cases)
//     LVD_HANDLE_VARIANT(
//         vnt, value,
//         float, { },
//         int, { },
//         int, { },
//     )

    // Uncommenting this should result in an error about duplicate case value (std::string isn't part of the std::variant types)
//     LVD_HANDLE_VARIANT(
//         vnt, value,
//         float, { },
//         int, { },
//         std::string, { }
//     )

    // Change vnt to carry a float.
    vnt = expected_float;

    result.clear();
    LVD_HANDLE_VARIANT(
        vnt, value,
    )
    LVD_TEST_REQ_EQ(result, "");

    result.clear();
    LVD_HANDLE_VARIANT(
        vnt, value,
        int, { result = "int"; },
        float, {
            result = "float";
            LVD_TEST_REQ_EQ(value, expected_float);
        },
    )
    LVD_TEST_REQ_EQ(result, "float");

    result.clear();
    LVD_HANDLE_VARIANT(
        vnt, value,
        int, { result = "int"; },
    )
    LVD_TEST_REQ_EQ(result, "");

    // Different order.
    result.clear();
    LVD_HANDLE_VARIANT(
        vnt, value,
        float, {
            result = "float";
            LVD_TEST_REQ_EQ(value, expected_float);
        },
        int, { result = "int"; },
    )
    LVD_TEST_REQ_EQ(result, "float");
LVD_TEST_END

