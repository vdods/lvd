// 2014.11.30 - Victor Dods

#pragma once

#include <cassert>
#include <ostream>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace lvd {

// It's recommended to use IndentGuard instead of Indent and Unindent, so that the unindent
// action is guaranteed and easy.
struct Indent { size_t m_count; Indent (size_t count = 1) : m_count(count) { } };
struct Unindent { size_t m_count; Unindent (size_t count = 1) : m_count(count) { } };

struct Prefix
{
    std::string m_text;
    size_t m_log_level;

    Prefix (std::string const &text, size_t log_level)
    :   m_text(text)
    ,   m_log_level(log_level)
    { }
    Prefix (Prefix const &other)
    :   m_text(other.m_text)
    ,   m_log_level(other.m_log_level)
    { }
};

struct PushPrefix { Prefix m_prefix; PushPrefix (Prefix const &p) : m_prefix(p) { } };
struct PopPrefix { };

struct Log;

struct PrefixGuard
{
    Prefix m_prefix;
    mutable Log *m_log;

    explicit PrefixGuard (Log &l, Prefix const &p);
    explicit PrefixGuard (Prefix const &p)
    :   m_prefix(p)
    ,   m_log(nullptr)
    { }
    ~PrefixGuard ();
};

struct IndentGuard
{
    Indent m_indent;
    mutable Log *m_log;

    explicit IndentGuard (Log &l, size_t count = 1);
    explicit IndentGuard (size_t count = 1)
    :   m_indent(count)
    ,   m_log(nullptr)
    { }
    ~IndentGuard ();
};

// This should be used for types T which provide an overload of the form
//
//     Log &operator << (Log &out, T const &t)
//
// because for some reason the function resolution picks the templatized overload in Log over
// a global operator overload.  To specify, define one of these with value = true.
template <typename T_> struct HasCustomLogOutputOverload { static constexpr bool value = false; };

//
// Specific definitions of HasCustomLogOutputOverload for particular types associated with Log.
//

template <> struct HasCustomLogOutputOverload<char>         { static constexpr bool value = true; };
template <> struct HasCustomLogOutputOverload<std::string>  { static constexpr bool value = true; };
template <> struct HasCustomLogOutputOverload<char const *> { static constexpr bool value = true; };
template <> struct HasCustomLogOutputOverload<Indent>       { static constexpr bool value = true; };
template <> struct HasCustomLogOutputOverload<Unindent>     { static constexpr bool value = true; };
template <> struct HasCustomLogOutputOverload<PushPrefix>   { static constexpr bool value = true; };
template <> struct HasCustomLogOutputOverload<PopPrefix>    { static constexpr bool value = true; };
template <> struct HasCustomLogOutputOverload<PrefixGuard>  { static constexpr bool value = true; };
template <> struct HasCustomLogOutputOverload<IndentGuard>  { static constexpr bool value = true; };

// This is a refactor of IndentedFormatter designed to have a simpler and more powerful implementation.
struct Log
{
    explicit Log (std::ostream &out)
    :   m_out(out)
    ,   m_indentation_is_queued(true)
    ,   m_indent_level(0)
    ,   m_log_level_threshold(0)
    {
        m_prefix_stack.emplace_back("", 0);
    }

    static size_t constexpr INDENT_SPACE_COUNT = 4;

    static auto constexpr CRT_PREFIX = "\033[91;41mCRT:\033[0m ";
    static auto constexpr CRT_LEVEL = 600;

    static auto constexpr ERR_PREFIX = "\033[91mERR:\033[0m ";
    static auto constexpr ERR_LEVEL = 500;

    static auto constexpr WRN_PREFIX = "\033[93mWRN:\033[0m ";
    static auto constexpr WRN_LEVEL = 400;

    static auto constexpr INF_PREFIX = "\033[32mINF:\033[0m ";
    static auto constexpr INF_LEVEL = 300;

    static auto constexpr DBG_PREFIX = "\033[36mDBG:\033[0m ";
    static auto constexpr DBG_LEVEL = 200;

    static auto constexpr TRC_PREFIX = "\033[94mTRC:\033[0m ";
    static auto constexpr TRC_LEVEL = 100;

//     // Log level PrefixGuard functions.  Use these like
//     //
//     //     g_log.crt() << "whatever super important message\n";
//     //     g_log.err() << "something bad happened:\n" << IndentGuard() << value << '\n';
//     //
//     // The temporary PrefixGuard will destruct once the statement has executed, and the prefix
//     // will be popped, returning the prefix to its previous state.
//     // TODO: Need to make PrefixGuard (and IndentGuard) be the thing returned from these, and
//     // have it accept operator<< calls.
//
//     Log &crt () & { return *this << PrefixGuard(Prefix(CRT_PREFIX, CRT_LEVEL)); }
//     Log &err () & { return *this << PrefixGuard(Prefix(ERR_PREFIX, ERR_LEVEL)); }
//     Log &wrn () & { return *this << PrefixGuard(Prefix(WRN_PREFIX, WRN_LEVEL)); }
//     Log &inf () & { return *this << PrefixGuard(Prefix(INF_PREFIX, INF_LEVEL)); }
//     Log &dbg () & { return *this << PrefixGuard(Prefix(DBG_PREFIX, DBG_LEVEL)); }
//     Log &trc () & { return *this << PrefixGuard(Prefix(TRC_PREFIX, TRC_LEVEL)); }

    // Log level PrefixGuard functions.  Use these like
    //
    //     g_log << Log::crt() << "whatever super important message\n";
    //     g_log << Log::err() << "something bad happened:\n" << IndentGuard() << value << '\n';
    //
    // The temporary PrefixGuard will destruct once the statement has executed, and the prefix
    // will be popped, returning the prefix to its previous state.

    static PrefixGuard crt () { return PrefixGuard(Prefix(CRT_PREFIX, CRT_LEVEL)); }
    static PrefixGuard err () { return PrefixGuard(Prefix(ERR_PREFIX, ERR_LEVEL)); }
    static PrefixGuard wrn () { return PrefixGuard(Prefix(WRN_PREFIX, WRN_LEVEL)); }
    static PrefixGuard inf () { return PrefixGuard(Prefix(INF_PREFIX, INF_LEVEL)); }
    static PrefixGuard dbg () { return PrefixGuard(Prefix(DBG_PREFIX, DBG_LEVEL)); }
    static PrefixGuard trc () { return PrefixGuard(Prefix(TRC_PREFIX, TRC_LEVEL)); }

    // Accessors and modifiers

    std::ostream &out () const { return m_out; }
    size_t indent_level () const { return m_indent_level; }
    size_t log_level_threshold () const { return m_log_level_threshold; }
    Prefix const &prefix () const { assert(!m_prefix_stack.empty()); return m_prefix_stack.back(); }
    // This is used in LOG_FORMATTED.
    Log &as_reference () { return *this; }

    void indent (size_t count = 1) { m_indent_level += count; }
    void unindent (size_t count = 1) { assert(m_indent_level >= count); m_indent_level -= count; }
    void push_prefix (Prefix const &p) { assert(!m_prefix_stack.empty()); m_prefix_stack.push_back(p); }
    void pop_prefix () { assert(!m_prefix_stack.empty()); if (m_prefix_stack.size() > 1) m_prefix_stack.pop_back(); }
    void set_log_level_threshold (size_t log_level_threshold) { m_log_level_threshold = log_level_threshold; }

    // Make overloads of operator<< for basic char and string types.

    Log &operator << (char c)
    {
        auto const &p = prefix();
        if (p.m_log_level >= m_log_level_threshold)
        {
            if (m_indentation_is_queued)
                m_out << p.m_text << indentation(m_indent_level, INDENT_SPACE_COUNT);
            m_out << c;
        }
        m_indentation_is_queued = c == '\n';
        return *this;
    }
    Log &operator << (std::string const &s)
    {
        // This is not the best possible implementation, but is the simplest.
        for (auto c : s)
            *this << c;
        return *this;
    }
    Log &operator << (char const *s)
    {
        // This is not the best possible implementation, but is the simplest.
        for ( ; *s != '\0'; ++s)
            *this << *s;
        return *this;
    }
    Log &operator << (Indent i)
    {
        indent(i.m_count);
        return *this;
    }
    Log &operator << (Unindent u)
    {
        unindent(u.m_count);
        return *this;
    }
    Log &operator << (PushPrefix const &p)
    {
        push_prefix(p.m_prefix);
        return *this;
    }
    Log &operator << (PopPrefix)
    {
        pop_prefix();
        return *this;
    }
    Log &operator << (PrefixGuard const &g)
    {
        push_prefix(g.m_prefix);
        g.m_log = this;
        return *this;
    }
    Log &operator << (IndentGuard const &g)
    {
        indent(g.m_indent.m_count);
        g.m_log = this;
        return *this;
    }
    // Take anything that goes into std::ostream.  The std::enable_if is needed in order for
    // this template to not take precedence over the above overloads of operator<<.
    // The use of std::remove_reference_t is so that references (e.g. to `std::string const`)
    // are still detected and properly redirected to the appropriate overloads above.
    // Create a template specialization of HasCustomLogOutputOverload in order to specify
    // that another type has an overload of operator<<(Log&, ...) that should be used directly.
    template <typename T_>
    typename std::enable_if_t<
        !HasCustomLogOutputOverload<std::remove_reference_t<T_>>::value,
        Log
    > &operator << (T_&& t)
    {
        std::ostringstream out;
        out << std::forward<T_>(t);
        return *this << out.str();
    }

private:

    static std::string indentation (size_t indent_count, size_t indent_space_count) { return std::string(indent_count*indent_space_count, ' '); }

    std::ostream &m_out;
    bool m_indentation_is_queued;
    size_t m_indent_level;
    size_t m_log_level_threshold;
    std::vector<Prefix> m_prefix_stack;
};

inline PrefixGuard::PrefixGuard (Log &l, Prefix const &p)
:   m_prefix(p)
,   m_log(&l)
{
    m_log->push_prefix(m_prefix);
}

inline PrefixGuard::~PrefixGuard ()
{
    if (m_log != nullptr)
        m_log->pop_prefix();
    m_log = nullptr;
}

inline IndentGuard::IndentGuard (Log &l, size_t count)
:   m_indent(count)
,   m_log(&l)
{
    m_log->indent(m_indent.m_count);
}

inline IndentGuard::~IndentGuard ()
{
    if (m_log != nullptr)
        m_log->unindent(m_indent.m_count);
    m_log = nullptr;
}

} // end namespace lvd
