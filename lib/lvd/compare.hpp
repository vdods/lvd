// 2020.09.12 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

namespace lvd {

// Runtime complexity O(size); lexicographical ordering (not shortlex order).
template <typename Iterator_>
int compare_lexicographical (Range_t<Iterator_> const &lhs, Range_t<Iterator_> const &rhs) {
    for (Iterator_ lhs_it = lhs.begin(), rhs_it = rhs.begin(); lhs_it != lhs.end() && rhs_it != rhs.end(); ++lhs_it, ++rhs_it) {

    }

    for (size_t i = 0; i < lhs.size() && i < rhs.size(); ++i) {
        auto c = compare_data(lhs[i], rhs[i]);
        if (c != 0)
            return c;
    }
    // If we got this far, then they match on their common length, so the shorter one is "less".
    return lhs.size() < rhs.size() ? -1 : (lhs.size() == rhs.size() ? 0 : 1);
}

// TODO: Implement compare_shortlex

} // end namespace lvd
