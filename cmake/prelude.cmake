# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# This file must be included/used as CMAKE_PROJECT_TOP_LEVEL_INCLUDES -> before project() is called!
#

# ---- The include guard applies globally to the whole build ----
include_guard(GLOBAL)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(
        FATAL_ERROR
        "In-source builds are not supported. "
        "Please read the BUILDING document before trying to build this project. "
        "You may need to delete 'CMakeCache.txt' and 'CMakeFiles/' first."
    )
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# ---------------------------------------------------------------------------
# The CMAKE_EXPERIMENTAL_CXX_IMPORT_STD is not longer needed except for OSX
# ---------------------------------------------------------------------------
if(NOT BEMAN_USE_STD_MODULE OR CMAKE_VERSION VERSION_GREATER_EQUAL 4.3)
    if(NOT APPLE)
        return()
    endif()
endif()
# ---------------------------------------------------------------------------

# gersemi: off
# ---------------------------------------------------------------------------
# check if import std; is supported by CMAKE_CXX_COMPILER
# ---------------------------------------------------------------------------
if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.2 AND CMAKE_VERSION VERSION_LESS 4.3)
    if(PROJECT_NAME)
        message(
            WARNING
            "This CMake file has to be included before first project() command call!"
        )
    endif()

    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
endif()
# gersemi: on

# TODO(CK): Do we need this HACK still for linux too?
if(NOT APPLE)
    return()
endif()

# FIXME: clang++ we still needs to export CXX=clang++
if("$ENV{CXX}" STREQUAL "" AND CMAKE_CXX_COMPILER)
    message(WARNING "\$CXX is not set")
    set(ENV{CXX} ${CMAKE_CXX_COMPILER})
endif()

# ---------------------------------------------------------------------------
# Workaround needed for CMAKE and clang++ to find the libc++.modules.json file
# ---------------------------------------------------------------------------
if(
    CMAKE_VERSION VERSION_GREATER_EQUAL 4.2
    AND ("$ENV{CXX}" MATCHES "clang" OR CMAKE_CXX_COMPILER MATCHES "clang")
)
    # NOTE: Always use libc++
    # see https://releases.llvm.org/19.1.0/projects/libcxx/docs/index.html
    set(ENV{CXXFLAGS} -stdlib=libc++)
    message(STATUS "CXXFLAGS=-stdlib=libc++")

    if(APPLE)
        execute_process(
            OUTPUT_VARIABLE LLVM_PREFIX
            COMMAND brew --prefix llvm
            COMMAND_ECHO STDOUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        file(REAL_PATH ${LLVM_PREFIX} LLVM_DIR)
        set(LLVM_DIR ${LLVM_DIR} CACHE FILEPATH "")

        message(STATUS "LLVM_DIR=${LLVM_DIR}")
        add_link_options(-L${LLVM_DIR}/lib/c++)
        include_directories(SYSTEM ${LLVM_DIR}/include)

        # /usr/local/Cellar/llvm/21.1.8_1/lib/c++/libc++.modules.json
        # "/usr/local/Cellar/llvm/21.1.8_1/share/libc++/v1/std.cppm",
        set(CMAKE_CXX_STDLIB_MODULES_JSON
            ${LLVM_DIR}/lib/c++/libc++.modules.json
        )
    elseif(LINUX)
        execute_process(
            OUTPUT_VARIABLE LLVM_MODULES
            COMMAND clang++ -print-file-name=libc++.modules.json
            COMMAND_ECHO STDOUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(NOT CMAKE_CXX_STDLIB_MODULES_JSON)
            set(CMAKE_CXX_STDLIB_MODULES_JSON ${LLVM_MODULES})
        endif()
        message(
            STATUS
            "CMAKE_CXX_STDLIB_MODULES_JSON=${CMAKE_CXX_STDLIB_MODULES_JSON}"
        )
    endif()

    if(EXISTS ${CMAKE_CXX_STDLIB_MODULES_JSON})
        message(
            STATUS
            "CMAKE_CXX_STDLIB_MODULES_JSON=${CMAKE_CXX_STDLIB_MODULES_JSON}"
        )
        # gersemi: off
        set(CACHE{CMAKE_CXX_STDLIB_MODULES_JSON}
            TYPE FILEPATH
            HELP "Result of: clang++ -print-file-name=c++/libc++.modules.json"
            VALUE ${CMAKE_CXX_STDLIB_MODULES_JSON}
        )
        # gersemi: on
    else()
        message(
            FATAL_ERROR
            "File does NOT EXISTS! ${CMAKE_CXX_STDLIB_MODULES_JSON}"
        )
    endif()
endif()
