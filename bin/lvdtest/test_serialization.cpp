// 2021.01.04 - Copyright Victor Dods - Licensed under Apache 2.0

#include "DerivedString_serialization.hpp"
#include <iomanip>
#include "lvd/abort.hpp"
#include "lvd/random.hpp"
#include "lvd/random_array.hpp"
#include "lvd/random_map.hpp"
#include "lvd/random_optional.hpp"
#include "lvd/random_pair.hpp"
#include "lvd/random_set.hpp"
#include "lvd/random_string.hpp"
#include "lvd/random_unordered_map.hpp"
#include "lvd/random_unordered_set.hpp"
#include "lvd/random_vector.hpp"
#include "lvd/Range_t.hpp"
#include "lvd/req.hpp"
#include "lvd/serialization.hpp"
#include "lvd/test.hpp"
#include <ostream>
#include "print.hpp"
#include <random>

namespace lvd {

template <typename T_>
void serialization_test_case (req::Context &req_context, std::vector<std::byte> &buffer) {
    auto rng = std::mt19937{42};

    // In-place serialize/deserialize.  Requires a default constructor.
    if constexpr (std::is_default_constructible_v<T_>) {
        for (auto i = 0; i < 0x100; ++i) {
            buffer.clear();
            auto expected = make_random<T_>(rng);

            serialize_from(expected, std::back_inserter(buffer));

            T_ actual;
            deserialize_to(actual, lvd::range(buffer));

            LVD_TEST_REQ_EQ(actual, expected);
        }
    }

    static_assert(std::is_default_constructible_v<char>);

    // Returned-value serialize/deserialize.  This is probably slower because it doesn't re-use
    // a std::vector<std::byte> instance with its allocated memory.
    for (auto i = 0; i < 0x100; ++i) {
        auto expected = make_random<T_>(rng);
        auto actual = deserialized_to<T_>(lvd::range(serialized_from(expected)));
        LVD_TEST_REQ_EQ(actual, expected);
    }
}

// 323 must be >= 320 (which is Range_t) and >= 321 (which is random) and 322 (which is endian), since serialization.hpp depends on those.
LVD_TEST_BEGIN(323__serialization__00)
    std::vector<std::byte> buffer;

    serialization_test_case<bool>(req_context, buffer);

    serialization_test_case<char>(req_context, buffer);
    serialization_test_case<unsigned char>(req_context, buffer);

    serialization_test_case<short int>(req_context, buffer);
    serialization_test_case<unsigned short int>(req_context, buffer);

    serialization_test_case<int>(req_context, buffer);
    serialization_test_case<unsigned int>(req_context, buffer);

    serialization_test_case<long int>(req_context, buffer);
    serialization_test_case<long unsigned int>(req_context, buffer);

    serialization_test_case<long long int>(req_context, buffer);
    serialization_test_case<long long unsigned int>(req_context, buffer);

    serialization_test_case<ptrdiff_t>(req_context, buffer);
    serialization_test_case<size_t>(req_context, buffer);

    serialization_test_case<std::byte>(req_context, buffer);

    serialization_test_case<uint8_t>(req_context, buffer);
    serialization_test_case<int8_t>(req_context, buffer);

    serialization_test_case<uint16_t>(req_context, buffer);
    serialization_test_case<int16_t>(req_context, buffer);

    serialization_test_case<uint32_t>(req_context, buffer);
    serialization_test_case<int32_t>(req_context, buffer);

    serialization_test_case<uint64_t>(req_context, buffer);
    serialization_test_case<int64_t>(req_context, buffer);

    serialization_test_case<std::string>(req_context, buffer);
    serialization_test_case<std::pair<int,std::string>>(req_context, buffer);
    serialization_test_case<std::vector<uint32_t>>(req_context, buffer);
    serialization_test_case<std::array<uint32_t,1>>(req_context, buffer);
    serialization_test_case<std::array<uint32_t,2>>(req_context, buffer);
    serialization_test_case<std::array<uint32_t,3>>(req_context, buffer);
    serialization_test_case<std::array<uint32_t,10>>(req_context, buffer);
    serialization_test_case<std::array<std::string,10>>(req_context, buffer);
    serialization_test_case<std::array<std::map<uint16_t,std::string>,10>>(req_context, buffer);
    serialization_test_case<std::map<int,std::string>>(req_context, buffer);
    serialization_test_case<std::unordered_map<int,std::string>>(req_context, buffer);
    serialization_test_case<std::set<std::string>>(req_context, buffer);
    serialization_test_case<std::unordered_set<std::string>>(req_context, buffer);

    serialization_test_case<std::optional<bool>>(req_context, buffer);
    serialization_test_case<std::optional<uint16_t>>(req_context, buffer);
    serialization_test_case<std::optional<std::string>>(req_context, buffer);
    serialization_test_case<std::optional<std::optional<double>>>(req_context, buffer);

//     // Do an absurd one for fun.
//     serialization_test_case<std::vector<std::map<std::string,std::unordered_map<int,std::set<uint16_t>>>>>(req_context, buffer);
LVD_TEST_END

//
// Test a bunch of different ways to inherit a serializable class, where the Serialization_t
// implementation can be inherited also.
//

// Test case for different derived string types
template <typename DerivedString_>
void serialization_test_case_derived_string (req::Context &req_context) {
    // Make sure DerivedString_ works as expected
    auto ds = DerivedString_(std::string("blah"));
    LVD_TEST_REQ_EQ(ds, std::string("blah"));

    // Testing serialization of derived classes.
    std::vector<std::byte> buffer;
    serialization_test_case<DerivedString_>(req_context, buffer);
}

LVD_TEST_BEGIN(323__serialization__01)
    serialization_test_case_derived_string<DerivedString_DC_IP>(req_context);
    serialization_test_case_derived_string<DerivedString_DC_EP>(req_context);
    serialization_test_case_derived_string<DerivedString_NDC_IP>(req_context);
    serialization_test_case_derived_string<DerivedString_NDC_EP>(req_context);
LVD_TEST_END

} // end namespace lvd
