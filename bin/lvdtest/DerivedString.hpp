// 2021.01.21 - Copyright Victor Dods - Licensed under Apache 2.0

#pragma once

#include <string>

namespace lvd {

//
// DC = Default Constructor
// NDC = No Default Constructor
// IP = Implicit Promotion [from baseclass]
// EP = Explicit Promotion [from baseclass]
//

class DerivedString_DC_EP : public std::string {
public:

    using std::string::string;
    // Note that copy/move constructors are not implicitly inherited, so they must be defined.
    // And defining them will hide the baseclass's copy/move constructors, so versions of those
    // have to be provided too.
    DerivedString_DC_EP (DerivedString_DC_EP const &other) : std::string(other) { }
    DerivedString_DC_EP (DerivedString_DC_EP &&other) : std::string(std::move(other)) { }
    // Explicit promotion from baseclass.
    explicit DerivedString_DC_EP (std::string const &s) : std::string(s) { }
    explicit DerivedString_DC_EP (std::string &&s) : std::string(std::move(s)) { }
};

class DerivedString_DC_IP : public std::string {
public:

    using std::string::string;
    // Note that copy/move constructors are not implicitly inherited, so they must be defined.
    // And defining them will hide the baseclass's copy/move constructors, so versions of those
    // have to be provided too.
    DerivedString_DC_IP (DerivedString_DC_IP const &other) : std::string(other) { }
    DerivedString_DC_IP (DerivedString_DC_IP &&other) : std::string(std::move(other)) { }
    // Implicit promotion from baseclass.
    DerivedString_DC_IP (std::string const &s) : std::string(s) { }
    DerivedString_DC_IP (std::string &&s) : std::string(std::move(s)) { }
};

class DerivedString_NDC_EP : public std::string {
public:

    using std::string::string;
    // Delete default constructor
    DerivedString_NDC_EP () = delete;
    // Note that copy/move constructors are not implicitly inherited, so they must be defined.
    // And defining them will hide the baseclass's copy/move constructors, so versions of those
    // have to be provided too.
    DerivedString_NDC_EP (DerivedString_NDC_EP const &other) : std::string(other) { }
    DerivedString_NDC_EP (DerivedString_NDC_EP &&other) : std::string(std::move(other)) { }
    // Explicit promotion from baseclass.
    explicit DerivedString_NDC_EP (std::string const &s) : std::string(s) { }
    explicit DerivedString_NDC_EP (std::string &&s) : std::string(std::move(s)) { }
};

class DerivedString_NDC_IP : public std::string {
public:

    using std::string::string;
    // Delete default constructor
    DerivedString_NDC_IP () = delete;
    // Note that copy/move constructors are not implicitly inherited, so they must be defined.
    // And defining them will hide the baseclass's copy/move constructors, so versions of those
    // have to be provided too.
    DerivedString_NDC_IP (DerivedString_NDC_IP const &other) : std::string(other) { }
    DerivedString_NDC_IP (DerivedString_NDC_IP &&other) : std::string(std::move(other)) { }
    // Implicit promotion from baseclass.
    DerivedString_NDC_IP (std::string const &s) : std::string(s) { }
    DerivedString_NDC_IP (std::string &&s) : std::string(std::move(s)) { }
};

} // end namespace lvd
