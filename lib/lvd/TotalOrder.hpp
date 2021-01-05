// 2020.12.29 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
// #include <cassert>
// #include <functional>
#include "lvd/PartialOrder.hpp"
#include <ostream>
#include <string>

namespace lvd {

enum class TotalOrder : uint8_t {
    LESS_THAN       = uint8_t(PartialOrder::LESS_THAN),
    GREATER_THAN    = uint8_t(PartialOrder::GREATER_THAN),
    EQUAL           = uint8_t(PartialOrder::EQUAL),

    __LOWEST__      = LESS_THAN,
    __HIGHEST__     = EQUAL
};

inline std::string const &as_string (TotalOrder x) {
    auto constexpr COUNT = size_t(TotalOrder::__HIGHEST__) - size_t(TotalOrder::__LOWEST__) + 1;
    static std::array<std::string,COUNT> const TABLE{
        "LESS_THAN",
        "GREATER_THAN",
        "EQUAL",
    };
    return TABLE.at(size_t(x));
}

inline std::ostream &operator << (std::ostream &out, TotalOrder x) {
    return out << as_string(x);
}

inline PartialOrder as_partial_order (TotalOrder x) {
    return PartialOrder(uint8_t(x));
}

//
// Here are a few utility functions regarding TotalOrder
//

// For use in direct_product_partial_order_from
inline bool is_less_than_or_equal (TotalOrder r) {
    return r == TotalOrder::LESS_THAN || r == TotalOrder::EQUAL;
}

// For use in direct_product_partial_order_from
inline bool is_greater_than_or_equal (TotalOrder r) {
    return r == TotalOrder::GREATER_THAN || r == TotalOrder::EQUAL;
}

// This just swaps LESS_THAN and GREATER_THAN.
inline TotalOrder reversed_total_order (TotalOrder r) {
    switch (r) {
        case TotalOrder::LESS_THAN: return TotalOrder::GREATER_THAN;
        case TotalOrder::GREATER_THAN: return TotalOrder::LESS_THAN;
        default: return r;
    }
}

//
// Here are a few functions which define natural total orders on certain structures.
//

// Natural definition, not necessarily efficient.  Requires == and < to be implemented for the given type.
inline TotalOrder natural_total_order (auto const &lhs, auto const &rhs) {
    if (lhs == rhs)
        return TotalOrder::EQUAL;
    else if (lhs < rhs)
        return TotalOrder::LESS_THAN;
    else {
        assert(rhs < lhs);
        return TotalOrder::GREATER_THAN;
    }
}

// Defines the lexicographical total order induced on the sequences of a given type using natural_total_order.
inline TotalOrder lexicographical_total_order (auto const &lhs, auto const &rhs) {
    auto lhs_it = lhs.begin(), rhs_it = rhs.begin();
    for ( ; lhs_it != lhs.end() && rhs_it != rhs.end(); ++lhs_it, ++rhs_it) {
        auto t = natural_total_order(*lhs_it, *rhs_it);
        if (t != TotalOrder::EQUAL)
            return t;
    }
    if (lhs_it == lhs.end())
        return rhs_it == rhs.end() ? TotalOrder::EQUAL : TotalOrder::LESS_THAN;
    else {
        assert(rhs_it == rhs.end());
        return TotalOrder::GREATER_THAN;
    }
}

// Defines the lexicographical total order induced on the sequences of a given type having a total order.
template <typename T_>
TotalOrder lexicographical_total_order (auto const &lhs, auto const &rhs, std::function<TotalOrder(T_ const &,T_ const &)> const &total_order) {
    auto lhs_it = lhs.begin(), rhs_it = rhs.begin();
    for ( ; lhs_it != lhs.end() && rhs_it != rhs.end(); ++lhs_it, ++rhs_it) {
        auto t = total_order(*lhs_it, *rhs_it);
        if (t != TotalOrder::EQUAL)
            return t;
    }
    if (lhs_it == lhs.end())
        return rhs_it == rhs.end() ? TotalOrder::EQUAL : TotalOrder::LESS_THAN;
    else {
        assert(rhs_it == rhs.end());
        return TotalOrder::GREATER_THAN;
    }
}

// Empty tuple; always equal because it's a singleton.
inline TotalOrder lexicographical_total_order_from () {
    return TotalOrder::EQUAL;
}

// Inductive case for direct product.
template <typename... Rest_>
TotalOrder lexicographical_total_order_from (TotalOrder first_rel, Rest_&&... rest) {
    if (first_rel != TotalOrder::EQUAL)
        return first_rel;
    return lexicographical_total_order_from(std::forward<Rest_>(rest)...);
}

} // end namespace lvd
