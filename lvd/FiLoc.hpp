// 2006.02.11 - Victor Dods

#pragma once

#include <cassert>
#include <cstdint>
#include "lvd/core.hpp"
#include <ostream>

namespace lvd {

struct FiLoc
{
    static FiLoc const INVALID;

    FiLoc ()
        : m_line_number(0)
    {
        assert(!is_valid());
    }
    FiLoc (FiLoc const &) = default;
    FiLoc (FiLoc &&) = default;
    FiLoc (std::string const &filename)
        : m_filename(filename)
        , m_line_number(0)
    {
        assert(!m_filename.empty());
    }
    FiLoc (std::string const &filename, uint32_t line)
        : m_filename(filename)
        , m_line_number(line)
    {
        assert(!m_filename.empty());
        assert(m_line_number > 0);
    }

    FiLoc &operator = (FiLoc const &) = default;
    FiLoc &operator = (FiLoc &&) = default;

    bool is_valid () const { return !m_filename.empty(); }
    bool has_line_number () const { return !m_filename.empty() && m_line_number > 0; }
    std::string const &filename () const
    {
        assert(this != &INVALID && "can't use FiLoc::INVALID in this manner");
        return m_filename;
    }
    uint32_t line_number () const
    {
        assert(this != &INVALID && "can't use FiLoc::INVALID in this manner");
        return m_line_number;
    }
    std::string as_string () const;
    std::string line_directive_string () const;

    void set_filename (std::string const &filename) { m_filename = filename; }
    void set_line_number (uint32_t line_number) { m_line_number = line_number; }

    void increment_line_number (uint32_t by_value = 1);

private:

    std::string m_filename;
    uint32_t m_line_number;
}; // end of class FiLoc

inline std::ostream &operator << (std::ostream &out, FiLoc const &filoc)
{
    if (filoc.is_valid())
    {
        out << filoc.filename();
        if (filoc.line_number() > 0)
            out << ":" << filoc.line_number();
    }
    else
        out << "FiLoc::INVALID";
    return out;
}

// Use this macro to create an lvd::FiLoc at the current file:line location in the source file.
#define LVD_FILOC() lvd::FiLoc{__FILE__, __LINE__}

} // end namespace lvd
