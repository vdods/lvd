// 2021.02.09 - Copyright Victor Dods - Licensed under Apache 2.0

#include "lvd/req.hpp"
#include "lvd/test.hpp"
#include <string>

//
// This is a playground for testing that function overload resolution works as expected.
//

namespace blah {

struct Thingy { };

std::string operator* (Thingy) { return "::blah -- got Thingy"; }
// std::string fancy_function (Thingy) { return "::blah -- got Thingy"; }

struct Thangy : public Thingy { };

} // end namespace blah

struct Hippo { };

std::string operator* (Hippo) { return ":: -- got Hippo"; }
// std::string fancy_function (Hippo) { return ":: -- got Hippo"; }

struct Hippa : public blah::Thingy { };

namespace donkey {

struct Ostrich { };

struct Emu : public blah::Thingy { };

std::string operator* (Ostrich) { return "::donkey -- got Ostrich"; }
// std::string fancy_function (Ostrich) { return "::donkey -- got Ostrich"; }

} // end namespace donkey

LVD_TEST_BEGIN(666__overload_resolution__00__in_namespace_global)
    auto thi = blah::Thingy{};
    auto tha = blah::Thangy{};
    auto hippo = Hippo{};
    auto hippa = Hippa{};
    auto ost = donkey::Ostrich{};
    auto emu = donkey::Emu{};

    test_log << lvd::Log::dbg() << LVD_REFLECT(*thi) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*tha) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*hippo) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*hippa) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*ost) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*emu) << '\n';
LVD_TEST_END

namespace blah {

LVD_TEST_BEGIN(666__overload_resolution__01__in_namespace_blah)
    auto thi = blah::Thingy{};
    auto tha = blah::Thangy{};
    auto hippo = Hippo{};
    auto hippa = Hippa{};
    auto ost = donkey::Ostrich{};
    auto emu = donkey::Emu{};

    test_log << lvd::Log::dbg() << LVD_REFLECT(*thi) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*tha) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*hippo) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*hippa) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*ost) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*emu) << '\n';
LVD_TEST_END

} // end namespace blah

namespace donkey {

LVD_TEST_BEGIN(666__overload_resolution__02__in_namespace_donkey)
    auto thi = blah::Thingy{};
    auto tha = blah::Thangy{};
    auto hippo = Hippo{};
    auto hippa = Hippa{};
    auto ost = donkey::Ostrich{};
    auto emu = donkey::Emu{};

    test_log << lvd::Log::dbg() << LVD_REFLECT(*thi) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*tha) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*hippo) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*hippa) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*ost) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*emu) << '\n';
LVD_TEST_END

} // end namespace donkey

namespace something_completely_different {

LVD_TEST_BEGIN(666__overload_resolution__03__in_namespace_something_completely_different)
    auto thi = blah::Thingy{};
    auto tha = blah::Thangy{};
    auto hippo = Hippo{};
    auto hippa = Hippa{};
    auto ost = donkey::Ostrich{};
    auto emu = donkey::Emu{};

    test_log << lvd::Log::dbg() << LVD_REFLECT(*thi) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*tha) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*hippo) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*hippa) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*ost) << '\n';
    test_log << lvd::Log::dbg() << LVD_REFLECT(*emu) << '\n';
LVD_TEST_END

} // end namespace something_completely_different
