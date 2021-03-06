// 2020.08.24 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/fmt.hpp"
#include "lvd/Log.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include <sstream>

//
// All these *Thing classes and namespace stuff is to attempt to reproduce this really
// annoying compile error which happens when moving ostream operator<< code into a
// library and putting it inside a namespace.  In certain cases, this causes the overload
// resolution to freak out and not find the intended overload.
//

// To test out custom print to Log
struct HippoThing {
    int m;
    HippoThing (int m_) : m(m_) { }
};

inline lvd::Log &operator << (lvd::Log &out, HippoThing const &x)
{
    return out << "HippoThing{" << x.m << '}';
}

// For the above operator<< to work, the following needs a definition with value = true.
template <> struct lvd::HasCustomLogOutputOverload<HippoThing> : std::true_type { };

// To test out custom print to Log
struct OstrichThing {
    int m;
    OstrichThing (int m_) : m(m_) { }
};

inline std::ostream &operator << (std::ostream &out, OstrichThing const &x)
{
    return out << "OstrichThing{" << x.m << '}';
}

namespace aaa {

struct DonkeyThing {
    int m;
    DonkeyThing (int m_) : m(m_) { }
};

// Requires lvd::HasCustomLogOutputOverload<aaa::DonkeyThing> specialization.
inline lvd::Log &operator << (lvd::Log &out, DonkeyThing const &x)
{
    return out << "DonkeyThing[LogOverload]{" << x.m << '}';
}

inline std::ostream &operator << (std::ostream &out, DonkeyThing const &x) // Overload A
{
    return out << "DonkeyThing{" << x.m << '}';
}

} // end namespace aaa

// This is needed for the operator<< overload for Log to work.  This can't be inside a namespace.
template <> struct lvd::HasCustomLogOutputOverload<aaa::DonkeyThing> : std::true_type { };

// NOTE TO PROGRAMMER: This kind of overload (e.g. for aaa::DonkeyThing) HAS to go within the
// namespace in which the type is declared, which in this case is aaa.  In a `namespace aaa { ... }`
// block itself, and not just qualified with `aaa::`.  Thus if you were to comment out Overload A
// above and uncomment either Overload B or Overload C, it would fail to compile, and worse yet,
// the error that Overload C produces gives no hint as to what the problem is (the error for
// Overload B seems to give a direct explanation, at least in g++).
//
// inline std::ostream &aaa::operator << (std::ostream &out, DonkeyThing const &x) // Overload B
// {
//     return out << "DonkeyThing{" << x.m << '}';
// }
//
// inline std::ostream &operator << (std::ostream &out, aaa::DonkeyThing const &x) // Overload C
// {
//     return out << "DonkeyThing{" << x.m << '}';
// }

LVD_TEST_BEGIN(300__Log__00)
    std::ostringstream out;
    lvd::Log log(out);
    log << "blah\n" << lvd::IndentGuard() << "hippo\n";
    log << "thingy";
    LVD_TEST_REQ_EQ(out.str(), "blah\n    hippo\nthingy");
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__01)
    std::ostringstream out;
    lvd::Log log(out);
    HippoThing x{123};
    log << x;
    LVD_TEST_REQ_EQ(out.str(), "HippoThing{123}");
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__02)
    std::ostringstream out;
    lvd::Log log(out);
    OstrichThing x{123};
    log << x;
    LVD_TEST_REQ_EQ(out.str(), "OstrichThing{123}");
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__03)
    std::ostringstream out;
    aaa::DonkeyThing x{123};
    out << x;
    LVD_TEST_REQ_EQ(out.str(), "DonkeyThing{123}");
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__04)
    std::ostringstream out;
    lvd::Log log(out);
    aaa::DonkeyThing x{123};
    log << x;
    LVD_TEST_REQ_EQ(out.str(), "DonkeyThing[LogOverload]{123}");
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__nil)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::nil() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::NIL)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__trc)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::trc() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::TRC)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__dbg)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::dbg() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::DBG)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__inf)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::inf() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::INF)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__wrn)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::wrn() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::WRN)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__err)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::err() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::ERR)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__10__LogLevel__crt)
    std::ostringstream out;
    lvd::Log log(out);
    std::string message{"hippo mom"};
    log << lvd::Log::crt() << message;
    LVD_TEST_REQ_EQ(out.str(), lvd::prefix_text(lvd::LogLevel::CRT)+message);
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__70__histogram)
    std::ostringstream out;
    lvd::Log log(out);
    log << lvd::Log::trc() << "blah\n";
    log << lvd::Log::dbg() << "thingy stuff\n";
    log << lvd::Log::dbg() << "uuustuff\n";
    log << lvd::Log::wrn() << "warning fancy face\n";
    LVD_TEST_REQ_EQ(log.log_level_histogram(), lvd::LogLevelHistogram(0,1,2,0,1,0,0));
LVD_TEST_END

void test_Log_ANSIColor_case (lvd::req::Context &req_context, lvd::ANSIColor fg, lvd::ANSIColor bg, std::string const &expected_string_) {
    auto expected_string = expected_string_ + "A MAD HIPPO IS A GLAD HIPPO\033[0m";

    // Try two ways -- one using Log directly, the other using LVD_LOG_FMT, which is a fancy macro.
    {
        std::ostringstream out;
        lvd::Log log(out);
        log << lvd::ANSIColorCode(fg, bg) << "A MAD HIPPO IS A GLAD HIPPO" << lvd::ANSIColorCode(lvd::ANSIColor::RESET_TO_NORMAL);
        auto s = out.str();
        req_context.log() << lvd::Log::dbg() << s << '\n';
        LVD_TEST_REQ_EQ(s, expected_string);
    }
    {
        auto s = LVD_LOG_FMT(lvd::ANSIColorCode(fg, bg) << "A MAD HIPPO IS A GLAD HIPPO" << lvd::ANSIColorCode(lvd::ANSIColor::RESET_TO_NORMAL));
        LVD_TEST_REQ_EQ(s, expected_string);
    }
}

#define TEST_LOG_ANSI_COLOR_CASE(fg, bg, expected_string) test_Log_ANSIColor_case(req_context, fg, bg, expected_string);

LVD_TEST_BEGIN(300__Log__80__ANSIColorCode)
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::RESET_TO_NORMAL, lvd::ANSIColor::NONE, "\033[0m");

    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_BLACK, lvd::ANSIColor::NONE, "\033[30m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_RED, lvd::ANSIColor::NONE, "\033[31m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_GREEN, lvd::ANSIColor::NONE, "\033[32m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_YELLOW, lvd::ANSIColor::NONE, "\033[33m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_BLUE, lvd::ANSIColor::NONE, "\033[34m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_MAGENTA, lvd::ANSIColor::NONE, "\033[35m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_CYAN, lvd::ANSIColor::NONE, "\033[36m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_WHITE, lvd::ANSIColor::NONE, "\033[37m");

    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_BLACK, lvd::ANSIColor::NONE, "\033[90m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_RED, lvd::ANSIColor::NONE, "\033[91m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_GREEN, lvd::ANSIColor::NONE, "\033[92m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_YELLOW, lvd::ANSIColor::NONE, "\033[93m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_BLUE, lvd::ANSIColor::NONE, "\033[94m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_MAGENTA, lvd::ANSIColor::NONE, "\033[95m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_CYAN, lvd::ANSIColor::NONE, "\033[96m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_WHITE, lvd::ANSIColor::NONE, "\033[97m");

    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_BLACK, "\033[40m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_RED, "\033[41m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_GREEN, "\033[42m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_YELLOW, "\033[43m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_BLUE, "\033[44m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_MAGENTA, "\033[45m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_CYAN, "\033[46m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_WHITE, "\033[47m");

    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_BLACK, "\033[100m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_RED, "\033[101m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_GREEN, "\033[102m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_YELLOW, "\033[103m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_BLUE, "\033[104m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_MAGENTA, "\033[105m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_CYAN, "\033[106m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_WHITE, "\033[107m");

    // A few select combos
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_BLACK, lvd::ANSIColor::BRIGHT_RED, "\033[30;101m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_RED, lvd::ANSIColor::DARK_BLACK, "\033[91;40m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_WHITE, lvd::ANSIColor::BRIGHT_CYAN, "\033[97;106m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_RED, lvd::ANSIColor::DARK_BLUE, "\033[31;44m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_BLUE, lvd::ANSIColor::DARK_RED, "\033[34;41m");
LVD_TEST_END

void test_Log_ANSIColorGuard_case (lvd::req::Context &req_context, lvd::ANSIColor fg, lvd::ANSIColor bg, std::string const &expected_string_) {
    auto expected_string = expected_string_ + "A MAD HIPPO IS A GLAD HIPPO\033[0m";

    // Try two ways -- one using Log directly, the other using LVD_LOG_FMT, which is a fancy macro.
    {
        std::ostringstream out;
        lvd::Log log(out);
        log << lvd::ANSIColorGuard<lvd::Log>(fg, bg) << "A MAD HIPPO IS A GLAD HIPPO";
        auto s = out.str();
        req_context.log() << lvd::Log::dbg() << s << '\n';
        LVD_TEST_REQ_EQ(s, expected_string);
    }
    {
        auto s = LVD_LOG_FMT(lvd::ANSIColorGuard<lvd::Log>(fg, bg) << "A MAD HIPPO IS A GLAD HIPPO");
        req_context.log() << lvd::Log::dbg() << s << '\n';
        LVD_TEST_REQ_EQ(s, expected_string);
    }
}

#define TEST_LOG_ANSI_COLOR_CASE(fg, bg, expected_string) test_Log_ANSIColor_case(req_context, fg, bg, expected_string);

LVD_TEST_BEGIN(300__Log__90__ANSIColorGuard__00)
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::RESET_TO_NORMAL, lvd::ANSIColor::NONE, "\033[0m");

    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_BLACK, lvd::ANSIColor::NONE, "\033[30m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_RED, lvd::ANSIColor::NONE, "\033[31m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_GREEN, lvd::ANSIColor::NONE, "\033[32m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_YELLOW, lvd::ANSIColor::NONE, "\033[33m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_BLUE, lvd::ANSIColor::NONE, "\033[34m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_MAGENTA, lvd::ANSIColor::NONE, "\033[35m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_CYAN, lvd::ANSIColor::NONE, "\033[36m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_WHITE, lvd::ANSIColor::NONE, "\033[37m");

    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_BLACK, lvd::ANSIColor::NONE, "\033[90m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_RED, lvd::ANSIColor::NONE, "\033[91m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_GREEN, lvd::ANSIColor::NONE, "\033[92m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_YELLOW, lvd::ANSIColor::NONE, "\033[93m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_BLUE, lvd::ANSIColor::NONE, "\033[94m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_MAGENTA, lvd::ANSIColor::NONE, "\033[95m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_CYAN, lvd::ANSIColor::NONE, "\033[96m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_WHITE, lvd::ANSIColor::NONE, "\033[97m");

    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_BLACK, "\033[40m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_RED, "\033[41m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_GREEN, "\033[42m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_YELLOW, "\033[43m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_BLUE, "\033[44m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_MAGENTA, "\033[45m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_CYAN, "\033[46m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_WHITE, "\033[47m");

    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_BLACK, "\033[100m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_RED, "\033[101m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_GREEN, "\033[102m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_YELLOW, "\033[103m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_BLUE, "\033[104m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_MAGENTA, "\033[105m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_CYAN, "\033[106m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_WHITE, "\033[107m");

    // A few select combos
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_BLACK, lvd::ANSIColor::BRIGHT_RED, "\033[30;101m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_RED, lvd::ANSIColor::DARK_BLACK, "\033[91;40m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_WHITE, lvd::ANSIColor::BRIGHT_CYAN, "\033[97;106m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_RED, lvd::ANSIColor::DARK_BLUE, "\033[31;44m");
    TEST_LOG_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_BLUE, lvd::ANSIColor::DARK_RED, "\033[34;41m");
LVD_TEST_END

LVD_TEST_BEGIN(300__Log__90__ANSIColorGuard__01)
    std::ostringstream out;
    lvd::Log log(out);

    log << lvd::Log::dbg() << "my favorite phrase is:\n" << lvd::IndentGuard() << lvd::ANSIColorGuard<lvd::Log>(lvd::ANSIColor::BRIGHT_WHITE, lvd::ANSIColor::DARK_BLUE) << "HIPPO";
    log << '\n'; // NOTE: For now, this has to be printed after the ANSIColorGuard is destroyed.

    test_log << out.str();

    LVD_TEST_REQ_EQ(out.str(), "\033[36mDBG:\033[0m my favorite phrase is:\n\033[36mDBG:\033[0m     \033[97;44mHIPPO\033[0m\n");
LVD_TEST_END
