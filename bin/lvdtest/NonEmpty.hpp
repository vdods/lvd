// 2021.01.28 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/sst/SV_t.hpp"
#include "lvd/type.hpp"
#include "lvd/type_string_of.hpp"
// #include "print.hpp"

namespace lvd {

struct NonEmpty_s : public sst::Base_s {
    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"NonEmpty"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return !cv.empty();
    }
};

// Disallow default construction, since that would be an empty string.
inline decltype(auto) constexpr check_policy_for__ctor_default (NonEmpty_s) { return sst::value_v<sst::CheckPolicy,sst::PROHIBIT>; }

// Adding to a nonempty string can't make it empty.
LVD_DEFINE_INPLACE_OPERATOR_CHECK_POLICIES_FOR(NonEmpty_s, add_eq, sst::ALLOW__NO_CHECK)

// Sum of nonempty string with anything else is nonempty string.
LVD_DEFINE_SEMANTIC_BIN_OP(+, add, NonEmpty_s, NonEmpty_s, NonEmpty_s, sst::ALLOW__NO_CHECK)
LVD_DEFINE_SEMANTIC_BIN_OP_RESULT_POLICY_COMMUTATIVE__SV_T(add, NonEmpty_s, sst::CAST_TO_SV__NO_CHECK)

// Semantic value type factories.
template <typename C_> using NonEmpty_t = sst::SV_t<NonEmpty_s,C_>;

} // end namespace lvd
