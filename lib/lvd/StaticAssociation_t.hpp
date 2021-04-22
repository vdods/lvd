// 2021.02.13 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include "lvd/FiLoc.hpp"
#include <stdexcept>

namespace lvd {

// A static association is a singleton, static instance of an associative container, the elements of
// which are "registered" via instantiations of StaticAssociationRegistrar_t.  A particular instance
// of a static association is identified by a StaticAssociationDefinition type, which also defines
// what kind of container it is.
//
// A StaticAssociationDefinition should look like `struct Xyz { using Container = Pqr; };` where `Pqr` is an
// associative container type, e.g. std::map<uint32_t,std::string>.  Different StaticAssociationDefinition
// types correspond to different associative container instances, so in particular you can't re-use the same
// StaticAssociationDefinition type for different instances of static associations.
//
// The LVD_STATIC_ASSOCIATION_DEFINE macro is provided for convenience, though it doesn't work if
// the container type is a template type having a comma in it, since the C preprocessor misparses that.
template <typename StaticAssociationDefinition_>
inline typename StaticAssociationDefinition_::Container &static_association_singleton () {
    // Because this function is inline and this local var is static, this local var exists exactly
    // once across all translation units (i.e. it's a true singleton).  This is specified by the C++ standard.
    static typename StaticAssociationDefinition_::Container container;
    return container;
}

// Create inline static instances of this StaticAssociationRegistrar_t<Xyz> to register an element of the
// static association Xyz.  The LVD_STATIC_ASSOCIATION_REGISTER macro is provided for convenience.
template <typename StaticAssociationDefinition_>
struct StaticAssociationRegistrar_t {
    template <typename... Args_>
    StaticAssociationRegistrar_t (FiLoc &&filoc, Args_&&... args) {
        auto [it, inserted] = static_association_singleton<StaticAssociationDefinition_>().emplace(std::forward<Args_>(args)...);
        std::ignore = it;
        if (!inserted)
            throw std::domain_error(filoc.as_string() + ": collision in static association");
    }
};

// Use this macro to create a StaticAssociationDefinition.  Unfortunately, the macro doesn't
// work if there's a template type with a comma in it, because the C preprocessor parses it as
// < and > operators instead of as template expressions.
#define LVD_STATIC_ASSOCIATION_DEFINE(Name, Container_) struct Name { using Container = Container_; };
// Use this macro to add an element to the given StaticAssociation.  This will pass the arguments
// after unique_id to StaticAssociationDefinition::Container::emplace.
#define LVD_STATIC_ASSOCIATION_REGISTER(StaticAssociationDefinition, unique_id, ...) namespace { inline static lvd::StaticAssociationRegistrar_t<StaticAssociationDefinition> __##StaticAssociationDefinition##__##unique_id{lvd::FiLoc(__FILE__, __LINE__), __VA_ARGS__}; }

} // end namespace lvd
