// 2019.12.17 - Victor Dods

#pragma once

#include "lvd/fmt.hpp"
#include <stdexcept>

namespace lvd {

enum class ANSIColor : uint32_t
{
    RESET_TO_NORMAL = 0,

    DARK_BLACK,
    DARK_RED,
    DARK_GREEN,
    DARK_YELLOW,
    DARK_BLUE,
    DARK_MAGENTA,
    DARK_CYAN,
    DARK_WHITE,

    BRIGHT_BLACK,
    BRIGHT_RED,
    BRIGHT_GREEN,
    BRIGHT_YELLOW,
    BRIGHT_BLUE,
    BRIGHT_MAGENTA,
    BRIGHT_CYAN,
    BRIGHT_WHITE,

    NONE,

    __LOWEST__ = RESET_TO_NORMAL,
    __HIGHEST__ = NONE,

    // Duplicates of the above that use more sensible names.

    BLACK           = DARK_BLACK,
    RED             = DARK_RED,
    GREEN           = DARK_GREEN,
    BROWN           = DARK_YELLOW,
    BLUE            = DARK_BLUE,
    MAGENTA         = DARK_MAGENTA,
    CYAN            = DARK_CYAN,
    BRIGHT_GREY     = DARK_WHITE,
    DARK_GREY       = BRIGHT_BLACK,
    YELLOW          = BRIGHT_YELLOW,
    WHITE           = BRIGHT_WHITE
};

struct ANSIColorCode
{
    ANSIColor m_foreground;
    ANSIColor m_background;

    ANSIColorCode (ANSIColor foreground, ANSIColor background = ANSIColor::NONE)
    :   m_foreground(foreground)
    ,   m_background(background)
    {
        if (background == ANSIColor::RESET_TO_NORMAL)
            throw std::domain_error("background must not be ANSIColor::RESET_TO_NORMAL");
        if (foreground == ANSIColor::RESET_TO_NORMAL && background != ANSIColor::NONE)
            throw std::domain_error("if foreground is ANSIColor::RESET_TO_NORMAL then background must be ANSIColor::NONE");
    }
};

// Print an ANSIColorCode as an escape sequence that looks like "\033[X;Ym".
std::ostream &operator << (std::ostream &out, ANSIColorCode const &ansi_color_code);

// template <typename GuardedType_, typename StreamType_>
// struct Guard
// {
//     GuardedType_ m_guarded_thing;
//     mutable StreamType_ *m_stream;
//
//     template <typename... Args_>
//     Guard (Args_&&... args)
//     :   m_guarded_thing(std::forward<Args_>(args)...)
//     ,   m_stream(nullptr)
//     { }
//     ~Guard ()
//     {
//         if (m_stream != nullptr)
//         {
//             // DO STUFF
//         }
//     }
// };

template <typename T_>
struct ANSIColorGuard
{
    ANSIColorCode m_ansi_color_code;
    mutable T_ *m_out;

    template <typename... Args_>
    explicit ANSIColorGuard (Args_&&... args)
    :   m_ansi_color_code(std::forward<Args_>(args)...)
    ,   m_out(nullptr)
    { }
    ~ANSIColorGuard ()
    {
        if (m_out != nullptr)
            *m_out << ANSIColorCode(ANSIColor::RESET_TO_NORMAL);
        m_out = nullptr;
    }
};

template <typename T_>
auto &operator << (T_ &out, ANSIColorGuard<T_> const &g)
{
    g.m_out = &out;
    return out << g.m_ansi_color_code;
}

} // end namespace lvd
