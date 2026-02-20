# gersemi: off
# /CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# gersemi: on

cmake_minimum_required(VERSION 3.30...4.3)

include(./cmake/prelude.cmake)

#===================================================
project(beman.execution VERSION 0.2.0 LANGUAGES CXX)
#===================================================

# Modules opt in only on compilers that support it: msvc, g++-15 and clang-20+
include(./cmake/cxx-modules-rules.cmake)

set(TARGET_NAME beman.execution_headers) # used in src, and docs
set(TARGET_NAMESPACE beman) # TODO(CK): still used in docs, is this really needed?
set(TARGET_PREFIX ${PROJECT_NAME}) # NOTE: used in src, and docs?
set(TARGETS_EXPORT_NAME ${PROJECT_NAME}-targets) # NOTE: must be set, is important for installation! CK

#===============================================================================
if(BEMAN_USE_MODULES)
    option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

    set(CMAKE_CXX_SCAN_FOR_MODULES ON)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

    # CMake requires the language standard to be specified as compile feature
    # when a target provides C++23 modules and the target will be installed
    add_library(${TARGET_PREFIX} STATIC) # FIXME: only static yet! CK
    add_library(beman::execution ALIAS ${TARGET_PREFIX})
    target_compile_features(
        ${TARGET_PREFIX}
        PUBLIC cxx_std_${CMAKE_CXX_STANDARD}
    )

    # TODO(CK): the export header should be used!
    include(GenerateExportHeader)
    generate_export_header(
        ${TARGET_PREFIX}
        BASE_NAME ${TARGET_PREFIX}
        EXPORT_FILE_NAME beman/execution/modules_export.hpp
    )
    target_sources(
        ${TARGET_PREFIX}
        PUBLIC
            FILE_SET HEADERS
                BASE_DIRS include ${CMAKE_CURRENT_BINARY_DIR}
                FILES
                    ${CMAKE_CURRENT_BINARY_DIR}/beman/execution/modules_export.hpp
    )
    target_compile_definitions(${TARGET_PREFIX} PUBLIC BEMAN_HAS_MODULES)
    set_target_properties(
        ${TARGET_PREFIX}
        PROPERTIES
            VERSION ${PROJECT_VERSION}
            SOVERSION ${PROJECT_VERSION_MAJOR}
            DLL_NAME_WITH_SOVERSION TRUE
    )

    # FIXME: Quickfix only to prevent linker problems on windows dll? CK
    if(WIN32 AND BUILD_SHARED_LIBS)
        set_target_properties(
            ${TARGET_PREFIX}
            PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON
        )
    endif()

    find_package(Threads REQUIRED)
    target_link_libraries(${TARGET_PREFIX} PUBLIC Threads::Threads)
else()
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

if(BEMAN_USE_MODULES AND BEMAN_HAS_IMPORT_STD)
    target_compile_definitions(${TARGET_PREFIX} PUBLIC BEMAN_HAS_IMPORT_STD)
    set_target_properties(${TARGET_PREFIX} PROPERTIES CXX_MODULE_STD ON)
elseif(BEMAN_USE_MODULES)
    message(WARNING "Missing or disabled support for CMAKE_CXX_MODULE_STD!")
endif()
#===============================================================================

option(
    BEMAN_EXECUTION_ENABLE_TESTING
    "Enable building tests and test infrastructure. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

option(
    BEMAN_EXECUTION_BUILD_EXAMPLES
    "Enable building examples. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

add_subdirectory(src/beman/execution)

#===============================================================================
# NOTE: this must be done before tests! CK
include(./cmake/beman-install-library.cmake)
beman_install_library(${TARGET_PREFIX} TARGETS ${TARGET_NAME} ${TARGET_PREFIX}
    # TODO: DEPENDENCIES [===[beman.inplace_vector 1.0.0]===] [===[beman.scope 0.0.1 EXACT]===] fmt
    DEPENDENCIES Threads
)
#===============================================================================

enable_testing()

if(BEMAN_EXECUTION_ENABLE_TESTING)
    add_subdirectory(tests/beman/execution)
endif()

if(BEMAN_EXECUTION_BUILD_EXAMPLES)
    add_subdirectory(examples)
    add_subdirectory(docs/code)
endif()
