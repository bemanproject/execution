# gersemi: off
# /CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# gersemi: on

cmake_minimum_required(VERSION 3.30...4.2)

include(cmake/prelude.cmake)

#===================================================
project(beman_execution VERSION 0.1.0 LANGUAGES CXX)
#===================================================

# Modules opt in only on compilers that support it: msvc, g++-15 and clang-20+
include(cxx-modules-rules)

set(TARGET_NAME beman_execution_headers) # used in src, and docs
set(TARGET_NAMESPACE beman) # NOTE: only still used in docs?
set(TARGET_PREFIX ${PROJECT_NAME}) # NOTE: only still used in docs?
set(TARGET_PACKAGE_NAME ${PROJECT_NAME}-config)
set(TARGETS_EXPORT_NAME ${PROJECT_NAME}-config-targets)

#===============================================================================
if(BEMAN_USE_MODULES)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)

    # CMake requires the language standard to be specified as compile feature
    # when a target provides C++23 modules and the target will be installed
    add_library(beman_execution STATIC)
    add_library(beman::beman_execution ALIAS beman_execution)
    target_compile_features(
        beman_execution
        PUBLIC cxx_std_${CMAKE_CXX_STANDARD}
    )

    include(GenerateExportHeader)

    generate_export_header(
        beman_execution
        BASE_NAME beman_execution
        EXPORT_FILE_NAME beman/execution/modules_export.hpp
    )
    target_sources(
        beman_execution
        PUBLIC
            FILE_SET HEADERS
                BASE_DIRS include ${CMAKE_CURRENT_BINARY_DIR}
                FILES
                    ${CMAKE_CURRENT_BINARY_DIR}/beman/execution/modules_export.hpp
    )
    target_compile_definitions(beman_execution PUBLIC BEMAN_HAS_MODULES)
endif()

if(BEMAN_USE_MODULES AND CMAKE_CXX_MODULE_STD)
    target_compile_definitions(beman_execution PUBLIC BEMAN_HAS_IMPORT_STD)
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

option(
    BEMAN_EXECUTION_ENABLE_INSTALL
    "Install the project components. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

add_subdirectory(src/beman/execution)

if(BEMAN_EXECUTION_ENABLE_TESTING)
    enable_testing()

    add_subdirectory(tests/beman/execution)
endif()

if(BEMAN_EXECUTION_BUILD_EXAMPLES)
    add_subdirectory(examples)
    add_subdirectory(docs/code)
endif()

if(NOT BEMAN_EXECUTION_ENABLE_INSTALL OR CMAKE_SKIP_INSTALL_RULES)
    return()
endif()

include(infra/cmake/beman-install-library-config.cmake)
# FIXME: install TARGETS target beman.execution is exported but not all of its interface file sets are installed
# TODO: beman_install_library(beman_execution beman_execution_headers)

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
