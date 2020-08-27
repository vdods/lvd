// 2014.11.30 - Victor Dods

#pragma once

#include <array>
#include <cassert>
#include <experimental/array>
#include <numeric>
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

enum class LogLevel : uint8_t
{
    NIL = 0,
    TRC,
    DBG,
    INF,
    WRN,
    ERR,
    CRT,

    __LOWEST__ = NIL,
    __HIGHEST__ = CRT
};

auto constexpr LOG_LEVEL_COUNT = size_t(LogLevel::__HIGHEST__) - size_t(LogLevel::__LOWEST__) + 1;

inline std::string const &as_string (LogLevel ll)
{
    static std::string const TABLE[LOG_LEVEL_COUNT] = {
        "NIL",
        "TRC",
        "DBG",
        "INF",
        "WRN",
        "ERR",
        "CRT",
    };
    return TABLE[uint32_t(ll)];
}

inline std::string const &prefix_text (LogLevel ll)
{
    static std::string const TABLE[LOG_LEVEL_COUNT] = {
        "\033[0m", // Reset color, no prefix text.
        "\033[94mTRC:\033[0m ",
        "\033[36mDBG:\033[0m ",
        "\033[32mINF:\033[0m ",
        "\033[93mWRN:\033[0m ",
        "\033[91mERR:\033[0m ",
        "\033[91;41mCRT:\033[0m ",
    };
    return TABLE[uint32_t(ll)];
}

// LogLevelHistogram provides a way to count the number of messages of each LogLevel were
// printed to a log.  Really this means how many times a PrefixGuard of a given LogLevel
// was <<'ed into the Log object.
class LogLevelHistogram
{
public:

    using Buckets = std::array<size_t,LOG_LEVEL_COUNT>;

    LogLevelHistogram () { clear(); }
    LogLevelHistogram (LogLevelHistogram const &) = default;
    LogLevelHistogram (LogLevelHistogram &&) = default;
    template <typename... Args_>
    LogLevelHistogram (Args_&&... args) : m_buckets(std::experimental::make_array<size_t>(std::forward<Args_>(args)...)) {  }

    bool operator == (LogLevelHistogram const &other) const { return m_buckets == other.m_buckets; }

    size_t bucket (LogLevel ll) const { return m_buckets[uint32_t(ll)]; }
    size_t &bucket (LogLevel ll) { return m_buckets[uint32_t(ll)]; }
    Buckets const &buckets () const { return m_buckets; }
    Buckets &buckets () { return m_buckets; }
    size_t bucket_total () const { return std::accumulate(m_buckets.begin(), m_buckets.end(), size_t(0)); }

    void increment (LogLevel ll) { bucket(ll) += 1; }
    void clear () { m_buckets.fill(0); }

private:

    Buckets m_buckets;
};

inline std::ostream &operator << (std::ostream &out, LogLevelHistogram const &h)
{
    out << "LogLevelHistogram with bucket_total " << h.bucket_total() << " {\n";
    for (size_t i = 0; i < LOG_LEVEL_COUNT; ++i)
    {
        auto ll = LogLevel(i);
        out << "    " << as_string(ll) << ": " << h.bucket(ll) << '\n';
    }
    return out << "}\n";
}

struct Prefix
{
    std::string m_text;
    LogLevel m_log_level;

    Prefix (std::string const &text, LogLevel log_level)
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
// a global operator overload.  To specify, inherit std::true_type.
template <typename T_> struct HasCustomLogOutputOverload : std::false_type { };

//
// Specific definitions of HasCustomLogOutputOverload for particular types associated with Log.
//

template <> struct HasCustomLogOutputOverload<char> : std::true_type { };
template <> struct HasCustomLogOutputOverload<std::string> : std::true_type { };
template <> struct HasCustomLogOutputOverload<char const *> : std::true_type { };
template <> struct HasCustomLogOutputOverload<Indent> : std::true_type { };
template <> struct HasCustomLogOutputOverload<Unindent> : std::true_type { };
template <> struct HasCustomLogOutputOverload<PushPrefix> : std::true_type { };
template <> struct HasCustomLogOutputOverload<PopPrefix> : std::true_type { };
template <> struct HasCustomLogOutputOverload<PrefixGuard> : std::true_type { };
template <> struct HasCustomLogOutputOverload<IndentGuard> : std::true_type { };

// This is a refactor of IndentedFormatter designed to have a simpler and more powerful implementation.
struct Log
{
    explicit Log (std::ostream &out)
    :   m_out(out)
    ,   m_indentation_is_queued(true)
    ,   m_indent_level(0)
    ,   m_log_level_threshold(LogLevel::NIL)
    {
        m_prefix_stack.emplace_back("", LogLevel::NIL);
    }

    static size_t constexpr INDENT_SPACE_COUNT = 4;

    // Log level PrefixGuard functions.  Use these like
    //
    //     g_log << Log::crt() << "whatever super important message\n";
    //     g_log << Log::err() << "something bad happened:\n" << IndentGuard() << value << '\n';
    //
    // The temporary PrefixGuard will destruct once the statement has executed, and the prefix
    // will be popped, returning the prefix to its previous state.

    static PrefixGuard nil () { return PrefixGuard(Prefix(prefix_text(LogLevel::NIL), LogLevel::NIL)); }
    static PrefixGuard trc () { return PrefixGuard(Prefix(prefix_text(LogLevel::TRC), LogLevel::TRC)); }
    static PrefixGuard dbg () { return PrefixGuard(Prefix(prefix_text(LogLevel::DBG), LogLevel::DBG)); }
    static PrefixGuard inf () { return PrefixGuard(Prefix(prefix_text(LogLevel::INF), LogLevel::INF)); }
    static PrefixGuard wrn () { return PrefixGuard(Prefix(prefix_text(LogLevel::WRN), LogLevel::WRN)); }
    static PrefixGuard err () { return PrefixGuard(Prefix(prefix_text(LogLevel::ERR), LogLevel::ERR)); }
    static PrefixGuard crt () { return PrefixGuard(Prefix(prefix_text(LogLevel::CRT), LogLevel::CRT)); }

    // Accessors and modifiers

    // This is used in LOG_FORMATTED.
    Log &as_reference () { return *this; }
    std::ostream &out () const { return m_out; }
    size_t indent_level () const { return m_indent_level; }
    LogLevel log_level_threshold () const { return m_log_level_threshold; }
    Prefix const &prefix () const { assert(!m_prefix_stack.empty()); return m_prefix_stack.back(); }
    LogLevelHistogram const &log_level_histogram () const { return m_log_level_histogram; }
    LogLevelHistogram &log_level_histogram () { return m_log_level_histogram; }

    void indent (size_t count = 1) { m_indent_level += count; }
    void unindent (size_t count = 1) { assert(m_indent_level >= count); m_indent_level -= count; }
    void push_prefix (Prefix const &p) {
        assert(!m_prefix_stack.empty());
        m_prefix_stack.push_back(p);
        m_log_level_histogram.increment(p.m_log_level);
    }
    void pop_prefix () { assert(!m_prefix_stack.empty()); if (m_prefix_stack.size() > 1) m_prefix_stack.pop_back(); }
    void set_log_level_threshold (LogLevel log_level_threshold) { m_log_level_threshold = log_level_threshold; }

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
    // The use of std::remove_cv_t and std::remove_reference_t is so that references (e.g.
    // `std::string &`) and cv qualifiers (e.g. `int const`) are stripped so that proper
    // redirection to the appropriate overloads above happen.  Create a template specialization
    // of HasCustomLogOutputOverload in order to specify that another type has an overload of
    // operator<<(Log&, ...) that should be used directly.
    template <typename T_, typename = std::enable_if_t<!HasCustomLogOutputOverload<std::remove_cv_t<std::remove_reference_t<T_>>>::value>>
    Log &operator << (T_&& t)
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
    LogLevel m_log_level_threshold;
    std::vector<Prefix> m_prefix_stack;
    // This tracks the number of times each particular log level (via Prefix) has been pushed.
    LogLevelHistogram m_log_level_histogram;
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
