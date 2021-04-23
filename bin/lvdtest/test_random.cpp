// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#include <cstdint>
#include "DerivedString_random.hpp"
#include "lvd/random_array.hpp"
#include "lvd/random_map.hpp"
#include "lvd/random_optional.hpp"
#include "lvd/random_pair.hpp"
#include "lvd/random_set.hpp"
#include "lvd/random_unordered_map.hpp"
#include "lvd/random_unordered_set.hpp"
#include "lvd/random_variant.hpp"
#include "lvd/random_vector.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include "print.hpp"

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

template <typename T_>
void test_random_generation_for_type (req::Context &req_context) {
    auto rng = std::mt19937{42};
    for (auto i = 0; i < 0x100; ++i) {
        req_context.log() << Log::trc() << make_random<T_>(rng) << '\n';
    }
    if constexpr (std::is_default_constructible_v<T_>) {
        for (auto i = 0; i < 0x100; ++i) {
            T_ dest;
            populate_random(dest, rng);
            req_context.log() << Log::trc() << dest << '\n';
        }
    }
}

LVD_TEST_BEGIN(321__random__010)
    test_random_generation_for_type<bool>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__011)
    test_random_generation_for_type<std::byte>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__0120)
    test_random_generation_for_type<uint8_t>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__0121)
    test_random_generation_for_type<int8_t>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__0130)
    test_random_generation_for_type<uint16_t>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__0131)
    test_random_generation_for_type<int16_t>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__0140)
    test_random_generation_for_type<uint32_t>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__0141)
    test_random_generation_for_type<int32_t>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__0150)
    test_random_generation_for_type<uint64_t>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__0151)
    test_random_generation_for_type<int64_t>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__0160)
    test_random_generation_for_type<float>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__0161)
    test_random_generation_for_type<double>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__0162)
    test_random_generation_for_type<long double>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__02)
    test_random_generation_for_type<std::string>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__032)
    test_random_generation_for_type<std::vector<std::string>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__04)
    test_random_generation_for_type<std::pair<uint32_t,std::string>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__05)
    test_random_generation_for_type<std::map<uint32_t,std::string>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__06)
    test_random_generation_for_type<std::unordered_map<uint32_t,std::string>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__07)
    test_random_generation_for_type<std::set<uint32_t>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__08)
    test_random_generation_for_type<std::unordered_set<uint32_t>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__035)
    test_random_generation_for_type<std::vector<std::map<uint32_t,std::string>>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__080)
    test_random_generation_for_type<std::optional<bool>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__081)
    test_random_generation_for_type<std::optional<uint8_t>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__082)
    test_random_generation_for_type<std::optional<std::string>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__090)
    test_random_generation_for_type<std::optional<std::optional<uint16_t>>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__091)
    test_random_generation_for_type<std::variant<float>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__092)
    test_random_generation_for_type<std::variant<float,uint32_t>>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__093)
    test_random_generation_for_type<std::variant<float,uint32_t,std::array<int16_t,3>>>(req_context);
LVD_TEST_END

//
// Testing generation of random values for types that don't have default constructors
//

LVD_TEST_BEGIN(321__random__10)
    test_random_generation_for_type<DerivedString_DC_IP>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__11)
    test_random_generation_for_type<DerivedString_DC_EP>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__12)
    test_random_generation_for_type<DerivedString_NDC_IP>(req_context);
LVD_TEST_END

LVD_TEST_BEGIN(321__random__13)
    test_random_generation_for_type<DerivedString_NDC_EP>(req_context);
LVD_TEST_END

} // end namespace lvd
