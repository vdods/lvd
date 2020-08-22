// 2019.04.26 - Victor Dods

#include "lvd/FiRange.hpp"

#include <sstream>

namespace lvd {

FiRange const FiRange::INVALID;

#define DIRECTORY_SLASH_STRING '/'

// Defined in FiLoc.cpp -- should really use std::filesystem
std::string FilenamePortion (std::string const &path);

std::string FiRange::as_string () const
{
    if (is_valid())
    {
        if (m_start == m_end)
            return LVD_FMT(m_filename << ':' << m_start);
        else
            return LVD_FMT(m_filename << ':' << m_start << '-' << m_end);
    }
    else
        return std::string("FiRange::INVALID");
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

std::ostream &operator << (std::ostream &stream, FiRange const &firange)
{
    return stream << firange.as_string();
}

} // end namespace lvd
