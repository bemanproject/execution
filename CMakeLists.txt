# cmake-format: off
# CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

cmake_minimum_required(VERSION 3.25...3.31)

project(
    beman.execution
    DESCRIPTION "Beman implementation of asynchronous components"
    LANGUAGES CXX
    VERSION 0.0.1
)

option(
    BEMAN_EXECUTION_BUILD_TESTS
    "Enable building tests and test infrastructure. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)
option(
    BEMAN_EXECUTION_BUILD_EXAMPLES
    "Enable building examples. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)
option(
    BEMAN_EXECUTION_INSTALL_CONFIG_FILE_PACKAGE
    "Enable creating and installing a CMake config-file package. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

add_subdirectory(src/beman/execution)

if(BEMAN_EXECUTION_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests/beman/execution)
endif()

if(BEMAN_EXECUTION_BUILD_EXAMPLES)
  enable_testing()
  add_subdirectory(examples)
endif()
