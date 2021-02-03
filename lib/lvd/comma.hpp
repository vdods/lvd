// 2021.01.24 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <ostream>

namespace lvd {

// For convenience in output.
struct Comma { };

// Convenient singleton instance of Comma.
inline static Comma const comma = Comma();

inline std::ostream &operator<< (std::ostream &out, Comma) {
    return out << ',';
}

// TODO: Could create operator>> that expects a comma or throws


// For convenience in output.
struct CommaSpace { };

// Convenient singleton instance of CommaSpace.
inline static CommaSpace const cspace = CommaSpace();

inline std::ostream &operator<< (std::ostream &out, CommaSpace) {
    return out << ", ";
}

// TODO: Could create operator>> that expects a comma and then whitespace or throws

// Makes it easier to write code that prints e.g. comma-delimited sequences.
template <typename T_>
class InfixDelimiter_t {
public:

    InfixDelimiter_t () = delete;
    template <typename Delimiter_>
    InfixDelimiter_t (Delimiter_ &&delimiter)
        :   m_has_printed_delimiter(false)
        ,   m_delimiter(std::forward<Delimiter_>(delimiter))
    { }

    std::ostream &print (std::ostream &out) {
        if (m_has_printed_delimiter)
            out << m_delimiter;
        else
            m_has_printed_delimiter = true;
        return out;
    }

private:

    bool m_has_printed_delimiter;
    T_ m_delimiter;
};

template <typename T_>
inline std::ostream &operator<< (std::ostream &out, InfixDelimiter_t<T_> &d) {
    return d.print(out);
}

//
// Some convenient factory functions
//

inline decltype(auto) make_comma_delimiter () {
    return InfixDelimiter_t<char>(',');
}

inline decltype(auto) make_comma_space_delimiter () {
    return InfixDelimiter_t<std::string>(", ");
}

} // end namespace lvd
