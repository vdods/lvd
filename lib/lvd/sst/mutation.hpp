// 2021.02.09 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/sst/SV_t.hpp"

namespace lvd {
namespace sst {

// Mutation_t is a scope guard class for use in "safely" modifying a semantic value.If changes violate
// the validity condition for the semantic value, then the program will abort .
//
// Construction of Mutation_t will keep a pointer to the SV_t object in question, and destruction of
// Mutation_t will cause the semantic value validity check to happen.  If the check fails, the program
// will abort (there's no possible way to throw within the pattern of Mutation_t, since the check happens
// during the destructor of Mutation_t, and C++ specifies that throwing within a destructor causes the
// program to abort).
//
// The recommended usage for Mutation_t is something like:
//
//      SV_t<SomeClass_s,ConcreteType> sv{something};
//      <code using sv>
//      // Now we will mutate sv in a nontrivial way
//      {
//          auto m = mutation(sv);              // Note that sv should be considered invalid for the rest of this block!
//          m.cv().fancy_procedure(1,2,3);      // Mutate non-const cv in-place.
//          change_stuff(m.cv());               // More mutations
//          // m is about to be destroyed, and will abort if the validity check fails.
//      }
//      // Assuming we made it this far, sv can be considered valid again now.
//
// If a recoverable mutation is desired, then the value should be copied from the SV_t object, modified,
// and then assign-moved into the SV_t object.  This allows the SV_t object to stay in a valid state
// and remain so even if an exception is thrown.
template <typename S_, typename C_>
class Mutation_t {
public:

    Mutation_t () = delete;
    Mutation_t (Mutation_t const &) = delete;
    Mutation_t (Mutation_t &&other) : m_sv(other.m_sv) { other.m_sv = nullptr; }
    explicit Mutation_t (SV_t<S_,C_> &sv) : m_sv(&sv) { }
    ~Mutation_t () { finish(); }

    Mutation_t &operator= (Mutation_t const &) = delete;
    Mutation_t &operator= (Mutation_t &&other) { m_sv = other.m_sv; other.m_sv = nullptr; }

    C_ const &cv () const { return m_sv->cv(); }
    C_ &cv () { return m_sv->cv_nonconst(); }

    // Explicitly finish the mutation and execute the appropriate value check immediately.
    void finish () {
        if (m_sv != nullptr) {
            m_sv->template check<ALLOW__VERIFY_OR_ABORT>();
            m_sv = nullptr;
        }
    }

private:

    SV_t<S_,C_> *m_sv;
};

// Convenience function for creating a Mutation_t object using type deduction.
template <typename S_, typename C_>
inline Mutation_t<S_,C_> mutation (SV_t<S_,C_> &sv) {
    return Mutation_t(sv);
}

} // end namespace sst
} // end namespace lvd
