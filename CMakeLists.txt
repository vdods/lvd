###############################################################################
# LVD (Library of Victor Dods) -- generally useful thingies
###############################################################################

cmake_minimum_required(VERSION 3.18)
project(lvd)
# NOTE: After cutting a release, you should increment the patch number and append `-DEV` so that
# the deb packages produced while developing are distinguished from the actual tagged release.
set(lvd_VERSION 0.6.0)

option(LVD_BUILD_TESTS "Enable compilation of lvd test binary" ON)
option(BUILD_SHARED_LIBS "Build shared libraries (instead of static libraries)" ON)

# Set and require the C++17 standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# # Options to correctly link the standard C++ lib on Mac.
# if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin") # This is the correct way to detect Mac OS X operating system -- see http://www.openguru.com/2009/04/cmake-detecting-platformoperating.html
#     set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")
#     if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang") # GCC ("GNU") probably would require a different option
#         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#     endif()
# endif()

###############################################################################
# Dependencies
###############################################################################

# Helper target(s)

add_library(Strict INTERFACE)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_compile_options(Strict INTERFACE -Wall -Werror)
endif()

add_library(SaveTemps INTERFACE)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin" OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_compile_options(SaveTemps INTERFACE -save-temps)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # TODO
endif()

###############################################################################
# Libraries
###############################################################################

set(lvd_SOURCES
    lvd/aliases.hpp
    lvd/ANSIColor.cpp
    lvd/ANSIColor.hpp
    lvd/FiLoc.cpp
    lvd/FiLoc.hpp
    lvd/FiPos.cpp
    lvd/FiPos.hpp
    lvd/FiRange.cpp
    lvd/FiRange.hpp
    lvd/fmt.hpp
    lvd/g_log.cpp
    lvd/g_log.hpp
    lvd/literal.cpp
    lvd/literal.hpp
    lvd/Log.hpp
    lvd/move_cast.hpp
    lvd/not_null.hpp
    lvd/NullOstream.cpp
    lvd/NullOstream.hpp
    lvd/OstreamDelegate.cpp
    lvd/OstreamDelegate.hpp
    lvd/Pipe.hpp
    lvd/Range_t.hpp
    lvd/req.hpp
    lvd/ScopeGuard.hpp
    lvd/static_if.hpp
    lvd/test.cpp
    lvd/test.hpp
    lvd/util.cpp
    lvd/util.hpp
)

add_library(lvd ${lvd_SOURCES})

# Copied from https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages
set_property(TARGET lvd PROPERTY VERSION ${lvd_VERSION})
set_property(TARGET lvd PROPERTY SOVERSION 0)
set_property(TARGET lvd PROPERTY INTERFACE_lvd_MAJOR_VERSION 0)
set_property(TARGET lvd APPEND PROPERTY COMPATIBLE_INTERFACE_STRING lvd_MAJOR_VERSION)

target_compile_definitions(lvd PUBLIC PACKAGE_VERSION="${lvd_VERSION}")
target_include_directories(lvd PUBLIC ${PROJECT_SOURCE_DIR})
target_link_libraries(lvd PUBLIC Strict)

###############################################################################
# Executables
###############################################################################

#
# lvdtest
#

set(lvdtest_SOURCES
    lvdtest/main.cpp
    lvdtest/test_abort.cpp
    lvdtest/test_ANSIColor.cpp
    lvdtest/test_Log.cpp
    lvdtest/test_move_cast.cpp
    lvdtest/test_not_null.cpp
    lvdtest/test_NullOstream.cpp
    lvdtest/test_OstreamDelegate.cpp
    lvdtest/test_Pipe.cpp
    lvdtest/test_util.cpp
)
add_executable(lvdtest ${lvdtest_SOURCES})
target_compile_definitions(lvdtest PUBLIC PACKAGE_VERSION="${lvd_VERSION}")
target_link_libraries(lvdtest PUBLIC Strict lvd)

###############################################################################
# Install rules
###############################################################################

install(
    TARGETS lvd                                 # Libraries produced by this package
    RUNTIME DESTINATION lib                     # This is relative to CMAKE_INSTALL_PREFIX
)
install(
    DIRECTORY lvd                               # This holds all the reflex and trison targets (targetspec and codespec files)
    DESTINATION include                         # This is relative to CMAKE_INSTALL_PREFIX
)
install(
    FILES                                       # Important files
#         LICENSE
        README.md
        lvd-config.cmake                        # lvd-config[-version].cmake is how find_package(lvd) works.
        "${CMAKE_CURRENT_BINARY_DIR}/lvd-config-version.cmake"
    DESTINATION lib/lvd                         # This is relative to CMAKE_INSTALL_PREFIX
)

###############################################################################
# Package config stuff
###############################################################################

# See https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/lvd-config-version.cmake"
    VERSION ${lvd_VERSION}
    COMPATIBILITY AnyNewerVersion
)

###############################################################################
# CPack rules for creating distributions
###############################################################################

# Reference: https://github.com/geoffmcl/cpack-test

if(WIN32)
    if(USE_WIX_TOOLSET)
        set(CPACK_GENERATOR "WIX") # this need WiX Tooset installed and a path to candle.exe
    else ()
        set(CPACK_GENERATOR "NSIS") # this needs NSIS installed, and available
    endif ()
    set(CPACK_SOURCE_GENERATOR "ZIP")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CPACK_GENERATOR "PackageMake")
else()
    set(CPACK_GENERATOR "DEB")
    set(CPACK_SOURCE_GENERATOR "TGZ")
endif()

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Victor Dods")

string(REPLACE "." ";" VERSION_LIST ${lvd_VERSION})
list(GET VERSION_LIST 0 lvd_VERSION_MAJOR)
list(GET VERSION_LIST 1 lvd_VERSION_MINOR)
list(GET VERSION_LIST 2 lvd_VERSION_PATCH)

set(CPACK_PACKAGE_VERSION "${lvd_VERSION_MAJOR}.${lvd_VERSION_MINOR}.${lvd_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION_MAJOR "${lvd_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${lvd_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${lvd_VERSION_PATCH}")

# set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.html")
# set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")
# set(CPACK_RESOURCE_FILE_WELCOME "${PROJECT_SOURCE_DIR}/README.html")

set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${lvd_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${lvd_VERSION}")

set(CPACK_SOURCE_IGNORE_FILES "${PROJECT_SOURCE_DIR}/build/;${PROJECT_SOURCE_DIR}/.git/;${PROJECT_SOURCE_DIR}/.gitignore;${PROJECT_SOURCE_DIR}/lvd.kdev4;${PROJECT_SOURCE_DIR}/.kdev4")

include(CPack)
