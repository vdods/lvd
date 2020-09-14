// 2020.09.13 - Victor Dods

#include "lvd/ANSIColor.hpp"
#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include <sstream>

LVD_TEST_BEGIN(250__ANSIColor__00)
    std::ostringstream out;
    out << lvd::ANSIColorCode(lvd::ANSIColor::RESET_TO_NORMAL) << "blah";
    test_log << lvd::Log::dbg() << out.str() << '\n';
    LVD_TEST_REQ_EQ(out.str(), "\033[0mblah");
LVD_TEST_END

void test_ANSIColorCode (lvd::req::Context &req_context, lvd::ANSIColor fg, lvd::ANSIColor bg, std::string const &expected_string) {
    auto s = LVD_FMT(lvd::ANSIColorCode(fg, bg));
    req_context.log() << lvd::Log::dbg() << s << "A MAD HIPPO IS A GLAD HIPPO\033[0m\n";
    LVD_TEST_REQ_EQ(s, expected_string);
}

#define TEST_ANSI_COLOR_CASE(fg, bg, expected_string) test_ANSIColorCode(req_context, fg, bg, expected_string);

LVD_TEST_BEGIN(250__ANSIColor__01__ANSIColorCode)
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::RESET_TO_NORMAL, lvd::ANSIColor::NONE, "\033[0m");

    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_BLACK, lvd::ANSIColor::NONE, "\033[30m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_RED, lvd::ANSIColor::NONE, "\033[31m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_GREEN, lvd::ANSIColor::NONE, "\033[32m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_YELLOW, lvd::ANSIColor::NONE, "\033[33m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_BLUE, lvd::ANSIColor::NONE, "\033[34m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_MAGENTA, lvd::ANSIColor::NONE, "\033[35m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_CYAN, lvd::ANSIColor::NONE, "\033[36m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_WHITE, lvd::ANSIColor::NONE, "\033[37m");

    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_BLACK, lvd::ANSIColor::NONE, "\033[90m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_RED, lvd::ANSIColor::NONE, "\033[91m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_GREEN, lvd::ANSIColor::NONE, "\033[92m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_YELLOW, lvd::ANSIColor::NONE, "\033[93m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_BLUE, lvd::ANSIColor::NONE, "\033[94m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_MAGENTA, lvd::ANSIColor::NONE, "\033[95m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_CYAN, lvd::ANSIColor::NONE, "\033[96m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_WHITE, lvd::ANSIColor::NONE, "\033[97m");

    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_BLACK, "\033[40m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_RED, "\033[41m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_GREEN, "\033[42m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_YELLOW, "\033[43m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_BLUE, "\033[44m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_MAGENTA, "\033[45m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_CYAN, "\033[46m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_WHITE, "\033[47m");

    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_BLACK, "\033[100m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_RED, "\033[101m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_GREEN, "\033[102m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_YELLOW, "\033[103m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_BLUE, "\033[104m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_MAGENTA, "\033[105m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_CYAN, "\033[106m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_WHITE, "\033[107m");

    // A few select combos
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_BLACK, lvd::ANSIColor::BRIGHT_RED, "\033[30;101m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_RED, lvd::ANSIColor::DARK_BLACK, "\033[91;40m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::BRIGHT_WHITE, lvd::ANSIColor::BRIGHT_CYAN, "\033[97;106m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_RED, lvd::ANSIColor::DARK_BLUE, "\033[31;44m");
    TEST_ANSI_COLOR_CASE(lvd::ANSIColor::DARK_BLUE, lvd::ANSIColor::DARK_RED, "\033[34;41m");
LVD_TEST_END

void test_ANSIColorGuard (lvd::req::Context &req_context, lvd::ANSIColor fg, lvd::ANSIColor bg, std::string const &expected_string) {
    std::ostringstream out;
    out << lvd::ANSIColorGuard<std::ostringstream>(fg, bg) << "A MAD HIPPO IS A GLAD HIPPO";

//     auto s = LVD_FMT(lvd::ANSIColorGuard<std::ostream>(fg, bg) << "A MAD HIPPO IS A GLAD HIPPO");
    auto s = out.str();
    req_context.log() << lvd::Log::dbg() << s << '\n';
    LVD_TEST_REQ_EQ(s, expected_string+"A MAD HIPPO IS A GLAD HIPPO\033[0m");
}

#define TEST_ANSI_COLOR_GUARD(fg, bg, expected_string) test_ANSIColorGuard(req_context, fg, bg, expected_string);

LVD_TEST_BEGIN(250__ANSIColor__02__ANSIColorGuard)
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::RESET_TO_NORMAL, lvd::ANSIColor::NONE, "\033[0m");

    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::DARK_BLACK, lvd::ANSIColor::NONE, "\033[30m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::DARK_RED, lvd::ANSIColor::NONE, "\033[31m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::DARK_GREEN, lvd::ANSIColor::NONE, "\033[32m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::DARK_YELLOW, lvd::ANSIColor::NONE, "\033[33m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::DARK_BLUE, lvd::ANSIColor::NONE, "\033[34m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::DARK_MAGENTA, lvd::ANSIColor::NONE, "\033[35m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::DARK_CYAN, lvd::ANSIColor::NONE, "\033[36m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::DARK_WHITE, lvd::ANSIColor::NONE, "\033[37m");

    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::BRIGHT_BLACK, lvd::ANSIColor::NONE, "\033[90m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::BRIGHT_RED, lvd::ANSIColor::NONE, "\033[91m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::BRIGHT_GREEN, lvd::ANSIColor::NONE, "\033[92m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::BRIGHT_YELLOW, lvd::ANSIColor::NONE, "\033[93m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::BRIGHT_BLUE, lvd::ANSIColor::NONE, "\033[94m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::BRIGHT_MAGENTA, lvd::ANSIColor::NONE, "\033[95m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::BRIGHT_CYAN, lvd::ANSIColor::NONE, "\033[96m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::BRIGHT_WHITE, lvd::ANSIColor::NONE, "\033[97m");

    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_BLACK, "\033[40m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_RED, "\033[41m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_GREEN, "\033[42m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_YELLOW, "\033[43m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_BLUE, "\033[44m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_MAGENTA, "\033[45m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_CYAN, "\033[46m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::DARK_WHITE, "\033[47m");

    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_BLACK, "\033[100m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_RED, "\033[101m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_GREEN, "\033[102m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_YELLOW, "\033[103m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_BLUE, "\033[104m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_MAGENTA, "\033[105m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_CYAN, "\033[106m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::NONE, lvd::ANSIColor::BRIGHT_WHITE, "\033[107m");

    // A few select combos
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::DARK_BLACK, lvd::ANSIColor::BRIGHT_RED, "\033[30;101m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::BRIGHT_RED, lvd::ANSIColor::DARK_BLACK, "\033[91;40m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::BRIGHT_WHITE, lvd::ANSIColor::BRIGHT_CYAN, "\033[97;106m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::DARK_RED, lvd::ANSIColor::DARK_BLUE, "\033[31;44m");
    TEST_ANSI_COLOR_GUARD(lvd::ANSIColor::DARK_BLUE, lvd::ANSIColor::DARK_RED, "\033[34;41m");
LVD_TEST_END
