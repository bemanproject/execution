# gersemi: off
# /CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# gersemi: on

cmake_minimum_required(VERSION 3.30...4.2)

include(cmake/prelude.cmake)
#===================================================
project(beman.execution VERSION 0.0.1 LANGUAGES CXX)
#===================================================

# Modules opt in only on compilers that support it: msvc, g++-15 and clang-20+
include(cxx-modules-rules)

set(TARGET_NAME execution)
set(TARGET_NAMESPACE beman)
set(TARGET_PREFIX ${TARGET_NAMESPACE}.${TARGET_NAME})
set(TARGET_LIBRARY ${TARGET_NAME})
set(TARGET_ALIAS ${TARGET_NAMESPACE}::${TARGET_NAME})
set(TARGET_PACKAGE_NAME ${PROJECT_NAME}-config)
set(TARGETS_EXPORT_NAME ${PROJECT_NAME}-targets)

add_library(${TARGET_LIBRARY} STATIC)
add_library(${TARGET_ALIAS} ALIAS ${TARGET_LIBRARY})

# CMake requires the language standard to be specified as compile feature
# when a target provides C++23 modules and the target will be installed
target_compile_features(${TARGET_LIBRARY} PUBLIC cxx_std_23)

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

add_subdirectory(src/beman/execution)

enable_testing()

if(BEMAN_EXECUTION_ENABLE_TESTING)
    add_subdirectory(tests/beman/execution)
endif()

if(BEMAN_EXECUTION_BUILD_EXAMPLES)
    add_subdirectory(examples)
    add_subdirectory(docs/code)
endif()

if(NOT BEMAN_EXECUTION_ENABLE_INSTALL OR CMAKE_SKIP_INSTALL_RULES)
    return()
endif()

include(beman-install-library-config)
# TODO(CK): beman_install_library(${TARGET_LIBRARY})

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_PACKAGE_NAME}-version.cmake
    VERSION ${CMAKE_PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    "cmake/Config.cmake.in"
    ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_PACKAGE_NAME}.cmake
    INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_PACKAGE_NAME}.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${TARGET_PACKAGE_NAME}-version.cmake
    DESTINATION ${INSTALL_CONFIGDIR}
)

set(CPACK_GENERATOR TGZ)
include(CPack)
