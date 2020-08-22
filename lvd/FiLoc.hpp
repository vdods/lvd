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

std::ostream &operator << (std::ostream &stream, FiLoc const &filoc);

} // end namespace lvd
