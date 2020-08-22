// 2019.04.26 - Victor Dods

#include "lvd/FiPos.hpp"

#include <sstream>

namespace lvd {

FiPos const FiPos::INVALID;

std::string FiPos::as_string () const
{
    if (is_valid())
        return LVD_FMT(m_line << ',' << m_column);
    else
        return std::string("FiPos::INVALID");
}

void FiPos::decrement_column (std::uint32_t by_value)
{
    if (!is_valid())
        return;

    assert(has_valid_column());
    if (by_value >= m_column)
        m_column = 1;
    else
        m_column -= by_value;
    assert(has_valid_column());
}

FiPos FiPos::operator + (std::uint32_t increment_column_by_value) const
{
    FiPos retval(*this);
    retval += increment_column_by_value;
    return retval;
}

FiPos FiPos::operator - (std::uint32_t decrement_column_by_value) const
{
    FiPos retval(*this);
    retval -= decrement_column_by_value;
    return retval;
}

void FiPos::operator += (std::uint32_t increment_column_by_value)
{
    if (is_valid())
        increment_column(increment_column_by_value);
}

void FiPos::operator -= (std::uint32_t decrement_column_by_value)
{
    decrement_column(decrement_column_by_value);
}

std::ostream &operator << (std::ostream &stream, FiPos const &fipos)
{
    return stream << fipos.as_string();
}

} // end namespace lvd
