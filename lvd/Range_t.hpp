// 2020.03.29 - Victor Dods

#pragma once

#include "lvd/core.hpp"

namespace lvd {

// Allows range-based for syntax for iterator pairs.
template <typename Iterator_>
class Range_t {
public:

    Range_t () = delete;
    Range_t (Iterator_ begin, Iterator_ end) : m_begin(begin), m_end(end) { }
    Range_t (Range_t const &other) : m_begin(other.m_begin), m_end(other.m_end) { }

    Range_t &operator = (Range_t const &other) {
        m_begin = other.m_begin;
        m_end = other.m_end;
    }

    Iterator_ begin () const { return m_begin; }
    Iterator_ end () const { return m_end; }

private:

    Iterator_ m_begin;
    Iterator_ m_end;
};

// Convenience function for creating a Range_t for use in range-based for syntax, e.g.
// `for (auto x : range(v.begin()+1, v.end()-1)) { ... }` where v is e.g. a std::vector.
template <typename Iterator_>
Range_t<Iterator_> range (Iterator_ begin, Iterator_ end) {
    return Range_t<Iterator_>(begin, end);
}

} // end namespace lvd
