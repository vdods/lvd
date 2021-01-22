// 2020.03.29 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/fmt.hpp"
#include <type_traits>
#include <utility>

namespace lvd {

// Allows range-based for syntax for iterator pairs.
template <typename Iterator_>
class Range_t {
public:

    Range_t () = delete;
    Range_t (Range_t const &) = default;
    Range_t (Range_t &&) = default;
    template <typename Container_, typename = std::enable_if_t<!std::is_same_v<Container_,Range_t>>>
    explicit Range_t (Container_ const &container) : m_begin(container.begin()), m_end(container.end()) { }
    template <typename Container_, typename = std::enable_if_t<!std::is_same_v<Container_,Range_t> && !std::is_same_v<Container_,std::pair<Iterator_,Iterator_>>>>
    explicit Range_t (Container_ &container) : m_begin(container.begin()), m_end(container.end()) { }
    explicit Range_t (std::pair<Iterator_,Iterator_> const &range) : m_begin(range.first), m_end(range.second) { }
    Range_t (Iterator_ begin, Iterator_ end) : m_begin(begin), m_end(end) { }

    Range_t &operator = (Range_t const &) = default;
    Range_t &operator = (Range_t &&) = default;

    bool operator == (Range_t const &other) const { return m_begin == other.m_begin && m_end == other.m_end; }
    bool operator != (Range_t const &other) const { return m_begin != other.m_begin || m_end != other.m_end; }

    Iterator_ &begin () { return m_begin; }
    Iterator_ &end () { return m_end; }

    Iterator_ begin () const { return m_begin; }
    Iterator_ end () const { return m_end; }

    // Returned the signed size of this Range_t.  The size will be:
    // -    positive if begin() < end(),
    // -    zero if begin() == end(),
    // -    negative if begin() > end().
    auto size () const { return std::distance(m_begin, m_end); }
    // Returns true iff size() == 0.
    bool empty () const { return m_begin == m_end; }

    // Returns a 0-sized Range_t located at this->begin().
    Range_t begin_as_range () const { return Range_t(m_begin, m_begin); }
    // Returns a 0-sized Range_t located at this->end().
    Range_t end_as_range () const { return Range_t(m_end, m_end); }

    // Sets this to the smallest Range_t which contains both this and other.  This really
    // only makes sense when operator< is well-defined for Iterator_.
    Range_t &union_with (Range_t const &other) {
        m_begin = std::min(m_begin, other.m_begin);
        m_end = std::max(m_end, other.m_end);
        return *this;
    }
    // Returns the largest Range_t which is contained by both this and other.  This really
    // only makes sense when operator< is well-defined for Iterator_.  Note that if the
    // intersection is empty, then the resulting begin and end values will be equal (and
    // therefore its size will be 0), but otherwise unspecified.
    Range_t &intersection_with (Range_t const &other) {
        Iterator_ b = std::max(m_begin, other.m_begin);
        Iterator_ e = std::min(m_end, other.m_end);
        // Ensure this doesn't produce a Range_t with negative orientation (negative size)
        m_begin = std::min(b, e);
        m_end = e;
        return *this;
    }

    // Returns the smallest Range_t which contains both this and other.  This really
    // only makes sense when operator< is well-defined for Iterator_.
    Range_t unioned_with (Range_t const &other) const {
        Range_t retval(*this);
        retval.union_with(other);
        return retval;
    }
    // Returns the largest Range_t which is contained by both this and other.  This really
    // only makes sense when operator< is well-defined for Iterator_.  Note that if the
    // intersection is empty, then the resulting begin and end values will be equal (and
    // therefore its size will be 0), but otherwise unspecified.
    Range_t intersectioned_with (Range_t const &other) const {
        Range_t retval(*this);
        retval.intersection_with(other);
        return retval;
    }

private:

    Iterator_ m_begin;
    Iterator_ m_end;
};

// Somewhat nontrivial -- lhs.end() can coincide with rhs.start() as long as at least one of lhs and rhs is nonempty.
template <typename Iterator_>
bool operator < (Range_t<Iterator_> const &lhs, Range_t<Iterator_> const &rhs) {
    if (lhs.empty() && rhs.empty())
        return lhs.end() < rhs.begin();
    else
        return lhs.end() <= rhs.begin();
}

template <typename Iterator_>
bool operator <= (Range_t<Iterator_> const &lhs, Range_t<Iterator_> const &rhs) {
    return lhs.end() <= rhs.begin();
}

// Convenience function for creating a Range_t over a container for use in range-based for
// syntax, e.g. `for (auto x : range(v)) { ... }` where v is e.g. a std::vector.
template <typename Container_>
Range_t<typename Container_::const_iterator> range (Container_ const &container) {
    return Range_t<typename Container_::const_iterator>(container);
}

// Convenience function for creating a Range_t over a container for use in range-based for
// syntax, e.g. `for (auto x : range(v)) { ... }` where v is e.g. a std::vector.
template <typename Container_>
Range_t<typename Container_::iterator> range (Container_ &container) {
    return Range_t<typename Container_::iterator>(container);
}

// Convenience function for creating a Range_t from a std::pair of iterators for use in
// range-based for syntax, e.g. `for (auto x : range(m.equal_range(...))) { ... }`
// where m is e.g. a std::map.
template <typename Iterator_>
Range_t<Iterator_> range (std::pair<Iterator_,Iterator_> const &pair) {
    return Range_t<Iterator_>(pair);
}

// Convenience function for creating a Range_t for use in range-based for syntax, e.g.
// `for (auto x : range(v.begin()+1, v.end()-1)) { ... }` where v is e.g. a std::vector.
template <typename Iterator_>
Range_t<Iterator_> range (Iterator_ begin, Iterator_ end) {
    return Range_t<Iterator_>(begin, end);
}

// Convenience function which provides the expected idempotent behavior.  I.e. lvd::range(lvd::range(x)) == lvd::range(x).
template <typename Iterator_>
Range_t<Iterator_> range (Range_t<Iterator_> const &r) {
    return r;
}

// Convenience function which provides the expected idempotent behavior.  I.e. lvd::range(lvd::range(x)) == lvd::range(x).
template <typename Iterator_>
Range_t<Iterator_> range (Range_t<Iterator_> &&r) {
    return std::move(r);
}

template <typename T_> struct is_Range_t_ : public std::false_type { };
template <typename Iterator_> struct is_Range_t_<Range_t<Iterator_>> : public std::true_type { };

// Determines if a given type T_ is a Range_t<Iterator_> for some type Iterator_.
template <typename T_>
inline bool constexpr is_Range_t = is_Range_t_<T_>::value;

} // end namespace lvd
