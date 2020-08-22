// 2006.02.11 - Victor Dods

#include "lvd/FiLoc.hpp"

#include <sstream>

namespace lvd {

FiLoc const FiLoc::INVALID;

#define DIRECTORY_SLASH_STRING '/'

std::string FilenamePortion (std::string const &path)
{
    std::string::size_type last_slash = path.find_last_of(DIRECTORY_SLASH_STRING);
    if (last_slash == std::string::npos)
        return path;
    else
        return path.substr(last_slash+1);
}

std::string FiLoc::as_string () const
{
    // assert(this != &INVALID && "can't use FiLoc::INVALID in this manner");
    // assert(is_valid());
    if (is_valid())
    {
        std::ostringstream out;
        out << m_filename;
        if (m_line_number > 0)
            out << ":" << m_line_number;
        return out.str();
    }
    else
    {
        return std::string("FiLoc::INVALID");
    }
}

std::string FiLoc::line_directive_string () const
{
    assert(this != &INVALID && "do not use this on a FiLoc without a line number");
    assert(is_valid());

    std::ostringstream out;
    out << "#line " << m_line_number << " \"" << FilenamePortion(m_filename) << "\"";
    return out.str();
}

void FiLoc::increment_line_number (uint32_t by_value)
{
    assert(m_line_number > 0 && "don't use this on non-line-number-using FiLocs");
    m_line_number += by_value;
}

std::ostream &operator << (std::ostream &stream, FiLoc const &filoc)
{
    return stream << filoc.as_string();
}

} // end namespace lvd
