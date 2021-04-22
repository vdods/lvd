// 2021.01.10 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/type.hpp"
#include <variant>

namespace lvd {

// TODO: Change the macros to use the visitor pattern with `Visitor_t`.
// References:
// - https://en.cppreference.com/w/cpp/utility/variant/visit
// - https://stackoverflow.com/questions/57726401/stdvariant-vs-inheritance-vs-other-ways-performance

// Helper for std::visit
template <typename... Types_> struct Visitor_t : Types_... {
    Visitor_t (Types_...) = delete;
    using Types_::operator()...;
};

// This is a way to essentially do a switch statement on a variadic sequence of types using
// an index that's only known at runtime.  Should pass in a lambda of the form
//
//     [...](auto &&t){
//         static_assert(is_Type_t_v<std::decay_t<decltype(t)>>);
//         using T_ = typename std::decay_t<decltype(t)>::T;
//         <do something that is specific to T_>
//     }
//
// The passed-in t will range through Types_... via Type_t.
template <size_t TRY_INDEX_, typename... Types_, typename Function_>
void call_on_indexed_type (size_t index, Function_ const &function) {
    if (index == TRY_INDEX_) {
        function(ty<std::tuple_element_t<TRY_INDEX_,std::tuple<Types_...>>>);
    } else if constexpr (TRY_INDEX_+1 < sizeof...(Types_)) {
        call_on_indexed_type<TRY_INDEX_+1,Types_...>(index, function);
    } else {
        throw std::runtime_error("invalid index for variadic type sequence");
    }
}

// Used to represent a value of a given type without needing to actually instantiate it.
template <typename T_>
struct Hypothetical_t {
    Hypothetical_t () = default;
    Hypothetical_t (Hypothetical_t const &) = default;
    Hypothetical_t (Hypothetical_t &&) = default;
    Hypothetical_t (T_ const &) { }
    Hypothetical_t (T_ &&) { }
};

// Used to determine the index of the type T_ within the given Hypothetical_t<std::variant<...>>.
template <typename T_, typename First_, typename... Rest_>
size_t constexpr variant_type_index (Hypothetical_t<std::variant<First_,Rest_...>>) {
    if constexpr (std::is_same_v<T_,First_>) {
        return 0;
    } else if constexpr (sizeof...(Rest_) == 0) {
        return std::variant_npos;
    } else if constexpr (variant_type_index<T_>(Hypothetical_t<std::variant<Rest_...>>()) != std::variant_npos) {
        return 1 + variant_type_index<T_>(Hypothetical_t<std::variant<Rest_...>>());
    } else {
        return std::variant_npos;
    }
}

// Used to determine the index of the type T_ within the given Hypothetical_t<std::variant<...>>.
template <typename T_, typename... Types_>
size_t constexpr variant_type_index (std::variant<Types_...> const &) {
    return variant_type_index<T_>(Hypothetical_t<std::variant<Types_...>>());
}

// Convenience macro for dealing with std::variant in a reasonable way.
//
// The first argument, `v`, is the std::variant value whose type will be switched upon.
//
// The second argument, `v_strongly_typed`, is the name of the variable that will be declared
// as a const reference to std::get<T>(v) within the case code block for each typed handler.
//
// The third argument MUST be default (as in the keyword itself).
//
// The fourth argument is code that will be placed within a {}-scoped block for the default case.
//
// After that, every pair of arguments (and they must be in pairs) define a type T and handler code for
// that type case.  There will be a `auto const &v_strongly_typed = std::get<T>(v);` declaration within
// the case's {}-scoped block, then the handler code, then a break statement.
//
// Note that there is NO fallthrough on each case, since the typed variable v_strongly_typed is well-defined
// only for each single type case.  This is enforced by an implicit break statement at the end of each
// handler code block.
//
// Note that if there is a compile error something like "T should occur for exactly once in alternatives"
// then this indicates a superfluous type case (i.e. one not present in the std::variant's types).
//
// Note that a compile error "duplicate case value" means exactly that, regarding type cases.
//
// See https://bitbashing.io/std-visit.html for more context on why this whole mess of macros exists.
#define LVD_HANDLE_VARIANT_WITH_DEFAULT(v, v_strongly_typed, default_keyword, default_handler_code, ...) \
switch ((v).index()) { \
    LVD_HANDLE_VARIANT_CASES(v, v_strongly_typed, __VA_ARGS__) \
    default_keyword: { default_handler_code; break; } \
}

// Convenience macro for dealing with std::variant in a reasonable way, same as
// LVD_HANDLE_VARIANT_WITH_DEFAULT except no default case is to be specified, and therefore the default_keyword
// and default_handler_code arguments are omitted.
#define LVD_HANDLE_VARIANT(v, v_strongly_typed, ...) \
switch ((v).index()) { \
    LVD_HANDLE_VARIANT_CASES(v, v_strongly_typed, __VA_ARGS__) \
}

// Helper macro for LVD_HANDLE_VARIANT.
#define LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) case lvd::variant_type_index<T>(v): { auto const &v_strongly_typed = std::get<T>(v); std::ignore = v_strongly_typed; T_handler_code; break; }

// Recursive helper macros for LVD_HANDLE_VARIANT.  Reference: https://gist.github.com/irondoge/9488bba0e8b170afbee96fbea84a976a
#define LVD_HANDLE_VARIANT_CASES_0(...)
// This one is needed to handle the case in LVD_HANDLE_VARIANT where only the default handler is given.
#define LVD_HANDLE_VARIANT_CASES_1(...)
#define LVD_HANDLE_VARIANT_CASES_2(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_0(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_3(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_1(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_4(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_2(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_5(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_3(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_6(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_4(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_7(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_5(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_8(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_6(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_9(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_7(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_10(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_8(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_11(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_9(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_12(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_10(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_13(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_11(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_14(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_12(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_15(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_13(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_16(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_14(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_17(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_15(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_18(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_16(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_19(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_17(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_20(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_18(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_21(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_19(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_22(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_20(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_23(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_21(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_24(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_22(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_25(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_23(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_26(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_24(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_27(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_25(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_28(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_26(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_29(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_27(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_30(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_28(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_31(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_29(v, v_strongly_typed, __VA_ARGS__)
#define LVD_HANDLE_VARIANT_CASES_32(v, v_strongly_typed, T, T_handler_code, ...) LVD_HANDLE_VARIANT_CASE(v, v_strongly_typed, T, T_handler_code) LVD_HANDLE_VARIANT_CASES_30(v, v_strongly_typed, __VA_ARGS__)

// _0 is intentionally included.  Here, the convention is to use 0-indexed elements.
#define LVD_GET_32TH_VARIADIC_ARGUMENT(_0, _1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, _32, ...) _32
// Here, the convention is to use 0-indexed elements.
#define LVD_COUNT_VARIADIC_ARGUMENTS(...) LVD_GET_32TH_VARIADIC_ARGUMENT(__VA_ARGS__, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0)

#define LVD_HANDLE_VARIANT_CASES_DEREF_AGAIN(N) LVD_HANDLE_VARIANT_CASES_ ## N
#define LVD_HANDLE_VARIANT_CASES_DEREF(N) LVD_HANDLE_VARIANT_CASES_DEREF_AGAIN(N)
#define LVD_HANDLE_VARIANT_CASES(v, v_strongly_typed, ...) LVD_HANDLE_VARIANT_CASES_DEREF(LVD_COUNT_VARIADIC_ARGUMENTS(__VA_ARGS__))(v, v_strongly_typed, ##__VA_ARGS__)

} // end namespace lvd
