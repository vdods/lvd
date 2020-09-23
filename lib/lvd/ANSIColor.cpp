// 2019.12.17 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/ANSIColor.hpp"

#include <cassert>
#include <iostream>

namespace lvd {

enum class ANSIColorCodeForeground : uint32_t
{
    DARK_BLACK      = 30,
    DARK_RED        = 31,
    DARK_GREEN      = 32,
    DARK_YELLOW     = 33,
    DARK_BLUE       = 34,
    DARK_MAGENTA    = 35,
    DARK_CYAN       = 36,
    DARK_WHITE      = 37,

    BRIGHT_BLACK    = 90,
    BRIGHT_RED      = 91,
    BRIGHT_GREEN    = 92,
    BRIGHT_YELLOW   = 93,
    BRIGHT_BLUE     = 94,
    BRIGHT_MAGENTA  = 95,
    BRIGHT_CYAN     = 96,
    BRIGHT_WHITE    = 97
};

enum class ANSIColorCodeBackground : uint32_t
{
    DARK_BLACK      = 40,
    DARK_RED        = 41,
    DARK_GREEN      = 42,
    DARK_YELLOW     = 43,
    DARK_BLUE       = 44,
    DARK_MAGENTA    = 45,
    DARK_CYAN       = 46,
    DARK_WHITE      = 47,

    BRIGHT_BLACK    = 100,
    BRIGHT_RED      = 101,
    BRIGHT_GREEN    = 102,
    BRIGHT_YELLOW   = 103,
    BRIGHT_BLUE     = 104,
    BRIGHT_MAGENTA  = 105,
    BRIGHT_CYAN     = 106,
    BRIGHT_WHITE    = 107
};

ANSIColorCodeForeground ansi_color_code_foreground (ANSIColor ansi_color)
{
    switch (ansi_color)
    {
        case ANSIColor::DARK_BLACK:         return ANSIColorCodeForeground::DARK_BLACK;
        case ANSIColor::DARK_RED:           return ANSIColorCodeForeground::DARK_RED;
        case ANSIColor::DARK_GREEN:         return ANSIColorCodeForeground::DARK_GREEN;
        case ANSIColor::DARK_YELLOW:        return ANSIColorCodeForeground::DARK_YELLOW;
        case ANSIColor::DARK_BLUE:          return ANSIColorCodeForeground::DARK_BLUE;
        case ANSIColor::DARK_MAGENTA:       return ANSIColorCodeForeground::DARK_MAGENTA;
        case ANSIColor::DARK_CYAN:          return ANSIColorCodeForeground::DARK_CYAN;
        case ANSIColor::DARK_WHITE:         return ANSIColorCodeForeground::DARK_WHITE;
        case ANSIColor::BRIGHT_BLACK:       return ANSIColorCodeForeground::BRIGHT_BLACK;
        case ANSIColor::BRIGHT_RED:         return ANSIColorCodeForeground::BRIGHT_RED;
        case ANSIColor::BRIGHT_GREEN:       return ANSIColorCodeForeground::BRIGHT_GREEN;
        case ANSIColor::BRIGHT_YELLOW:      return ANSIColorCodeForeground::BRIGHT_YELLOW;
        case ANSIColor::BRIGHT_BLUE:        return ANSIColorCodeForeground::BRIGHT_BLUE;
        case ANSIColor::BRIGHT_MAGENTA:     return ANSIColorCodeForeground::BRIGHT_MAGENTA;
        case ANSIColor::BRIGHT_CYAN:        return ANSIColorCodeForeground::BRIGHT_CYAN;
        case ANSIColor::BRIGHT_WHITE:       return ANSIColorCodeForeground::BRIGHT_WHITE;
        case ANSIColor::NONE:               throw std::domain_error("This ANSIColor is invalid or has no corresponding foreground color code");
        default:                            throw std::domain_error("Invalid ANSIColor");
    }
}

ANSIColorCodeBackground ansi_color_code_background (ANSIColor ansi_color)
{
    switch (ansi_color)
    {
        case ANSIColor::DARK_BLACK:         return ANSIColorCodeBackground::DARK_BLACK;
        case ANSIColor::DARK_RED:           return ANSIColorCodeBackground::DARK_RED;
        case ANSIColor::DARK_GREEN:         return ANSIColorCodeBackground::DARK_GREEN;
        case ANSIColor::DARK_YELLOW:        return ANSIColorCodeBackground::DARK_YELLOW;
        case ANSIColor::DARK_BLUE:          return ANSIColorCodeBackground::DARK_BLUE;
        case ANSIColor::DARK_MAGENTA:       return ANSIColorCodeBackground::DARK_MAGENTA;
        case ANSIColor::DARK_CYAN:          return ANSIColorCodeBackground::DARK_CYAN;
        case ANSIColor::DARK_WHITE:         return ANSIColorCodeBackground::DARK_WHITE;
        case ANSIColor::BRIGHT_BLACK:       return ANSIColorCodeBackground::BRIGHT_BLACK;
        case ANSIColor::BRIGHT_RED:         return ANSIColorCodeBackground::BRIGHT_RED;
        case ANSIColor::BRIGHT_GREEN:       return ANSIColorCodeBackground::BRIGHT_GREEN;
        case ANSIColor::BRIGHT_YELLOW:      return ANSIColorCodeBackground::BRIGHT_YELLOW;
        case ANSIColor::BRIGHT_BLUE:        return ANSIColorCodeBackground::BRIGHT_BLUE;
        case ANSIColor::BRIGHT_MAGENTA:     return ANSIColorCodeBackground::BRIGHT_MAGENTA;
        case ANSIColor::BRIGHT_CYAN:        return ANSIColorCodeBackground::BRIGHT_CYAN;
        case ANSIColor::BRIGHT_WHITE:       return ANSIColorCodeBackground::BRIGHT_WHITE;
        case ANSIColor::NONE:               throw std::domain_error("This ANSIColor is invalid or has no corresponding background color code");
        default:                            throw std::domain_error("Invalid ANSIColor");
    }
}

std::ostream &operator << (std::ostream &out, ANSIColorCode const &ansi_color_code)
{
    assert(ansi_color_code.m_foreground != ANSIColor::RESET_TO_NORMAL || ansi_color_code.m_background == ANSIColor::NONE);
    out << "\033[";

    bool foreground_set = false;

    if (ansi_color_code.m_foreground == ANSIColor::RESET_TO_NORMAL) {
        out << '0';
        foreground_set = true;
    } else if (ansi_color_code.m_foreground != ANSIColor::NONE) {
        out << uint32_t(ansi_color_code_foreground(ansi_color_code.m_foreground));
        foreground_set = true;
    }

    if (ansi_color_code.m_background != ANSIColor::NONE) {
        if (foreground_set)
            out << ';';
        out << uint32_t(ansi_color_code_background(ansi_color_code.m_background));
    }

    return out << 'm';
}

} // end namespace lvd
