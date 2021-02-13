// 2021.02.03 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/sst/SV_t.hpp"
// #include "lvd/type.hpp"
// #include "lvd/type_string_of.hpp"

namespace lvd {
namespace sst {

struct NonNull_s : public Base_s {
    using Super_s = Base_s;

    template <typename S_>
    static std::string const &type_string () {
        static std::string const STR{"NonNull"};
        return STR;
    }
    template <typename C_>
    static bool const is_valid (C_ const &cv) {
        return cv != nullptr;
    }
};

inline decltype(auto) constexpr check_policy_for__ctor_default (NonNull_s) { return value_v<CheckPolicy,PROHIBIT>; }

template <typename SV_, typename C_, typename T_>
inline decltype(auto) constexpr check_policy_for__ctor_copy_T (NonNull_s) {
    if constexpr (std::is_same_v<std::decay_t<T_>,nullptr_t>)
        return value_v<CheckPolicy,PROHIBIT>;
    else
        return check_policy_for__ctor_copy_T<SV_,C_,T_>(NonNull_s::Super_s{});
}

template <typename SV_, typename C_, typename T_>
inline decltype(auto) constexpr check_policy_for__ctor_move_T (NonNull_s) {
    if constexpr (std::is_same_v<std::decay_t<T_>,nullptr_t>)
        return value_v<CheckPolicy,PROHIBIT>;
    else
        return check_policy_for__ctor_move_T<SV_,C_,T_>(NonNull_s::Super_s{});
}

template <typename SV_, typename C_, typename First_, typename... Rest_>
inline decltype(auto) constexpr check_policy_for__ctor_variadic (NonNull_s) {
    if constexpr (sizeof...(Rest_) == 0 && std::is_same_v<std::decay_t<First_>,nullptr_t>)
        return value_v<CheckPolicy,PROHIBIT>;
    else
        return check_policy_for__ctor_variadic<SV_,C_,First_,Rest_...>(NonNull_s::Super_s{});
}

template <typename C_>
bool constexpr operator == (SV_t<NonNull_s,C_> const &, nullptr_t) {
    return false;
}
template <typename C_>
bool constexpr operator != (SV_t<NonNull_s,C_> const &, nullptr_t) {
    return true;
}

// Semantic value type factories.
template <typename C_> using NonNull_t = SV_t<NonNull_s,C_>;

// NonNull_s-specific frontends to make_sv and make_sv__with_collapse
template <typename C_> decltype(auto) make_non_null (C_ &&cv) { return make_sv<NonNull_s>(std::forward<C_>(cv)); }
template <typename C_> decltype(auto) make_non_null__with_collapse (C_ &&cv) { return make_sv__with_collapse<NonNull_s>(std::forward<C_>(cv)); }

} // end namespace sst
} // end namespace lvd
