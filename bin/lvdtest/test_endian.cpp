// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/endian.hpp"
#include "lvd/random.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include <random>

namespace lvd {

template <typename T_>
bool is_endian_symmetric (T_ const &x) {
    static_assert(sizeof(T_) == 1 || sizeof(T_) == 2 || sizeof(T_) == 4 || sizeof(T_) == 8);
    auto bytes = reinterpret_cast<std::byte const *>(&x);
    if constexpr (sizeof(T_) == 1) {
        return true;
    } else if constexpr (sizeof(T_) == 2) {
        return bytes[0] == bytes[1];
    } else if constexpr (sizeof(T_) == 4) {
        return bytes[0] == bytes[3] && bytes[1] == bytes[2];
    } else if constexpr (sizeof(T_) == 8) {
        return bytes[0] == bytes[7] && bytes[1] == bytes[6] && bytes[2] == bytes[5] && bytes[3] == bytes[4];
    }
}

// 322 must be >= 321 (which is random), since endian.hpp depends on that.
LVD_TEST_BEGIN(322__endian)
    test_log << Log::trc() << LVD_REFLECT(machine_is_big_endian()) << '\n';
    test_log << Log::trc() << LVD_REFLECT(machine_is_lil_endian()) << '\n';
    test_log << Log::trc() << LVD_REFLECT(machine_endianness()) << '\n';

    auto rng = std::mt19937{42};
    for (uint32_t i = 0; i < 0x10000; ++i) {
        auto x1 = make_random<uint8_t>(rng);
        auto x2 = make_random<uint16_t>(rng);
        auto x4 = make_random<uint32_t>(rng);
        auto x8 = make_random<uint64_t>(rng);

//         test_log << Log::trc() << LVD_REFLECT(x1) << ", " << LVD_REFLECT(swapped_byte_order_of(x1)) << '\n';
//         test_log << Log::trc() << LVD_REFLECT(x2) << ", " << LVD_REFLECT(swapped_byte_order_of(x2)) << '\n';
//         test_log << Log::trc() << LVD_REFLECT(x4) << ", " << LVD_REFLECT(swapped_byte_order_of(x4)) << '\n';
//         test_log << Log::trc() << LVD_REFLECT(x8) << ", " << LVD_REFLECT(swapped_byte_order_of(x8)) << '\n';

        if (is_endian_symmetric(x1))
            LVD_TEST_REQ_EQ(swapped_byte_order_of(x1), x1);
        else
            LVD_TEST_REQ_NEQ(swapped_byte_order_of(x1), x1);

        if (is_endian_symmetric(x2))
            LVD_TEST_REQ_EQ(swapped_byte_order_of(x2), x2);
        else
            LVD_TEST_REQ_NEQ(swapped_byte_order_of(x2), x2);

        if (is_endian_symmetric(x4))
            LVD_TEST_REQ_EQ(swapped_byte_order_of(x4), x4);
        else
            LVD_TEST_REQ_NEQ(swapped_byte_order_of(x4), x4);

        if (is_endian_symmetric(x8))
            LVD_TEST_REQ_EQ(swapped_byte_order_of(x8), x8);
        else
            LVD_TEST_REQ_NEQ(swapped_byte_order_of(x8), x8);
    }
LVD_TEST_END

} // end namespace lvd

