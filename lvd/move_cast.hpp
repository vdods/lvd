// 2019.12.29 - Victor Dods

#pragma once

#include <gsl/gsl>
#include "lvd/fmt.hpp"
#include <memory>

namespace lvd {

//
// Define a version of static_cast and dynamic_cast for std::unique_ptr<T> and gsl::not_null<std::unique_ptr<T>>.
// It's not obvious how to define this for std::unique_ptr<T,Deleter> etc.
//

template <typename Target_, typename Source_, typename SourceDeleter_>
std::unique_ptr<Target_> static_move_cast (std::unique_ptr<Source_,SourceDeleter_> &p)
{
    return std::unique_ptr<Target_>(static_cast<Target_*>(p.release()));
}

// NOTE: the passed-in pointer p is garbage after this call, and violates the gsl::not_null
// contract, so it shouldn't be used at all after this call, or gsl::not_null will cause an error.
template <typename Target_, typename Source_, typename SourceDeleter_>
gsl::not_null<std::unique_ptr<Target_>> static_move_cast (gsl::not_null<std::unique_ptr<Source_,SourceDeleter_>> &p)
{
    return gsl::not_null<std::unique_ptr<Target_>>(static_cast<Target_*>(std::move(p).get().release()));
}

// In order to not violate the Principle Of Least Surprise, if the internal dynamic_cast fails
// (i.e. if the pointer was non-null but the dynamic_cast pointer was null, then the passed-in
// std::unique_ptr is not released (so that the resource it points to is not lost).  In this
// case, the returned std::unique_ptr is null.  This may not be the most desired behavior though.
// Up to experimentation and experience.
template <typename Target_, typename Source_, typename SourceDeleter_>
std::unique_ptr<Target_> dynamic_move_cast (std::unique_ptr<Source_,SourceDeleter_> &p)
{
    Target_ *dynamic_cast_p = dynamic_cast<Target_*>(p.get());
    bool dynamic_cast_succeeded = p == nullptr || dynamic_cast_p != nullptr;
    if (dynamic_cast_succeeded)
        p.release(); // std::unique_ptr::release is declared noexcept.
    return std::unique_ptr<Target_>(dynamic_cast_p);
}

// NOTE: the passed-in pointer p is garbage after this call, and violates the gsl::not_null
// contract, so it shouldn't be used at all after this call, or gsl::not_null will cause an error.
template <typename Target_, typename Source_, typename SourceDeleter_>
gsl::not_null<std::unique_ptr<Target_>> dynamic_move_cast (gsl::not_null<std::unique_ptr<Source_,SourceDeleter_>> &p)
{
    // The dynamic_cast is intentionally operating on references so that it will throw if the result is null.
    // std::unique_ptr::release is declared noexcept.
    return gsl::not_null<std::unique_ptr<Target_>>(&dynamic_cast<Target_&>(*std::move(p).get().release()));
}

} // end namespace lvd
