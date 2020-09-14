// 2019.04.26 - Victor Dods

#pragma once

#include <cassert>
#include <cstdint>
#include "lvd/FiPos.hpp"
#include <ostream>

namespace lvd {

/// FiRange represents a contiguous range (between two FiPos-es) within a file.
struct FiRange
{
    static FiRange const INVALID;

    FiRange ()
        :   m_start(FiPos::INVALID)
        ,   m_end(FiPos::INVALID)
    {
        assert(!is_valid());
    }
    FiRange (FiRange const &) = default;
    FiRange (FiRange &&) = default;
    FiRange (std::string const &filename)
        :   m_filename(filename)
        ,   m_start(FiPos::INVALID)
        ,   m_end(FiPos::INVALID)
    {
        assert(!m_filename.empty());
        assert(!is_valid());
    }
    /// Construct a FiRange corresponding to a zero char range at the given FiPos.
    FiRange (std::string const &filename, FiPos const &fipos)
        :   m_filename(filename)
        ,   m_start(fipos)
        ,   m_end(fipos)
    {
        assert(is_valid());
    }
    /// start and end must be increasing in the sense described in the comments
    /// for is_valid().
    FiRange (std::string const &filename, FiPos const &start, FiPos const &end)
        :   m_filename(filename)
        ,   m_start(start)
        ,   m_end(end)
    {
        assert(is_valid());
    }

    FiRange &operator = (FiRange const &) = default;
    FiRange &operator = (FiRange &&) = default;

    bool operator == (FiRange const &other) const
    {
        return m_filename == other.m_filename && m_start == other.m_start && m_end == other.m_end;
    }

    /// Only valid if the filename is nonempty, and start <= end, based on definition in FiPos.
    bool is_valid () const
    {
        return !m_filename.empty() && m_start <= m_end;
    }
//     bool HasLineNumber () const { return !m_filename.empty() && m_start.HasLineNumber() && m_end.HasLineNumber(); }
    std::string const &filename () const
    {
        assert(this != &INVALID && "can't use FiRange::INVALID in this manner");
        return m_filename;
    }
    FiPos const &start () const
    {
        assert(this != &INVALID && "can't use FiRange::INVALID in this manner");
        return m_start;
    }
    FiPos const &end () const
    {
        assert(this != &INVALID && "can't use FiRange::INVALID in this manner");
        return m_end;
    }
    std::string as_string () const;
//     /// If this FiRange satisfies the constraints given in this function, i.e.
//     /// the start and end lines are the same, and the start and end columns are 1,
//     /// then this FiRange is convertable to well-defined FiLoc, which is accessible
//     /// via as_filoc.
//     bool is_convertable_to_filoc () const { return m_start.line() == m_end.line() && m_start.column() == 1 && m_end.column() == 1; }
//     FiLoc as_filoc () const { return FiLoc(m_filename, m_start.line()); }

    // TODO: Should really use FiPos for this purpose.
    FiRange start_as_firange () const { return FiRange(m_filename, m_start); }
    FiRange end_as_firange () const { return FiRange(m_filename, m_end); }

    /// This adds the given value to the column of the end.
    FiRange operator + (std::uint32_t increment_column_by_value) const;
    /// This joins two FiRange objects into the minimal FiRange that contains both,
    /// i.e. the convex hull in the lexicographical topology.  They must both have
    /// the same filename and must both be valid, or FiRange::INVALID will result.
    FiRange operator + (FiRange const &other) const;

    /// This subtracts the given value from the column of the start (or sets it
    /// to 1 if the result would be negative).
    FiRange operator - (std::uint32_t decrement_column_by_value) const;

    void set_filename (std::string const &filename) { m_filename = filename; }
    void set_start (FiPos const &start) { m_start = start; }
    void set_end (FiPos const &end) { m_end = end; }

//     // These are interop methods to make FiRange act like FiLoc.  (TEMP)
//     void set_line (std::uint32_t line) { m_start = m_end = FiPos(line, 1); assert(is_convertable_to_filoc()); }

    // Adds the given value to the column of the end.
    void operator += (std::uint32_t increment_column_by_value);
    // If the filenames don't match, then this is set to FiRange::INVALID.
    // If only one of the operands is valid, this will be set to the valid FiRange.
    // If both operands are valid, then this will be set to the convex hull of the
    // FiRange values.  This choice is made to make the combining of FiRanges more
    // friendly, even if it's less than totally well-defined.
    void operator += (FiRange const &other);

    /// Subtracts the given value from the column of the start (or sets it to
    /// 1 if the result would be negative).
    void operator -= (std::uint32_t decrement_column_by_value);

private:

    std::string m_filename;
    FiPos m_start;
    FiPos m_end;
}; // end of class FiRange

inline std::ostream &operator << (std::ostream &out, FiRange const &firange)
{
    if (firange.is_valid())
    {
        if (firange.start() == firange.end())
            out << firange.filename() << ':' << firange.start();
        else
            out << firange.filename() << ':' << firange.start() << '-' << firange.end();
    }
    else
        out << "FiRange::INVALID";
    return out;
}

} // end namespace lvd
