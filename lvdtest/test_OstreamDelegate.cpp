// 2020.03.29 - Victor Dods

#include <iostream>
#include "lvd/OstreamDelegate.hpp"
#include "lvd/req.hpp"
#include "lvd/Test.hpp"
#include <sstream>
#include <vector>

class Ostrich {
public:

    Ostrich (Ostrich const &) = default;
    Ostrich (Ostrich &&) = default;
    template <typename... Args_>
    Ostrich (Args_&&... args) : m_stuff{std::forward<Args_>(args)...} { }

    std::vector<int> const &stuff () const { return m_stuff; }

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "Ostrich(";
            for (size_t i = 0; i < m_stuff.size(); ++i) {
                out << m_stuff[i];
                if (i+1 < m_stuff.size())
                    out << ", ";
            }
            out << ')';
        });
    }

private:

    std::vector<int> m_stuff;
};

void print_Ostrich (std::ostream &out, Ostrich const &ostrich) {
    out << ostrich;
}

// There has been trouble with operator overload resolution when namespaces are involved,
// so this code is to address that.
namespace hippo {

class Donkey {
public:

    Donkey (Donkey const &) = default;
    Donkey (Donkey &&) = default;
    template <typename... Args_>
    Donkey (Args_&&... args) : m_stuff{std::forward<Args_>(args)...} { }

    std::vector<int> const &stuff () const { return m_stuff; }

    operator lvd::OstreamDelegate () const {
        return lvd::OstreamDelegate::OutFunc([this](std::ostream &out){
            out << "Donkey(";
            for (size_t i = 0; i < m_stuff.size(); ++i) {
                out << m_stuff[i];
                if (i+1 < m_stuff.size())
                    out << ", ";
            }
            out << ')';
        });
    }

private:

    std::vector<int> m_stuff;
};

void print_Ostrich_in_hippo (std::ostream &out, Ostrich const &ostrich) {
    out << ostrich;
}

void print_hippo_Donkey_in_hippo (std::ostream &out, Donkey const &donkey) {
    out << donkey;
}

} // end namespace hippo

void print_hippo_Donkey (std::ostream &out, hippo::Donkey const &donkey) {
    out << donkey;
}

namespace platypus {

void print_Ostrich_in_platypus (std::ostream &out, Ostrich const &ostrich) {
    out << ostrich;
}

void print_hippo_Donkey_in_platypus (std::ostream &out, hippo::Donkey const &donkey) {
    out << donkey;
}

} // end namespace platypus

LVD_TEST_BEGIN(000__OstreamDelegate__0__0)
    auto x = Ostrich{1, 2, 4, 8};
    std::ostringstream string_out;
    string_out << x;
    LVD_REQ_EQ(string_out.str(), "Ostrich(1, 2, 4, 8)");
LVD_TEST_END

LVD_TEST_BEGIN(000__OstreamDelegate__0__1)
    auto x = Ostrich{1, 2, 4, 8};
    std::ostringstream string_out;
    print_Ostrich(string_out, x);
    LVD_REQ_EQ(string_out.str(), "Ostrich(1, 2, 4, 8)");
LVD_TEST_END

LVD_TEST_BEGIN(000__OstreamDelegate__0__2)
    auto x = Ostrich{1, 2, 4, 8};
    std::ostringstream string_out;
    hippo::print_Ostrich_in_hippo(string_out, x);
    LVD_REQ_EQ(string_out.str(), "Ostrich(1, 2, 4, 8)");
LVD_TEST_END

LVD_TEST_BEGIN(000__OstreamDelegate__0__3)
    auto x = Ostrich{1, 2, 4, 8};
    std::ostringstream string_out;
    platypus::print_Ostrich_in_platypus(string_out, x);
    LVD_REQ_EQ(string_out.str(), "Ostrich(1, 2, 4, 8)");
LVD_TEST_END

LVD_TEST_BEGIN(000__OstreamDelegate__1__0)
    auto x = hippo::Donkey{1, 2, 4, 8};
    std::ostringstream string_out;
    string_out << x;
    LVD_REQ_EQ(string_out.str(), "Donkey(1, 2, 4, 8)");
LVD_TEST_END

LVD_TEST_BEGIN(000__OstreamDelegate__1__1)
    auto x = hippo::Donkey{1, 2, 4, 8};
    std::ostringstream string_out;
    print_hippo_Donkey(string_out, x);
    LVD_REQ_EQ(string_out.str(), "Donkey(1, 2, 4, 8)");
LVD_TEST_END

LVD_TEST_BEGIN(000__OstreamDelegate__1__2)
    auto x = hippo::Donkey{1, 2, 4, 8};
    std::ostringstream string_out;
    hippo::print_hippo_Donkey_in_hippo(string_out, x);
    LVD_REQ_EQ(string_out.str(), "Donkey(1, 2, 4, 8)");
LVD_TEST_END

LVD_TEST_BEGIN(000__OstreamDelegate__1__3)
    auto x = hippo::Donkey{1, 2, 4, 8};
    std::ostringstream string_out;
    platypus::print_hippo_Donkey_in_platypus(string_out, x);
    LVD_REQ_EQ(string_out.str(), "Donkey(1, 2, 4, 8)");
LVD_TEST_END
