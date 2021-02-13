// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/comma.hpp"
#include "lvd/literal.hpp"
#include "lvd/random.hpp"
#include "lvd/read_bin_array.hpp"
#include "lvd/read_bin_map.hpp"
#include "lvd/read_bin_optional.hpp"
#include "lvd/read_bin_pair.hpp"
#include "lvd/read_bin_set.hpp"
#include "lvd/read_bin_string.hpp"
#include "lvd/read_bin_tuple.hpp"
#include "lvd/read_bin_type.hpp"
#include "lvd/read_bin_unordered_map.hpp"
#include "lvd/read_bin_unordered_set.hpp"
#include "lvd/read_bin_vector.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include "lvd/write_bin_array.hpp"
#include "lvd/write_bin_map.hpp"
#include "lvd/write_bin_optional.hpp"
#include "lvd/write_bin_pair.hpp"
#include "lvd/write_bin_set.hpp"
#include "lvd/write_bin_string.hpp"
#include "lvd/write_bin_tuple.hpp"
#include "lvd/write_bin_type.hpp"
#include "lvd/write_bin_unordered_map.hpp"
#include "lvd/write_bin_unordered_set.hpp"
#include "lvd/write_bin_vector.hpp"
#include "print.hpp"
#include <sstream>
#include <string>

namespace lvd {

template <typename T_, typename Encoding_>
void bin_roundtrip_test_case (req::Context &req_context, Encoding_ const &enc, T_ const &expected_value) {
    if constexpr (std::is_default_constructible_v<T_>) {
        std::ostringstream out;
        out << enc.out(expected_value);
        req_context.log() << Log::trc() << "actual output = " << literal_of(out.str()) << '\n';

        T_ actual_value;
        std::istringstream in(out.str());
        in >> enc.in(actual_value);

//         static_assert(std::is_same_v<decltype(expected_value),decltype(actual_value)>);
        req_context.log() << Log::trc() << LVD_REFLECT(expected_value) << cspace << LVD_REFLECT(actual_value) << '\n';

        LVD_TEST_REQ_EQ(actual_value, expected_value);
        req_context.log() << Log::trc() << "first test passed\n";
    }

    // Also test read_value.
    {
        std::ostringstream out;
        out << enc.out(expected_value);
        req_context.log() << Log::trc() << "actual output = " << literal_of(out.str()) << '\n';

        std::istringstream in(out.str());
        auto actual_value = enc.template read<T_>(in);
        LVD_TEST_REQ_EQ(actual_value, expected_value);

        req_context.log() << Log::trc() << "second test passed\n";
    }
}

template <typename T_>
void bin_roundtrip_encoding (req::Context &req_context, T_ const &expected_value) {
    bin_roundtrip_test_case(req_context, bin_big_e, expected_value);
    bin_roundtrip_test_case(req_context, bin_lil_e, expected_value);
    bin_roundtrip_test_case(req_context, bin_machine_e, expected_value);
    bin_roundtrip_test_case(req_context, tbin_big_e, expected_value);
    bin_roundtrip_test_case(req_context, tbin_lil_e, expected_value);
    bin_roundtrip_test_case(req_context, tbin_machine_e, expected_value);
}

LVD_TEST_BEGIN(231__read_write_bin__00__singletons)
//     bin_roundtrip_test_case(req_context, ty<void>);
    bin_roundtrip_encoding(req_context, ty<int32_t>);
    bin_roundtrip_encoding(req_context, ty<std::string>);
    bin_roundtrip_encoding(req_context, ty<std::vector<std::map<std::byte,bool>>>);
    bin_roundtrip_encoding(req_context, std::tuple<>());
LVD_TEST_END

template <typename T_, typename Encoding_>
void bin_roundtrip_test_case_random (req::Context &req_context, Encoding_ const &enc) {
    auto rng = std::mt19937{42};
    for (int i = 0; i < 100; ++i)
        bin_roundtrip_test_case(req_context, enc, make_random<T_>(rng));
}

template <typename Encoding_>
void bin_roundtrip_encoding_test_case_random (req::Context &req_context, Encoding_ const &enc) {
    bin_roundtrip_test_case_random<bool>(req_context, enc);
    bin_roundtrip_test_case_random<std::byte>(req_context, enc);
    bin_roundtrip_test_case_random<char>(req_context, enc);
    bin_roundtrip_test_case_random<int8_t>(req_context, enc);
    bin_roundtrip_test_case_random<uint8_t>(req_context, enc);
    bin_roundtrip_test_case_random<int16_t>(req_context, enc);
    bin_roundtrip_test_case_random<uint16_t>(req_context, enc);
    bin_roundtrip_test_case_random<int32_t>(req_context, enc);
    bin_roundtrip_test_case_random<uint32_t>(req_context, enc);
    bin_roundtrip_test_case_random<int64_t>(req_context, enc);
    bin_roundtrip_test_case_random<uint64_t>(req_context, enc);
    bin_roundtrip_test_case_random<float>(req_context, enc);
    bin_roundtrip_test_case_random<double>(req_context, enc);
    bin_roundtrip_test_case_random<std::string>(req_context, enc);
    bin_roundtrip_test_case_random<std::optional<uint32_t>>(req_context, enc);
    bin_roundtrip_test_case_random<std::optional<std::string>>(req_context, enc);
    bin_roundtrip_test_case_random<std::pair<char,uint8_t>>(req_context, enc);
    bin_roundtrip_test_case_random<std::tuple<int8_t>>(req_context, enc);
    bin_roundtrip_test_case_random<std::tuple<bool,int8_t>>(req_context, enc);
    bin_roundtrip_test_case_random<std::tuple<char,bool,int8_t>>(req_context, enc);
    bin_roundtrip_test_case_random<std::array<float,4>>(req_context, enc);
    bin_roundtrip_test_case_random<std::vector<std::byte>>(req_context, enc);
    bin_roundtrip_test_case_random<std::map<char,double>>(req_context, enc);
    bin_roundtrip_test_case_random<std::set<double>>(req_context, enc);
    bin_roundtrip_test_case_random<std::unordered_map<std::string,int8_t>>(req_context, enc);
    bin_roundtrip_test_case_random<std::unordered_set<int8_t>>(req_context, enc);
}

LVD_TEST_BEGIN(231__read_write_bin__01__bin_roundtrip)
    bin_roundtrip_encoding_test_case_random(req_context, bin_big_e);
    bin_roundtrip_encoding_test_case_random(req_context, bin_lil_e);
    bin_roundtrip_encoding_test_case_random(req_context, bin_machine_e);
    bin_roundtrip_encoding_test_case_random(req_context, tbin_big_e);
    bin_roundtrip_encoding_test_case_random(req_context, tbin_lil_e);
    bin_roundtrip_encoding_test_case_random(req_context, tbin_machine_e);
LVD_TEST_END

} // end namespace lvd
