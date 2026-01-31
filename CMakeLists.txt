# gersemi: off
# /CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# gersemi: on

cmake_minimum_required(VERSION 3.30...4.2)

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
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

    # CMake requires the language standard to be specified as compile feature
    # when a target provides C++23 modules and the target will be installed
    add_library(beman.execution STATIC)
    add_library(beman::execution ALIAS beman.execution)
    target_compile_features(
        beman.execution
        PUBLIC cxx_std_${CMAKE_CXX_STANDARD}
    )

    include(GenerateExportHeader)

    generate_export_header(
        beman.execution
        BASE_NAME beman.execution
        EXPORT_FILE_NAME beman/execution/modules_export.hpp
    )
    target_sources(
        beman.execution
        PUBLIC
            FILE_SET HEADERS
                BASE_DIRS include ${CMAKE_CURRENT_BINARY_DIR}
                FILES
                    ${CMAKE_CURRENT_BINARY_DIR}/beman/execution/modules_export.hpp
    )
    # FIXME: target_compile_definitions(beman.execution PUBLIC BEMAN_HAS_MODULES)
endif()

if(BEMAN_USE_MODULES AND CMAKE_CXX_MODULE_STD)
    target_compile_definitions(beman.execution PUBLIC BEMAN_HAS_IMPORT_STD)
else()
    message(WARNING "Missing support for CMAKE_CXX_MODULE_STD!")
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

# NOTE: this must be done before tests! CK
include(./cmake/beman-install-library-config.cmake)
beman_install_library(${TARGET_PREFIX} headers)

if(BEMAN_EXECUTION_ENABLE_TESTING)
    enable_testing()

    add_subdirectory(tests/beman/execution)
endif()

if(BEMAN_EXECUTION_BUILD_EXAMPLES)
    add_subdirectory(examples)
    add_subdirectory(docs/code)
endif()
