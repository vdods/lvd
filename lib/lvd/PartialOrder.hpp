// 2020.12.15 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <ostream>
#include <string>

namespace lvd {

enum class PartialOrder : uint8_t {
    LESS_THAN = 0,
    GREATER_THAN,
    EQUAL,
    INCOMPARABLE,

    __LOWEST__ = LESS_THAN,
    __HIGHEST__ = INCOMPARABLE
};

inline std::string const &as_string (PartialOrder x) {
    auto constexpr COUNT = size_t(PartialOrder::__HIGHEST__) - size_t(PartialOrder::__LOWEST__) + 1;
    static std::array<std::string,COUNT> const TABLE{
        "LESS_THAN",
        "GREATER_THAN",
        "EQUAL",
        "INCOMPARABLE"
    };
    return TABLE.at(size_t(x));
}

inline std::ostream &operator << (std::ostream &out, PartialOrder x) {
    return out << as_string(x);
}

//
// Here are a few
//

// For use in direct_product_partial_order_from
inline bool is_less_than_or_equal (PartialOrder r) {
    return r == PartialOrder::LESS_THAN || r == PartialOrder::EQUAL;
}

// For use in direct_product_partial_order_from
inline bool is_greater_than_or_equal (PartialOrder r) {
    return r == PartialOrder::GREATER_THAN || r == PartialOrder::EQUAL;
}

template <typename T_>
PartialOrder direct_product_partial_order (std::vector<T_> const &lhs, std::vector<T_> const &rhs, std::function<PartialOrder(T_ const &,T_ const &)> const &partial_order) {
    if (lhs.size() != rhs.size())
        return PartialOrder::INCOMPARABLE;

    // Empty vectors are equal.  Nonempty vectors may have elements which change that.
    auto retval = PartialOrder::EQUAL;

    for (auto lhs_it = lhs.begin(), rhs_it = rhs.begin(); lhs_it != lhs.end(); ++lhs_it, ++rhs_it) {
        assert(retval != PartialOrder::INCOMPARABLE);
        switch (partial_order(*lhs_it, *rhs_it)) {
            case PartialOrder::LESS_THAN:
                if (retval == PartialOrder::GREATER_THAN)
                    // LESS_THAN and GREATER_THAN are present, which means INCOMPARABLE
                    return PartialOrder::INCOMPARABLE;
                else {
                    retval = PartialOrder::LESS_THAN;
                    break;
                }

            case PartialOrder::GREATER_THAN:
                if (retval == PartialOrder::LESS_THAN)
                    // LESS_THAN and GREATER_THAN are present, which means INCOMPARABLE
                    return PartialOrder::INCOMPARABLE;
                else {
                    retval = PartialOrder::GREATER_THAN;
                    break;
                }

            case PartialOrder::EQUAL:
                // Still good, no need to change.
                break;

            case PartialOrder::INCOMPARABLE:
                // Can early-out here.
                return PartialOrder::INCOMPARABLE;
        }
    }

    return retval;
}

// Empty tuple; always equal because it's a singleton.
inline PartialOrder direct_product_partial_order_from () {
    return PartialOrder::EQUAL;
}

// Inductive case for direct product.
template <typename... Rest_>
PartialOrder direct_product_partial_order_from (PartialOrder first_rel, Rest_&&... rest) {
    auto rest_rel = direct_product_partial_order_from(std::forward<Rest_>(rest)...);
    if (first_rel == rest_rel)
        return first_rel;
    else if (is_less_than_or_equal(first_rel) && is_less_than_or_equal(rest_rel))
        return PartialOrder::LESS_THAN;
    else if (is_greater_than_or_equal(first_rel) && is_greater_than_or_equal(rest_rel))
        return PartialOrder::GREATER_THAN;
    else
        return PartialOrder::INCOMPARABLE;
}

// Defines a partial order that is equal or incomparable, but nothing else.
template <typename T_>
PartialOrder equal_or_incomparable_partial_order (T_ const &lhs, T_ const &rhs) {
    return lhs == rhs ? PartialOrder::EQUAL : PartialOrder::INCOMPARABLE;
}

} // end namespace lvd
