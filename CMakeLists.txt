###############################################################################
# LVD (Library of Victor Dods) -- generally useful thingies
###############################################################################

cmake_minimum_required(VERSION 3.18)
project(lvd)
# NOTE: After cutting a release, you should increment the patch number and append `-DEV` so that
# the deb packages produced while developing are distinguished from the actual tagged release.
set(lvd_VERSION 0.11.1-DEV)

option(BUILD_lvdtest "Build lvdtest binary" ON)
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

set(liblvd_HEADERS
    lib/lvd/abort.hpp
    lib/lvd/aliases.hpp
    lib/lvd/ANSIColor.hpp
    lib/lvd/call_site.hpp
    lib/lvd/comma.hpp
    lib/lvd/encoding.hpp
    lib/lvd/endian.hpp
    lib/lvd/FiLoc.hpp
    lib/lvd/FiPos.hpp
    lib/lvd/FiRange.hpp
    lib/lvd/fmt.hpp
    lib/lvd/g_log.hpp
    lib/lvd/g_req_context.hpp
    lib/lvd/IndexedTuple_t.hpp
    lib/lvd/literal.hpp
    lib/lvd/Log.hpp
    lib/lvd/move_cast.hpp
    lib/lvd/not_null.hpp
    lib/lvd/NullOstream.hpp
    lib/lvd/OstreamDelegate.hpp
    lib/lvd/PartiallyOrderedSet_t.hpp
    lib/lvd/PartialOrder.hpp
    lib/lvd/Pipe.hpp
    lib/lvd/random.hpp
    lib/lvd/Range_t.hpp
    lib/lvd/read.hpp
    lib/lvd/read_bin.hpp
    lib/lvd/remove_cv_recursive.hpp
    lib/lvd/req.hpp
    lib/lvd/ScopeGuard.hpp
    lib/lvd/serialization.hpp
    lib/lvd/static_if.hpp
    lib/lvd/test.hpp
    lib/lvd/TotalOrder.hpp
    lib/lvd/Type_t.hpp
    lib/lvd/TypeString_t.hpp
    lib/lvd/util.hpp
    lib/lvd/variant.hpp
    lib/lvd/write.hpp
    lib/lvd/write_bin.hpp
    lib/lvd/write_text.hpp
)

set(liblvd_SOURCES
    lib/lvd/ANSIColor.cpp
    lib/lvd/FiLoc.cpp
    lib/lvd/FiPos.cpp
    lib/lvd/FiRange.cpp
    lib/lvd/g_log.cpp
    lib/lvd/g_req_context.cpp
    lib/lvd/literal.cpp
    lib/lvd/NullOstream.cpp
    lib/lvd/OstreamDelegate.cpp
    lib/lvd/test.cpp
    lib/lvd/util.cpp
    lib/lvd/variant.hpp
)

add_library(liblvd ${liblvd_HEADERS} ${liblvd_SOURCES})
# This is to prevent cmake from prepending `lib` to `liblvd`.
set_target_properties(liblvd PROPERTIES PREFIX "")

# Copied from https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html#config-file-packages
set_property(TARGET liblvd PROPERTY VERSION ${lvd_VERSION})
set_property(TARGET liblvd PROPERTY SOVERSION 0)
set_property(TARGET liblvd PROPERTY INTERFACE_lvd_MAJOR_VERSION 0)
set_property(TARGET liblvd APPEND PROPERTY COMPATIBLE_INTERFACE_STRING lvd_MAJOR_VERSION)

target_compile_definitions(liblvd PUBLIC PACKAGE_VERSION="${lvd_VERSION}")
target_include_directories(liblvd PUBLIC ${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(liblvd PUBLIC Strict)

###############################################################################
# Executables
###############################################################################

#
# lvdtest
#

if(BUILD_lvdtest)
    set(lvdtest_SOURCES
        bin/lvdtest/DerivedString.hpp
        bin/lvdtest/DerivedString_random.hpp
        bin/lvdtest/DerivedString_serialization.hpp
        bin/lvdtest/main.cpp
        bin/lvdtest/test_abort.cpp
        bin/lvdtest/test_ANSIColor.cpp
        bin/lvdtest/test_endian.cpp
        bin/lvdtest/test_FiPos.cpp
        bin/lvdtest/test_literal.cpp
        bin/lvdtest/test_Log.cpp
        bin/lvdtest/test_move_cast.cpp
        bin/lvdtest/test_not_null.cpp
        bin/lvdtest/test_NullOstream.cpp
        bin/lvdtest/test_OstreamDelegate.cpp
        bin/lvdtest/test_PartiallyOrderedSet_t.cpp
        bin/lvdtest/test_PartialOrder.cpp
        bin/lvdtest/test_Pipe.cpp
        bin/lvdtest/test_random.cpp
        bin/lvdtest/test_Range_t.cpp
        bin/lvdtest/test_read_write_bin.cpp
        bin/lvdtest/test_req.cpp
        bin/lvdtest/test_serialization.cpp
        bin/lvdtest/test_TotalOrder.cpp
        bin/lvdtest/test_Type_t.cpp
        bin/lvdtest/test_TypeString_t.cpp
        bin/lvdtest/test_util.cpp
        bin/lvdtest/test_variant.cpp
        bin/lvdtest/test_write_text.cpp
    )
    add_executable(lvdtest ${lvdtest_SOURCES})
    target_compile_definitions(lvdtest PUBLIC PACKAGE_VERSION="${lvd_VERSION}")
    target_link_libraries(lvdtest PUBLIC Strict liblvd)
endif()

###############################################################################
# Install rules
###############################################################################

install(
    TARGETS liblvd                              # Libraries produced by this package
    RUNTIME DESTINATION lib                     # This is relative to CMAKE_INSTALL_PREFIX
)
install(
    FILES ${liblvd_HEADERS}
    DESTINATION include/lvd                     # This is relative to CMAKE_INSTALL_PREFIX
)
install(
    FILES                                       # Important files
        LICENSE.txt
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
