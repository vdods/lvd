// 2019.12.19 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/fmt.hpp"
#include "lvd/not_null.hpp" // Includes <gsl/gsl>
#include <memory>

namespace lvd {

//
// Convenience aliases to make frequently used types more compact.
//

template <typename T_>
using nn = gsl::not_null<T_>;

template <typename T_>
using nnp = gsl::not_null<T_*>;

template <typename T_>
using sp = std::shared_ptr<T_>;

template <typename T_, typename Deleter_ = std::default_delete<T_>>
using up = std::unique_ptr<T_,Deleter_>;

template <typename T_>
using nnsp = gsl::not_null<std::shared_ptr<T_>>;

template <typename T_, typename Deleter_ = std::default_delete<T_>>
using nnup = gsl::not_null<std::unique_ptr<T_,Deleter_>>;

//
// Analogs to std::make_* functions
//

// A convenience function for creating a gsl::not_null<std::shared_ptr<T_>>.
// This is equivalent to lvd::make_not_null_shared_ptr.
template <typename T_, typename... Args_>
nnsp<T_> make_nnsp (Args_&&... args)
{
    return nnsp<T_>(std::make_shared<T_>(std::forward<Args_>(args)...));
}

// A convenience function for creating a gsl::not_null<std::unique_ptr<T_>>.
// Note that there's currently no way to specify a custom deleter.  This is
// equivalent to lvd::make_not_null_unique_ptr.
template <typename T_, typename... Args_>
nnup<T_> make_nnup (Args_&&... args)
{
    return nnup<T_>(std::make_unique<T_>(std::forward<Args_>(args)...));
}

} // end namespace lvd
