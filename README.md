# lvd

Library of Victor Dods - Miscellaneously useful C++ code.

## Copyright and License

Copyright Victor Dods

Licensed under the Apache License, Version 2.0 (the "License"); you may not
use this software except in compliance with the License.  You may obtain a
copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

## Release Notes

Versioning system is [semantic versioning](https://semver.org/).

|Version|Notes|
|-------|-----|
|`v0.11.0`|Added `PartialOrder` and `PartiallyOrderedSet_t` which provide rudimentary poset container creation and search.|
|`v0.10.2`|Fixed a bug in which test-running code didn't honor the failure behavior setting.|
|`v0.10.1`|Fixed a bug in the implementation of static_move_cast for `nnp<T>`.|
|`v0.10.0`|Added `sp` and `nnsp` pointer aliases (and related), and added more methods to `Range_t`.|
|`v0.9.0`|Improved `basic_test_main` function, improved `Range_t`, and added `nnp<T> = nn<T*>` alias.|
|`v0.8.0`|Released `lvd` under Apache 2.0 license.|
|`v0.7.0`|Added `lvd::test::basic_test_main` as an easy way to run tests, added `lvd::req::g_req_context` as a convenient default `lvd::req::Context` object with corresponding macros, added `LVD_CALL_SITE()` macro, added ability to pass a container directly into `Range_t` and `range`, improved directory organization and install rules.|
|`v0.6.0`|Added the ability to provide an explanation for a failed `LVD_REQ_*` or `LVD_TEST_REQ_*`, fixed a bug regarding invalid `FiRange` when calling `start_as_firange` or `end_as_firange`, added an option for building `lvd` as shared or static lib(s), improved test summary formatting.|
|`v0.5.0`|Added `FiLocAndFuncName` and `LVD_CALL_SITE` macro for better use in debug messages, fixed a bug having to do with `ANSIColorGuard` and `Log`.|
|`v0.4.0`|Moved some functions from `lvd/util.hpp` to `lvd/literal.hpp`, fixed a bug in `lvd/ANSIColor.hpp`, added some more tests, cleaned up some macros that didn't belong.|
|`v0.3.1`|Renamed lvd::req::Context::out and lvd::test::Context::out to lvd::req::Context::log and lvd::test::Context::log respectively.|
|`v0.3.0`|Added `flush` method to `Log`, changed `g_log` to use `std::cerr` instead of `std::cout`, refactored `req` and `test` to use `Log`, renamed `lvd/core.hpp` to `lvd/fmt.hpp`, fixed a bug having to do with implicit conversion.|
|`v0.2.0`|Added `NullOstream`, added `test_out` param to test bodies, renamed `lvd/Test.?pp` to `lvd/test.?pp`, moved test code into `test` namespace, added context objects to `test` and `req` code, made `req` configurable so it can throw or abort, did other misc code cleanup.|
|`v0.1.0`|Added tracking of log level message count to `Log`.  Improved how tests are declared/registered.  Improved `FiLoc` and integrated it into `lvd/Test.hpp`.  Improved `FiPos` and `FiRange`.|
|`v0.0.3`|Fixed `lvd/abort.hpp` and added some more tests.|
|`v0.0.2`|A few minor improvements, fixed some missing headers, added some more tests.|
|`v0.0.1`|A few bugfixes to `Log` and `not_null<std::unique_ptr<...>>`.|
|`v0.0.0`|Initial commit.|

## Building

### Configuration

This project uses `cmake`.  To create and configure the build system,

    cd <path/to/lvd>
    mkdir build
    cd build
    cmake ..

or use `ccmake ..` instead for an interactive configuration, which allows one to specify certain
compilation/installation parameters.

### Building a distribution-specific package (e.g. for Ubuntu/Debian)

To create a Debian package named `lvd-X.Y.Z.deb` (this may also create other distro-specific
packages on those distros), run the following command(s) from the `build` dir configured above.

    make package

This can then be installed via

    sudo dpkg -i lvd-X.Y.Z.deb

This can be uninstalled via

    sudo dpkg -r lvd

### Building and installing the old-fashioned way

To build, run the following command(s) from the `build` dir configured above.

    make

To install:

    make install

This will print out a log of which files were installed.  There doesn't appear to be an automatic way
to uninstall the package if it was installed via `make install`, but one can just go and delete the
files that were listed in the log.

## Notes

-   Idea: Could potentially implement indented and colored log (as in lvd::Log) by using syntax like

        std::cout << Indent() << "xyz\n";

    where

        std::ostream &operator << (std::ostream &out, Indent &indent) {
            return indent(out); // Wraps `out` and returns itself, which is a std::ostream& which performs indenting.
        }

    And if it were desired that the indentation persisted longer than the statement temporary,
    then this could be done:

        std::cout << "blah\n";
        auto out = Indent(std::cout);
        out << "stuff\n" << Indent() << "and things\n";
        out << "more stuff\n";

    which would give output

        blah
            stuff
                and things
            more stuff

    Similar with color guards.

-   Idea: Use a thread-local variable to store a thread's current `Log` object, or maybe `Log` stack.  This would be
    used for better logging call-stack and other hierarchical stuff in a way that's thread safe.  Should still have
    the `g_log` object though.

-   Idea: Define a total order on `Type_t<T>`, in one of the following ways:
    -   `Type_t<T>` < `Type_t<U>` iff their `typeid`s compare less-than (this is not portable, but is fast).
    -   `Type_t<T>` < `Type_t<U>` iff their type strings compare less-than (portable, but not fast).
    -   There's probably a portable, fast way that can be figured out.
-   Idea: Cause `Type_t<T>` to inherit `Type_t<P>` for each parent class `P` of `T`.  Then it would be possible to
    define the natural partial order on types based on inheritance subtyping, as well as define a kind of type-based
    polymorphism on global functions.
-   Idea: Try to make a `Template_t<template X<typename...>>` analog of `Type_t<T>` which allows use of templates
    (not instances of templates) as first-class values.  Or could this work by specializing `Type_t<template X<typename...>>`?

-   Idea: Instead of overloading `std::ostream &operator<< (std::ostream &out, T const &t)` for a zillion types `T`,
    making the overload resolution failure error messages endless streams of misery, use a wrapper type which simply
    contains a single `T const &` member variable, which also carries semantic meaning regarding how the value is to
    be rendered into the stream.  In particular:
    -   `Print<T>` means "print in a minimal, human-readable form", with options:
        -   Print with only their content, but not necessarily their type info.
        -   Print as a literal, like in a programming language, where type content is conveyed.
    -   `Binary<T>` means "serialize as binary into the stream", with options:
        -   Assume the type is known, so only render value into the stream.
        -   Include full type info in the stream, so that deserialization doesn't have to have any context.
    A challenge in implementing this is to be able to still use function overloading, instead of the `std::hash<T>`
    pattern where the type has to be specified exactly.  There should be a type `HuStream` which holds a
    `std::ostream &` and any other stream-configuration options, and for each type `T` that should have a
    human-readable print method, there should be an overload like:
    -   `HuStream &operator<< (HuStream &out, T const &value)`
    Similar for `SzStream`

    Syntax ideas:
    -   Jamming

            std::cout << "normal" << lvd::hu << v0 << v1;               // v0, v1 are human-printed
            std::cout << "normal" << lvd::hu(v0) << lvd::hu(v1) << v2;  // v0, v1 are human-printed, but not v2
            std::cout << "normal" << lvd::hu(v0, v1) << v2;             // v0, v1 are human-printed, but not v2
            std::cout << (lvd::hu << v0 << v1) << v2;                   // v0, v1 are human-printed, but not v2
            std::cout << lvd::hu << v0 << v1 << lvd::hu_end << v2;      // v0, v1 are human-printed, but not v2
            binary_stream << lvd::sz << v0 << v1 << v2;                 // v0, v1, v2 are serialized
            binary_stream << (lvd::sz << v0 << v1) << v2;               // v0, v1 are serialized, but not v2
            binary_stream << lvd::sz << v0 << v1 << lvd::sz_end << v2;  // v0, v1 are serialized, but not v2
            binary_stream << lvd::sz << v0 << v1 << lvd::sz_end << v2;  // v0, v1 are serialized, but not v2

        A few different syntaxes:
        -   `out << v0 << hu(v1) << hu(v2) << v3;` : Each value uses its own call to `lvd::hu`.
        -   `out << v0 << hu(v1, v2) << v3;` : Can supply an arbitrary number of values to `lvd::hu`.
        -   `out << v0 << hu << v1 << v2 << hu_end << v3;` : Explicit begin and end to the `lvd::hu`-printed values.
        -   `out << v0 << (hu << v1 << v2) << v3;` : Paren-specified scope for `lvd::hu`-printed values.

    Potentially there could be other helper types and functions that parameterize the particular rendering into the
    stream, such as:
    -   For human-readable printing:
        -   Number base
        -   Boolean format
        -   Localization conventions, e.g. date, time, decimal formats
    -   For binary serialization:
        -   Endianness
        -   Compression/decompression of stream

-   Idea: For semantic subtypes, use a template metafunction to return the concrete type for a semantic subtype.
-   "Static Association" pattern design notes
    -   Goal is to have a header-only implementation of a pattern which allows registration of functions and
        other data associated with types, e.g. to implement a kind of run-time type polymorphism.  Example use cases:
        -   Serialization/deserialization of the `std::any` type.
        -   Implementation of a particular function with type polymorphism in one or multiple arguments, such as
            `abstract_type_of` or `equals` or `partial_order`.
        -   Using more-advanced type polymorphism, such as multiple-parameter polymorphism, or the poset of subtypes.
    -   What's a good name for this?  The essential properties are that it allows static-init-time definition of
        associative containers (NOT sequential containers, because the order of initialization is ill-defined).
        -   Static association?  This is a C++-specific use of "static" though, so it doesn't perfectly represent
            the abstract pattern.
    -   Simplest implementation is just to have a a static, global associative container whose elements are generated
        by type-specific instantiations of some other thing at initialize time (static var initialization).
    -   Next implementation is function overloading, where parameter types must match exactly.
    -   Next implementation is function overloading for single-parameter functions, using subtype poset to determine
        matching overload.  Note that it's possible, when a type is not exactly equal to one of the nodes in the
        poset, for the poset to produce multiple matching supertypes for a given type, and therefore the overload
        would be ambiguous -- the resolution is to refine the poset so that matches are always unambiguous.
    -   Next implementation is function overloading for multiple-parameter functions, using subtype poset to determine
        matching overload.  Conjecture: this is equivalent to subtype poset on the parameter tuples, where the subtype
        relation on tuples is a simple "and" of the component subtype relations.

## To-dos

-   Use https://www.boost.org/doc/libs/1_36_0/doc/html/interprocess/streams.html for an `ostream` that
    writes to a vector.
-   Need to ensure that the serialization of `std::unordered_map` and `std::unordered_set` produce
    a deterministic output, probably by outputting the key/value pairs in key-sorted order.
-   Maybe use http://aantron.github.io/better-enums/tutorial/Maps.html for enums.
-   Change `PartialOrder` to actually be a bitmask of `LEQ` and `GEQ`, so that:
    -   `PartialOrder::LESS_THAN` is `LEQ` and `!GEQ`,
    -   `PartialOrder::GREATER_THAN` is `!LEQ` and `GEQ`,
    -   `PartialOrder::EQUAL` is `LEQ` and `GEQ`,
    -   `PartialOrder::INCOMPARABLE` is `!LEQ` and `!GEQ`.
-   Improve `PartiallyOrderedSet_t` with an optional way to linearly sort children and parents.  This will make the
    part of the search algorithms where it must sift through children and parents much faster, and perhaps it will
    bring the algorithms up to a reasonably optimal level.
-   Make the `make package` include the source files as well; everything needed to compile from source.
-   Logging should accept a lambda via `std::function` parameter which actually does the computation to do the
    printing, so that if the logging level causes a particular logging command to be suppressed, that computation
    isn't invoked.  Could potentially also return a no-op `std::ostream` which ignores all input, and ideally
    the optimizer will figure this out and not bother calling the stuff.
-   All the `req` functions should accept lambdas via `std::function` for their parameter description and message
    so that that code isn't run unless the requirement condition fails.  Otherwise that string is getting made regardless
    of if the condition fails.  Generally, make the `req` checks as fast as possible.
-   Probably should take out the `const` from `P_ const &` in `not_null.hpp`:

        // Conversion operator to the underlying pointed-at value.  Note that this is only a reasonable
        // thing to do because the type contract of not_null is the same of that of a reference.
        // NOTE: This may or may not be a thing you want to happen automatically, but for now it'll go in
        // as an experiment.
        constexpr operator P_ const & () const { return *get(); }

-   Probably provide a specialization for `gsl::not_null<T*>` in order to handle pointer-to-const semantics,
    as well as cast-to-baseclass semantics for the constructor.  Also construction from references.
-   Use cmake export targets so dependent projects can include only the sub-portion of `lvd` they use.
    In particular, make one of the export targets be the header-only portion of the lib, so there's
    no need to link anything.
-   BUG: In this one, the newline isn't printed:

        test_log << Log::dbg() << "got token from Scanner: " << ANSIColorGuard<Log>(ANSIColor::BRIGHT_WHITE, ANSIColor::DARK_BLUE) << Render(token.m_data);
        // NOTE: For some reason, printing the newline before ANSIColorGuard resets the color
        // causes the unprinted-upon section of the next line to appear in the guarded background color.
        test_log << '\n';
        test_log << Log::dbg() << IndentGuard() << token << '\n';

    but in this one, it is:

        test_log << Log::dbg() << "got token from Scanner: " << ANSIColorGuard<Log>(ANSIColor::BRIGHT_WHITE, ANSIColor::DARK_BLUE) << Render(token.m_data);
        // NOTE: For some reason, printing the newline before ANSIColorGuard resets the color
        // causes the unprinted-upon section of the next line to appear in the guarded background color.
        test_log << Log::dbg() << '\n';
        test_log << Log::dbg() << IndentGuard() << token << '\n';

    NOTE TO SELF: The reason is because the log level is not equal to DBG in the first example, and for some
    reason, that prevents the newline from being printed.

-   Is there a way to print the values of all parameters of the current function?  This would be a nice
    way to make something a little more verbose than LVD_CALL_SITE() for caveman debugging purposes.
-   Ideally, Log would implement std::ostream, and then all this business about HasCustomLogOutputOverload
    would go away.
-   Ensure the following about Log
    -   The prefix, if exists, is printed in the first line.
    -   Figure out well-defined rules about how the histogram works (should it only be about PushPrefix?
        or should it count newlines printed for a given log level?)
-   Verify that OstreamDelegate is actually zero-overhead by looking at `test_OstreamDelegate.s`.
-   Separate util.hpp out into different files, and maybe leave out some of it (e.g. emit_* and remainder)
-   Figure out how to generate a timestamp and dirhash to append to the `.deb` package version so that
    intermediate changes between versioned releases can be distinguished.
-   Make test run method indicate errors if any occur.
-   Make fancier version of Test where Log is used for output, verbosity, and error tracking.  Though
    probably each test's Log object should output to a separate std::ostringstream, perhaps recorded
    to a file.
-   Maybe add formal test dependency tracking for fancier version of Test, where there's no need to
    explicitly number stuff to enforce an order of execution that respects dependencies between tests.
-   Make an analog of req.hpp called check.hpp which causes non-fatal errors that just print stuff,
    but have an option of triggering an abort/exception if desired.
-   Make test framework able to run in "child process" mode, where it can isolate abort/crash errors,
    and run all tests, instead of a single crash blocking the rest of them.  This would also allow for
    tests that expect a crash to occur.
