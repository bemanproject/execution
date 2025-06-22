# cmake-format: off
# /CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

cmake_minimum_required(VERSION 3.25...3.31)

project(beman_execution VERSION 0.0.1 LANGUAGES CXX)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds are not allowed!")
endif()

set(TARGET_NAME execution)
set(TARGET_NAMESPACE beman)
set(TARGET_PREFIX ${TARGET_NAMESPACE}.${TARGET_NAME})
set(TARGET_LIBRARY ${PROJECT_NAME})
set(TARGET_ALIAS ${TARGET_NAMESPACE}::${TARGET_NAME})
set(TARGET_PACKAGE_NAME ${PROJECT_NAME}-config)
set(TARGETS_EXPORT_NAME ${PROJECT_NAME}-targets)

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

option(
  BEMAN_EXECUTION_ENABLE_INSTALL
  "Install the project components. Values: { ON, OFF }."
  ${PROJECT_IS_TOP_LEVEL}
)

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

if(FALSE)
if(PROJECT_IS_TOP_LEVEL AND NOT BEMAN_EXECUTION_ENABLE_INSTALL OR CMAKE_SKIP_INSTALL_RULES)
  set(CMAKE_SKIP_INSTALL_RULES ON)

  include(FetchContent)

  # Add project_options from https://github.com/aminya/project_options
  # Change the version in the following URL to update the package
  # (watch the releases of the repository for future updates)
  set(PROJECT_OPTIONS_VERSION "v0.41.0")
  FetchContent_Declare(
    _project_options URL https://github.com/aminya/project_options/archive/refs/tags/${PROJECT_OPTIONS_VERSION}.zip
  )
  FetchContent_MakeAvailable(_project_options)
  include(${_project_options_SOURCE_DIR}/Index.cmake)

  # Initialize project_options variable related to this project
  # This overwrites `project_options` and sets `project_warnings`
  # uncomment to enable the options. Some of them accept one or more inputs:
  project_options(
    PREFIX
    ${TARGET_NAME}
    ENABLE_CACHE
    # NO! # ENABLE_CLANG_TIDY
    # NO! ENABLE_VS_ANALYSIS
    # ENABLE_INTERPROCEDURAL_OPTIMIZATION
    # ENABLE_NATIVE_OPTIMIZATION
    # ENABLE_DOXYGEN
    # ENABLE_COVERAGE
    ENABLE_SANITIZER_ADDRESS
    ENABLE_SANITIZER_UNDEFINED
    # TODO: ENABLE_SANITIZER_THREAD
    # FIXME: on Linux only with clang++? ENABLE_SANITIZER_MEMORY
    ENABLE_SANITIZER_POINTER_COMPARE
    ENABLE_SANITIZER_POINTER_SUBTRACT
    ENABLE_CONTROL_FLOW_PROTECTION
    ENABLE_STACK_PROTECTION
    ENABLE_OVERFLOW_PROTECTION
    # ENABLE_ELF_PROTECTION
    # ENABLE_RUNTIME_SYMBOLS_RESOLUTION
    # ENABLE_COMPILE_COMMANDS_SYMLINK
    # ENABLE_PCH
    # PCH_HEADERS
    # WARNINGS_AS_ERRORS
    # ENABLE_INCLUDE_WHAT_YOU_USE
    # ENABLE_GCC_ANALYZER
    # ENABLE_BUILD_WITH_TIME_TRACE
    # TODO: buggy! ENABLE_UNITY
    # LINKER "lld"
  )
endif()
endif()

add_subdirectory(src/beman/execution)

if(BEMAN_EXECUTION_ENABLE_TESTING)
  enable_testing()

  add_subdirectory(tests/beman/execution)
endif()

if(BEMAN_EXECUTION_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(NOT BEMAN_EXECUTION_ENABLE_INSTALL OR CMAKE_SKIP_INSTALL_RULES)
  return()
endif()

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_PACKAGE_NAME}-version.cmake
  VERSION ${CMAKE_PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
  "cmake/Config.cmake.in" ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_PACKAGE_NAME}.cmake
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_PACKAGE_NAME}.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_PACKAGE_NAME}-version.cmake
  DESTINATION ${INSTALL_CONFIGDIR}
)

set(CPACK_GENERATOR TGZ)
include(CPack)
