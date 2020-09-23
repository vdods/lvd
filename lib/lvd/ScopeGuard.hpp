// 2020.05.25 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <functional>
#include "lvd/fmt.hpp"

namespace lvd {

// Uses RAII to guarantee the release of a resource upon destruction, thereby providing exception safety.
struct ScopeGuard
{
    // Default construction has no-op release.
    ScopeGuard () { }
    // Disallow copying, since the ScopeGuard owns the single responsibility for release.
    ScopeGuard (ScopeGuard const &) = delete;
    // Allow moving.
    ScopeGuard (ScopeGuard &&) = default;
    // Construct non-trivial release.
    template <typename... Args_>
    ScopeGuard (Args_&&... args)
    :   m_release(std::forward<Args_>(args)...)
    { }

    ~ScopeGuard () { release(); }

    void release ()
    {
        if (m_release != nullptr)
        {
            m_release();
            m_release = nullptr;
        }
    }

private:

    std::function<void()> m_release;
};

} // end namespace lvd
