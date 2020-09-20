# lvd

Library of Victor Dods - Miscellaneously useful C++ code.

## Release Notes

Versioning system is [semantic versioning](https://semver.org/).

|Version|Notes|
|-------|-----|
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

## To-dos

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

-   Probably have `req` checks pass in a `std::function` that produces the explanation instead of a direct
    string, otherwise that string is getting made regardless of if the condition fails.  Generally, make the
    `req` checks as fast as possible.
-   Make a `g_req_context` which the app can control, i.e. to set the output `Log` and the `FailureBehavior`.
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
