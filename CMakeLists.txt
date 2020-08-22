###############################################################################
# LVD (Library of Victor Dods) -- generally useful thingies
###############################################################################

cmake_minimum_required(VERSION 3.10)
project(lvd)

option(LVD_COMPILE_TESTS "Enable compilation of lvd test cases" ON)

# The VERSION argument of cmake's project function is not semver compliant.
# See https://gitlab.kitware.com/cmake/cmake/issues/16716
set(PROJECT_VERSION "0.0.0")

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
    lvd/core.hpp
    lvd/aliases.hpp
    lvd/ANSIColor.cpp
    lvd/ANSIColor.hpp
    lvd/core.hpp
    lvd/FiLoc.cpp
    lvd/FiLoc.hpp
    lvd/FiPos.cpp
    lvd/FiPos.hpp
    lvd/FiRange.cpp
    lvd/FiRange.hpp
    lvd/g_log.cpp
    lvd/g_log.hpp
    lvd/Log.hpp
    lvd/move_cast.hpp
    lvd/not_null.hpp
    lvd/OstreamDelegate.cpp
    lvd/OstreamDelegate.hpp
    lvd/Pipe.hpp
    lvd/Range_t.hpp
    lvd/req.hpp
    lvd/ScopeGuard.hpp
    lvd/static_if.hpp
    lvd/Test.cpp
    lvd/Test.hpp
    lvd/util.cpp
    lvd/util.hpp
)

add_library(lvd ${lvd_SOURCES})
target_compile_definitions(lvd PUBLIC PACKAGE_VERSION="${PROJECT_VERSION}")
if(LVD_COMPILE_TESTS)
    target_compile_definitions(lvd PUBLIC LVD_COMPILE_TESTS=1)
endif()
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
    lvdtest/test_OstreamDelegate.cpp
    lvdtest/test_util.cpp
)
add_executable(lvdtest ${lvdtest_SOURCES})
target_compile_definitions(lvdtest PUBLIC PACKAGE_VERSION="${PROJECT_VERSION}")
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
        lvd-config.cmake                        # lvd-config.cmake is how find_package(lvd) works.
    DESTINATION lib/lvd                         # This is relative to CMAKE_INSTALL_PREFIX
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

string(REPLACE "." ";" VERSION_LIST ${PROJECT_VERSION})
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

set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")

set(CPACK_SOURCE_IGNORE_FILES "${PROJECT_SOURCE_DIR}/build/;${PROJECT_SOURCE_DIR}/.git/;${PROJECT_SOURCE_DIR}/.gitignore;${PROJECT_SOURCE_DIR}/lvd.kdev4;${PROJECT_SOURCE_DIR}/.kdev4")

include(CPack)
