// 2020.03.29 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/fmt.hpp"
#include <type_traits>

namespace lvd {

// Allows range-based for syntax for iterator pairs.
template <typename Iterator_>
class Range_t {
public:

    Range_t () = delete;
    template <typename Container_, typename = std::enable_if_t<!std::is_same_v<Container_,Range_t>>>
    Range_t (Container_ const &container) : m_begin(container.begin()), m_end(container.end()) { }
    template <typename Container_, typename = std::enable_if_t<!std::is_same_v<Container_,Range_t>>>
    Range_t (Container_ &container) : m_begin(container.begin()), m_end(container.end()) { }
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

// Convenience function for creating a Range_t for use in range-based for syntax, e.g.
// `for (auto x : range(v.begin()+1, v.end()-1)) { ... }` where v is e.g. a std::vector.
template <typename Iterator_>
Range_t<Iterator_> range (Iterator_ begin, Iterator_ end) {
    return Range_t<Iterator_>(begin, end);
}

} // end namespace lvd
