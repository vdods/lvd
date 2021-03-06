// 2019.12.29 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <gsl/gsl>
#include "lvd/fmt.hpp"
#include "lvd/move_cast.hpp"
#include <memory>
#include <type_traits>

//
// TEMP HACK -- this is a hopefully temporary specialization of gsl::not_null<std::unique_ptr<...>>
// which ideally will have a generalization without the need for any template specialization.
// This may involve creating a different implementation of not_null (the existing gsl::not_null
// seems to be somewhat lacking in certain casts and other semantics, and doesn't by default
// allow usage of std::unique_ptr).
//

namespace gsl {

// Specialization for std::shared_ptr for its specific semantics.
template <class P_>
class not_null<std::shared_ptr<P_>> {
public:
    typedef std::shared_ptr<P_> S_;

    // There's really no need for this, since the non_null contract is trivially satisfied in this case.
//     static_assert(std::is_assignable<S_&, std::nullptr_t>::value, "std::shared_ptr<P_,Deleter_> cannot be assigned nullptr.");

    // Default construction is not valid, since that would be a null shared_ptr..
    constexpr not_null () = delete;
    // Copy constructor is fine.
    constexpr not_null (not_null const &other) = default;
    // Move constructor.
    constexpr not_null (not_null &&other) = default;

    // Construct from shared_ptr.
    constexpr not_null(S_ const &t)
        :   ptr_(t)
    {
        Expects(ptr_ != nullptr); // TODO: Ensure that this throws if ptr_ is null.
    }
    // Move constructor -- used to move from a shared_ptr (which could conceivably be null)
    constexpr not_null(S_ &&t)
        :   ptr_(std::move(t))
    {
        Expects(ptr_ != nullptr); // TODO: Ensure that this throws if ptr_ is null.
    }

    // Accept std::shared_ptr<...> from a convertible type.
    // TODO: Figure out if this can be improved and generalized.
    template <typename Q_, typename = std::enable_if_t<std::is_convertible<Q_*,P_*>::value>>
    constexpr not_null(std::shared_ptr<Q_> const &other)
        :   ptr_(std::static_pointer_cast<Q_>(other)) // TODO: Shouldn't this cast to P_ instead of Q_?
    {
        Expects(ptr_ != nullptr);
    }
    // Accept std::shared_ptr<...> from a convertible type.
    // TODO: Figure out if this can be improved and generalized.
    template <typename Q_, typename = std::enable_if_t<std::is_convertible<Q_*,P_*>::value>>
    constexpr not_null(std::shared_ptr<Q_> &&other)
        :   ptr_(lvd::static_move_cast<Q_>(other)) // TODO: Shouldn't this cast to P_ instead of Q_?
    {
        Expects(ptr_ != nullptr);
    }

    // Accept gsl::not_null<std::shared_ptr<...>> from a convertible type.
    // TODO: Figure out if this can be improved and generalized.
    template <typename Q_, typename = std::enable_if_t<std::is_convertible<Q_*,P_*>::value>>
    constexpr not_null(not_null<std::shared_ptr<Q_>> const &other)
        :   ptr_(std::static_pointer_cast<Q_>(other.get())) // TODO: Shouldn't this cast to P_ instead of Q_?
    {
        // This check could theoretically be taken out, since the passed-in not_null should already be good.
        Expects(ptr_ != nullptr);
    }
    // Accept gsl::not_null<std::shared_ptr<...>> from a convertible type.
    // TODO: Figure out if this can be improved and generalized.
    template <typename Q_, typename = std::enable_if_t<std::is_convertible<Q_*,P_*>::value>>
    constexpr not_null(not_null<std::shared_ptr<Q_>> &&other)
        :   ptr_(lvd::static_move_cast<Q_>(other.get())) // TODO: Shouldn't this cast to P_ instead of Q_?
    {
        // This check could theoretically be taken out, since the passed-in not_null should already be good.
        Expects(ptr_ != nullptr);
    }

    // Accept from a convertible not_null holding raw pointer type.
    template <typename Q_, typename = std::enable_if_t<std::is_convertible<Q_*,P_*>::value>>
    constexpr not_null(not_null<Q_*> const &q)
        :   ptr_(q.ptr_)
    {
        // This check could theoretically be taken out, since the passed-in not_null should already be good.
        Expects(ptr_ != nullptr);
    }

    // Accept from a convertible raw pointer type.
    template <typename Q_, typename = std::enable_if_t<std::is_convertible<Q_*,P_*>::value>>
    constexpr not_null(Q_ *q)
        :   ptr_(q)
    {
        Expects(ptr_ != nullptr);
    }

//     template <typename U, typename = std::enable_if_t<std::is_convertible<U, S_>::value>>
//     constexpr not_null(U&& u) : ptr_(std::forward<U>(u))
//     {
//         Expects(ptr_ != nullptr);
//     }

//     template <typename U, typename = std::enable_if_t<std::is_convertible<U, S_>::value>>
//     constexpr not_null(const not_null<U>& other) : not_null(other.get())
//     {
//     }

    // Copy-assignment operator.
    constexpr not_null &operator= (not_null const &other) = default;
    // Move-assignment operator.
    constexpr not_null &operator= (not_null &&other) = default;

    // Return a const reference to the internal pointer.
    constexpr S_ const &get () const & {
        Ensures(ptr_ != nullptr);
        return ptr_;
    }
    // Return an rvalue reference to the internal pointer.  Note that this will invalidate the
    // not_null type contract, so this non_null object should not be used afterward.
    constexpr S_ move_get () && {
        Ensures(ptr_ != nullptr);
        return std::move(ptr_);
    }

    // Conversion operator to the underlying pointed-at value.  Note that this is only a reasonable
    // thing to do because the type contract of not_null is the same of that of a reference.
    // NOTE: This may or may not be a thing you want to happen automatically, but for now it'll go in
    // as an experiment.
    constexpr operator P_ const & () const { return *get(); }
    // Conversion operator to the underlying std::shared_ptr.
    constexpr operator S_ const & () const { return get(); }
//     // This is used to implicitly convert gsl::not_null<std::shared_ptr<S_>>&& to std::shared_ptr<S_>&&.
//     // NOTE: After this call, this pointer is null (i.e. its type contract is violated), and should not be used.
//     constexpr operator S_ && () && {
//         Ensures(ptr_ != nullptr);
//         return std::move(ptr_);
//     }
    // This is used to be able to convert to an up-casted std::shared_ptr<Q_> where P_* is convertible to Q_*.
    // NOTE: After this call, this pointer is null (i.e. its type contract is violated), and should not be used.
    template <typename Q_, typename = std::enable_if_t<std::is_convertible<P_*,Q_*>::value>>
    constexpr operator std::shared_ptr<Q_> () && {
        Ensures(ptr_ != nullptr);
        return std::shared_ptr<Q_>(std::move(ptr_));
    }
    // This is used to be able to convert to an up-casted Q_* where P_* is convertible to Q_*.
    template <typename Q_, typename = std::enable_if_t<std::is_convertible<P_*,Q_*>::value>>
    constexpr operator Q_ * () const {
        Ensures(ptr_ != nullptr);
        return ptr_.get();
    }
    // This is used to be able to convert to an up-casted gsl::not_null<Q_*> where P_* is convertible to Q_*.
    template <typename Q_, typename = std::enable_if_t<std::is_convertible<P_*,Q_*>::value>>
    constexpr operator not_null<Q_*> () const {
        Ensures(ptr_ != nullptr);
        return not_null<Q_*>(ptr_.get());
    }

    constexpr S_ const &operator-> () const { return get(); }
    constexpr decltype(auto) operator* () const { return *get(); }

    // Prevent compilation when someone attempts to assign a null pointer constant.
    not_null (std::nullptr_t) = delete;
    not_null &operator= (std::nullptr_t) = delete;

    // Unwanted operators ... pointers only point to single objects!
    not_null &operator++ () = delete;
    not_null &operator-- () = delete;
    not_null operator++ (int) = delete;
    not_null operator-- (int) = delete;
    not_null &operator+= (std::ptrdiff_t) = delete;
    not_null &operator-= (std::ptrdiff_t) = delete;
    void operator[] (std::ptrdiff_t) const = delete;

private:

    S_ ptr_;
};

// Design notes
// -    gsl::not_null doesn't provide a specialization for gsl::not_null<std::unique_ptr<...>>
//      which is quite an oversight.  This provides one and to make it behave
//      as expected.
// -    Tests should include situations where an up-cast should happen implicitly (e.g.
//      gsl::not_null<Derived*> should implicitly cast to gsl::not_null<Base*>, and
//      gsl::not_null<std::unique_ptr<Derived>>&& should implicitly cast to
//      gsl::not_null<std::unique_ptr<Base>>&&
//
// Note that not_null<std::unique_ptr<P_>> is somewhat conceptually ill-founded, because if
// you ever move out of one of these things into a different one (or a std::unique_ptr<P_>),
// this one is left in a null state, which is antithesis to the whole point of the class.
// However, if you just accept the convention that such an object shouldn't be used after
// it's moved from, then there's no problem.  The ideal solution would actually delete the
// symbol from the symbol table after the move, so it would be impossible to access it.
template <class P_, typename Deleter_>
class not_null<std::unique_ptr<P_,Deleter_>> {
public:
    typedef std::unique_ptr<P_,Deleter_> U_;

    // There's really no need for this, since the non_null contract is trivially satisfied in this case.
//     static_assert(std::is_assignable<U_&, std::nullptr_t>::value, "std::unique_ptr<P_,Deleter_> cannot be assigned nullptr.");

    // Default construction is not valid, since that would be a null unique_ptr.
    constexpr not_null () = delete;
    // Copy constructor is disallowed.
    constexpr not_null (not_null const &other) = delete;
    // Move constructor.
    constexpr not_null (not_null &&other) = default;

    // Copying a unique_ptr is disallowed.
    constexpr not_null (U_ const &) = delete;
    // Used to move from a unique_ptr (which could conceivably be null)
    constexpr not_null(U_ &&t)
        :   ptr_(std::move(t))
    {
        Expects(ptr_ != nullptr); // TODO: Ensure that this throws if ptr_ is null.
    }

    // Accept std::unique_ptr<...> from a convertible type.
    // TODO: Figure out if this can be improved and generalized.
    // TODO: Figure out how to handle a different deleter type
    template <
        typename Q_,
        typename = std::enable_if_t<
            std::is_convertible<Q_*,P_*>::value //&&
//             std::is_same<Deleter_,std::default_delete<Q_>>::value
        >
    >
    constexpr not_null(std::unique_ptr<Q_> &&other)
        :   ptr_(lvd::static_move_cast<Q_>(other)) // TODO: Shouldn't this cast to P_ instead of Q_?
    {
        Expects(ptr_ != nullptr);
    }

    // Accept gsl::not_null<std::unique_ptr<...>> from a convertible type.
    // TODO: Figure out if this can be improved and generalized.
    // TODO: Figure out how to handle a different deleter type
    template <
        typename Q_,
        typename = std::enable_if_t<
            std::is_convertible<Q_*,P_*>::value //&&
//             std::is_same<Deleter_,std::default_delete<Q_>>::value
        >
    >
    constexpr not_null(not_null<std::unique_ptr<Q_>> &&other)
        :   ptr_(lvd::static_move_cast<Q_>(other)) // TODO: Shouldn't this cast to P_ instead of Q_?
    {
        // This check could theoretically be taken out, since the passed-in not_null should already be good.
        Expects(ptr_ != nullptr);
    }

    // Accept from a convertible not_null holding raw pointer type.
    template <
        typename Q_,
        typename = std::enable_if_t<
            std::is_convertible<Q_*,P_*>::value //&&
//             std::is_same<Deleter_,std::default_delete<Q_>>::value
        >
    >
    constexpr not_null(not_null<Q_*> const &q)
        :   ptr_(q.ptr_)
    {
        // This check could theoretically be taken out, since the passed-in not_null should already be good.
        Expects(ptr_ != nullptr);
    }

    // Accept from a convertible raw pointer type.
    template <
        typename Q_,
        typename = std::enable_if_t<
            std::is_convertible<Q_*,P_*>::value //&&
//             std::is_same<Deleter_,std::default_delete<Q_>>::value
        >
    >
    constexpr not_null(Q_ *q)
        :   ptr_(q)
    {
        Expects(ptr_ != nullptr);
    }

//     template <typename U, typename = std::enable_if_t<std::is_convertible<U, U_>::value>>
//     constexpr not_null(U&& u) : ptr_(std::forward<U>(u))
//     {
//         Expects(ptr_ != nullptr);
//     }

//     template <typename U, typename = std::enable_if_t<std::is_convertible<U, U_>::value>>
//     constexpr not_null(const not_null<U>& other) : not_null(other.get())
//     {
//     }

    // Copy-assignment operator is disallowed
    not_null &operator= (not_null const &other) = delete;
    // Move-assignment operator.
    not_null &operator= (not_null &&other) = default;

    // Return a const reference to the internal pointer.
    constexpr U_ const &get () const & {
        Ensures(ptr_ != nullptr);
        return ptr_;
    }
    // Return an rvalue reference to the internal pointer.  Note that this will invalidate the
    // not_null type contract, so this non_null object should not be used afterward.
    constexpr U_ move_get () && {
        Ensures(ptr_ != nullptr);
        return std::move(ptr_);
    }

    // Conversion operator to the underlying pointed-at value.  Note that this is only a reasonable
    // thing to do because the type contract of not_null is the same of that of a reference.
    // NOTE: This may or may not be a thing you want to happen automatically, but for now it'll go in
    // as an experiment.
    constexpr operator P_ const & () const { return *get(); }
    // Conversion operator to the underlying std::unique_ptr.
    constexpr operator U_ const & () const { return get(); }
//     // This is used to implicitly convert gsl::not_null<std::unique_ptr<U_>>&& to std::unique_ptr<U_>&&.
//     // NOTE: After this call, this pointer is null (i.e. its type contract is violated), and should not be used.
//     constexpr operator U_ && () && {
//         Ensures(ptr_ != nullptr);
//         return std::move(ptr_);
//     }
    // This is used to be able to convert to an up-casted std::unique_ptr<Q_> where P_* is convertible to Q_*.
    // NOTE: After this call, this pointer is null (i.e. its type contract is violated), and should not be used.
    template <
        typename Q_,
        typename = std::enable_if_t<
            std::is_convertible<P_*,Q_*>::value //&&
//             std::is_same<Deleter_,std::default_delete<Q_>>::value
        >
    >
    constexpr operator std::unique_ptr<Q_> () && {
        Ensures(ptr_ != nullptr);
        return std::unique_ptr<Q_>(std::move(ptr_));
    }
    // This is used to be able to convert to an up-casted Q_* where P_* is convertible to Q_*.
    template <
        typename Q_,
        typename = std::enable_if_t<
            std::is_convertible<P_*,Q_*>::value //&&
//             std::is_same<Deleter_,std::default_delete<Q_>>::value
        >
    >
    constexpr operator Q_ * () const {
        Ensures(ptr_ != nullptr);
        return ptr_.get();
    }
    // This is used to be able to convert to an up-casted gsl::not_null<Q_*> where P_* is convertible to Q_*.
    template <
        typename Q_,
        typename = std::enable_if_t<
            std::is_convertible<P_*,Q_*>::value //&&
//             std::is_same<Deleter_,std::default_delete<Q_>>::value
        >
    >
    constexpr operator not_null<Q_*> () const {
        Ensures(ptr_ != nullptr);
        return not_null<Q_*>(ptr_.get());
    }

    constexpr U_ const &operator-> () const { return get(); }
    constexpr decltype(auto) operator* () const { return *get(); }

    // Prevent compilation when someone attempts to assign a null pointer constant.
    not_null (std::nullptr_t) = delete;
    not_null &operator= (std::nullptr_t) = delete;

    // Unwanted operators ... pointers only point to single objects!
    not_null &operator++ () = delete;
    not_null &operator-- () = delete;
    not_null operator++ (int) = delete;
    not_null operator-- (int) = delete;
    not_null &operator+= (std::ptrdiff_t) = delete;
    not_null &operator-= (std::ptrdiff_t) = delete;
    void operator[] (std::ptrdiff_t) const = delete;

private:

    U_ ptr_;
};

} // end namespace gsl

//
// Overloads of comparator operators of gsl::not_null with nullptr_t, which can be determined at compile time.
//

template <class T_> constexpr bool operator== (gsl::not_null<T_> const &lhs, nullptr_t) { return false; }
template <class T_> constexpr bool operator== (nullptr_t, gsl::not_null<T_> const &lhs) { return false; }
template <class T_> constexpr bool operator!= (gsl::not_null<T_> const &lhs, nullptr_t) { return true; }
template <class T_> constexpr bool operator!= (nullptr_t, gsl::not_null<T_> const &lhs) { return true; }

//
// MAYBE TEMP HACK (not sure if std::pointer_traits provides specialization for std::unique_ptr)
//

namespace std {

template <typename T_>
class pointer_traits<std::shared_ptr<T_>> : public pointer_traits<T_*> { };

template <typename T_, typename Deleter_>
class pointer_traits<std::unique_ptr<T_,Deleter_>> : public pointer_traits<T_*> { };

template <typename T_>
class pointer_traits<gsl::not_null<T_>> : public pointer_traits<T_> { };

} // end namespace std

namespace lvd {

// A convenience function for creating a gsl::not_null<std::shared_ptr<T_>>.
template <typename T_, typename... Args_>
gsl::not_null<std::shared_ptr<T_>> make_not_null_shared (Args_&&... args) {
    return gsl::not_null<std::shared_ptr<T_>>(std::make_shared<T_>(std::forward<Args_>(args)...));
}

// A convenience function for creating a gsl::not_null<std::unique_ptr<T_>>.
// Note that there's currently no way to specify a custom deleter.
template <typename T_, typename... Args_>
gsl::not_null<std::unique_ptr<T_>> make_not_null_unique (Args_&&... args) {
    return gsl::not_null<std::unique_ptr<T_>>(std::make_unique<T_>(std::forward<Args_>(args)...));
}

} // end namespace lvd
