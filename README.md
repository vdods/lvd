# lvd

Library of Victor Dods - Miscellaneously useful C++ code.

## Release Notes

Versioning system is [semantic versioning](https://semver.org/).

|Version|Notes|
|-------|-----|
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

-   Separate util.hpp out into different files, and maybe leave out some of it (e.g. emit_* and remainder)
