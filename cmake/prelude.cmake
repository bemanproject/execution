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

if(PROJECT_NAME)
    message(
        FATAL_ERROR
        "This CMake file has to be included before first project() command call!"
    )
endif()

# gersemi: off
# ---------------------------------------------------------------------------
# use ccache if found
# ---------------------------------------------------------------------------
find_program(CCACHE_EXECUTABLE "ccache" HINTS /usr/local/bin /opt/local/bin)
if(CCACHE_EXECUTABLE)
    message(STATUS "use ccache")
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}" CACHE PATH "ccache")
    set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_EXECUTABLE}" CACHE PATH "ccache")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# ---------------------------------------------------------------------------
# check if import std; is supported by CMAKE_CXX_COMPILER
# ---------------------------------------------------------------------------
if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.2)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
endif()
# gersemi: on

if("$ENV{CXX}" STREQUAL "" AND CMAKE_CXX_COMPILER)
    message(WARNING "\$CXX is not set")
    set(ENV{CXX} ${CMAKE_CXX_COMPILER})
endif()

# ---------------------------------------------------------------------------
# Workaround needed for CMAKE and clang++ to find the libc++.modules.json file
# ---------------------------------------------------------------------------
if(
    CMAKE_VERSION VERSION_GREATER 4.2
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

        set(CMAKE_CXX_STDLIB_MODULES_JSON
            ${LLVM_DIR}/lib/c++/libc++.modules.json
        )
    elseif(LINUX)
        execute_process(
            OUTPUT_VARIABLE LLVM_MODULES
            COMMAND clang++ -print-file-name=c++/libc++.modules.json
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
            WARNING
            "File does NOT EXISTS! ${CMAKE_CXX_STDLIB_MODULES_JSON}"
        )
    endif()
endif()
