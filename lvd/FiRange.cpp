// 2019.04.26 - Victor Dods

#include "lvd/FiRange.hpp"

#include "lvd/fmt.hpp"

namespace lvd {

FiRange const FiRange::INVALID;

// Defined in FiLoc.cpp -- should really use std::filesystem::path
std::string FilenamePortion (std::string const &path);

std::string FiRange::as_string () const
{
    return LVD_FMT(*this);
}

FiRange FiRange::operator + (std::uint32_t increment_column_by_value) const
{
    FiRange retval(*this);
    retval += increment_column_by_value;
    return retval;
}

FiRange FiRange::operator + (FiRange const &other) const
{
    FiRange retval(*this);
    retval += other;
    return retval;
}

FiRange FiRange::operator - (std::uint32_t decrement_column_by_value) const
{
    FiRange retval(*this);
    retval -= decrement_column_by_value;
    return retval;
}

void FiRange::operator += (std::uint32_t increment_column_by_value)
{
    if (is_valid())
        m_end += increment_column_by_value;
}

void FiRange::operator += (FiRange const &other)
{
    if (!is_valid())
        *this = other;
    else if (!other.is_valid())
    {
        // Nothing to do.
    }
    else if (m_filename != other.m_filename)
        *this = FiRange::INVALID;
    else
    {
        m_start = std::min(m_start, other.m_start);
        m_end = std::max(m_end, other.m_end);
    }
}

void FiRange::operator -= (std::uint32_t decrement_column_by_value)
{
    if (is_valid())
        m_start -= decrement_column_by_value;
}

} // end namespace lvd
