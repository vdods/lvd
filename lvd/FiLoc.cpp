// 2006.02.11 - Victor Dods

#include "lvd/FiLoc.hpp"

#include <sstream>

namespace lvd {

FiLoc const FiLoc::INVALID;

auto constexpr DIRECTORY_SLASH_STRING = '/';

// TODO: Use std::filesystem::path
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
    return LVD_FMT(*this);
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

} // end namespace lvd
