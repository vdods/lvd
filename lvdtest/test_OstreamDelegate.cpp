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

LVD_REGISTER_TEST(000__OstreamDelegate__0, ([](){
    auto x = Ostrich{1, 2, 4, 8};
    std::ostringstream string_out;
    string_out << x;
    LVD_REQ_EQ(string_out.str(), "Ostrich(1, 2, 4, 8)");
}));
