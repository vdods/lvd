// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#include <cstdint>
#include "lvd/random.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include <map>
#include "print.hpp"
#include <random>
#include <set>
#include <string>

namespace lvd {

LVD_TEST_BEGIN(321__random__00)
    std::set<uint8_t> missed;
    for (uint32_t i = 0; i < 0x100; ++i)
        missed.insert(uint8_t(i));

    auto rng = std::mt19937{42};

    uint32_t i;
    for (i = 0; i < 0x10000 && !missed.empty(); ++i)
        missed.erase(make_random<uint8_t>(rng));
    test_log << Log::trc() << "finished generating all values after " << i << " iterations\n";

    LVD_TEST_REQ_IS_TRUE(missed.empty());
LVD_TEST_END

LVD_TEST_BEGIN(321__random__01__is_valid)
    auto is_valid = [](uint8_t x){ return x % 2 == 0; };

    std::set<uint8_t> missed;
    for (uint32_t i = 0; i < 0x100; ++i)
        if (is_valid(i))
            missed.insert(uint8_t(i));

    auto rng = std::mt19937{42};

    uint32_t i;
    for (i = 0; i < 0x10000 && !missed.empty(); ++i) {
        auto x = make_random<uint8_t>(rng, is_valid);
        LVD_TEST_REQ_IS_TRUE(is_valid(x));
        missed.erase(x);
    }
    test_log << Log::trc() << "finished generating all values after " << i << " iterations\n";

    LVD_TEST_REQ_IS_TRUE(missed.empty());
LVD_TEST_END

LVD_TEST_BEGIN(321__random__02)
    auto rng = std::mt19937{42};
    for (auto i = 0; i < 0x100; ++i) {
        test_log << Log::trc() << make_random<std::string>(rng) << '\n';
    }
LVD_TEST_END

LVD_TEST_BEGIN(321__random__03)
    auto rng = std::mt19937{42};
    for (auto i = 0; i < 0x100; ++i) {
        test_log << Log::trc() << make_random<std::vector<std::string>>(rng) << '\n';
    }
LVD_TEST_END

LVD_TEST_BEGIN(321__random__04)
    auto rng = std::mt19937{42};
    for (auto i = 0; i < 0x100; ++i) {
        test_log << Log::trc() << make_random<std::pair<uint32_t,std::string>>(rng) << '\n';
    }
LVD_TEST_END

LVD_TEST_BEGIN(321__random__05)
    auto rng = std::mt19937{42};
    for (auto i = 0; i < 0x100; ++i) {
        test_log << Log::trc() << make_random<std::map<uint32_t,std::string>>(rng) << '\n';
    }
LVD_TEST_END

LVD_TEST_BEGIN(321__random__06)
    auto rng = std::mt19937{42};
    for (auto i = 0; i < 0x100; ++i) {
        test_log << Log::trc() << make_random<std::unordered_map<uint32_t,std::string>>(rng) << '\n';
    }
LVD_TEST_END

LVD_TEST_BEGIN(321__random__07)
    auto rng = std::mt19937{42};
    for (auto i = 0; i < 0x100; ++i) {
        test_log << Log::trc() << make_random<std::set<uint32_t>>(rng) << '\n';
    }
LVD_TEST_END

LVD_TEST_BEGIN(321__random__08)
    auto rng = std::mt19937{42};
    for (auto i = 0; i < 0x100; ++i) {
        test_log << Log::trc() << make_random<std::unordered_set<uint32_t>>(rng) << '\n';
    }
LVD_TEST_END

LVD_TEST_BEGIN(321__random__09)
    auto rng = std::mt19937{42};
    for (auto i = 0; i < 0x100; ++i) {
        test_log << Log::trc() << make_random<std::vector<std::map<uint32_t,std::string>>>(rng) << '\n';
    }
LVD_TEST_END

} // end namespace lvd
