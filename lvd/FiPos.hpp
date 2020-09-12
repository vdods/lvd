// 2019.04.26 - Victor Dods

#pragma once

#include <cassert>
#include <cstdint>
#include "lvd/fmt.hpp"
#include <ostream>

namespace lvd {

/// Represents a (line,column) pair, as in a specific char position in a file.
/// line and column numbers are both 1-based (i.e. the first line of a file is
/// line 1, and the first column in a line is column 1).
struct FiPos
{
    static FiPos const INVALID;

    FiPos ()
        :   m_line(0)
        ,   m_column(0)
    {
        assert(!is_valid());
    }
    FiPos (std::uint32_t line, std::uint32_t column)
        :   m_line(line)
        ,   m_column(column)
    {
        assert(is_valid());
    }

    /// A FiPos is only valid if the line and column numbers are positive.
    bool is_valid () const { return has_valid_line() && has_valid_column(); }
    bool has_valid_line () const { return m_line > 0; }
    bool has_valid_column () const { return m_column > 0; }
    std::uint32_t line () const { return m_line; }
    std::uint32_t column () const { return m_column; }
    std::string as_string () const;

    void set_line (std::uint32_t line) { m_line = line; }
    void increment_line (std::uint32_t by_value) { if (is_valid()) m_line += by_value; }
    void set_column (std::uint32_t column) { m_column = column; }
    void increment_column (std::uint32_t by_value) { if (is_valid()) m_column += by_value; }
    void decrement_column (std::uint32_t by_value);

    bool operator == (FiPos const &other) const { return m_line == other.m_line && m_column == other.m_column; }
    /// The ordering is lexicographic with line first, then column.
    bool operator <= (FiPos const &other) const { return m_line < other.m_line || (m_line == other.m_line && m_column <= other.m_column); }
    /// The ordering is lexicographic with line first, then column.
    bool operator < (FiPos const &other) const { return m_line < other.m_line || (m_line == other.m_line && m_column < other.m_column); }

    FiPos operator + (std::uint32_t increment_column_by_value) const;
    FiPos operator - (std::uint32_t decrement_column_by_value) const;

    void operator += (std::uint32_t increment_column_by_value);
    void operator -= (std::uint32_t decrement_column_by_value);

private:

    std::uint32_t m_line;
    std::uint32_t m_column;
}; // end of class FiPos

inline std::ostream &operator << (std::ostream &out, FiPos const &fipos)
{
    if (fipos.is_valid())
        out << fipos.line() << ',' << fipos.column();
    else
        out << "FiPos::INVALID";
    return out;
}

} // end namespace lvd
