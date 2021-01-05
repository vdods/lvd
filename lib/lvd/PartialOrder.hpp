// 2020.12.15 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <array>
#include <cassert>
#include <functional>
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
// Here are a few utility functions regarding PartialOrder
//

// For use in direct_product_partial_order_from
inline bool is_less_than_or_equal (PartialOrder r) {
    return r == PartialOrder::LESS_THAN || r == PartialOrder::EQUAL;
}

// For use in direct_product_partial_order_from
inline bool is_greater_than_or_equal (PartialOrder r) {
    return r == PartialOrder::GREATER_THAN || r == PartialOrder::EQUAL;
}

// This just swaps LESS_THAN and GREATER_THAN.
inline PartialOrder reversed_partial_order (PartialOrder r) {
    switch (r) {
        case PartialOrder::LESS_THAN: return PartialOrder::GREATER_THAN;
        case PartialOrder::GREATER_THAN: return PartialOrder::LESS_THAN;
        default: return r;
    }
}

//
// Here are a few functions which define natural partial orders on certain structures.
//

// Defines a partial order that is equal or incomparable, but nothing else.  This is the simplest natural
// poset on a type that has equality.
// template <typename T_>
inline PartialOrder equal_or_incomparable_partial_order (auto const &lhs, auto const &rhs) {
    return lhs == rhs ? PartialOrder::EQUAL : PartialOrder::INCOMPARABLE;
}

// This defines the partial order on sequences, where lhs < rhs is defined as
// lhs being a strict prefix of rhs.  If lhs and rhs differ on their common
// length, then they're incomparable.
inline PartialOrder prefix_partial_order (auto const &lhs, auto const &rhs) {
    auto lhs_size = lhs.size();
    auto rhs_size = rhs.size();
    auto common_length = std::min(lhs_size, rhs_size);
    auto equal_on_common_length = std::equal(lhs.begin(), lhs.begin()+common_length, rhs.begin(), rhs.begin()+common_length);
    if (!equal_on_common_length)
        return PartialOrder::INCOMPARABLE;
    else if (lhs_size == rhs_size)
        return PartialOrder::EQUAL;
    else if (lhs_size < rhs_size)
        return PartialOrder::LESS_THAN;
    else
        return PartialOrder::GREATER_THAN;
}

// Defines the partial order induced on the direct product for a given type having a partial order.
template <typename T_>
PartialOrder direct_product_partial_order (auto const &lhs, auto const &rhs, std::function<PartialOrder(T_ const &,T_ const &)> const &partial_order) {
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

// // Defines the partial order of a given type relative to a union of types.
// template <typename T_>
// PartialOrder element_of_union_partial_order (T_ const &lhs, std::vector<T_> const &rhs, std::function<PartialOrder(T_ const &,T_ const &)> const &partial_order) {
//     LVD_G_REQ_IS_FALSE(rhs.empty(), "for now, unions are not allowed to be empty");
//
//     // lhs < rhs iff lhs < X for some X in rhs.
//     // lhs > rhs iff (lhs >= X for all X in rhs) and (lhs > Y for some Y in rhs).
//     // lhs == rhs iff (lhs <= X for all X in rhs) and (lhs >= Y for all Y in rhs).
//     // lhs ~/~ rhs iff lhs ~/~ X for all X in rhs.
// }

// // Defines the partial order induced on unions of terms of a given type having a partial order.
// // Definition criteria
// // -    Union() is empty, and is therefore LESS_THAN any other nonempty thing.  Maybe forbid this for simplicity.
// // -    Union(X) == X for any X
// // -    Union(...,X,Y,Y,Z,...) == Union(...,X,Y,Z,...) for any X,Y,Z
// // -    Union(...,X,Union(A1,...,An),Z,...) == Union(...,X,A1,...,An,Z,...) for any X,A1,...,An,Z
// // -    Union(...,X,Y,...) == Union(...,Y,...) if X <= Y.
// // -    Union(...,X,Y,...) == Union(...,Y,X,...)
// // And the partial order is defined, with A ::= Union(A_1, ..., A_m) and B ::= Union(B_1, ..., B_n) as
// //
// //      A == B
// //          iff
// //      there exists some j such that for all i, A_i <= B_j
// //      and
// //      there exists some p such that for all q, B_
// template <typename T_>
// PartialOrder union_partial_order (std::vector<T_> const &lhs, std::vector<T_> const &rhs, std::function<PartialOrder(T_ const &,T_ const &)> const &partial_order) {
//     LVD_G_REQ_IS_FALSE(lhs.empty(), "for now, unions are not allowed to be empty");
//     LVD_G_REQ_IS_FALSE(rhs.empty(), "for now, unions are not allowed to be empty");
//
//     auto retval = PartialOrder::EQUAL;
//
//     for (auto const &lhs_element : lhs) {
//         for (auto const &rhs_element : rhs) {
//
//         }
//     }
// }

} // end namespace lvd
