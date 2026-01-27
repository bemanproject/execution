# gersemi: off
# /CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# gersemi: on

cmake_minimum_required(VERSION 3.25...4.2)

#========================== pre project settings ===============================
# gersemi: off
if(CMAKE_VERSION VERSION_EQUAL 4.2)
    set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")

    if(CMAKE_CXX_STDLIB_MODULES_JSON)
        message(
            STATUS
            "CMAKE_CXX_STDLIB_MODULES_JSON=${CMAKE_CXX_STDLIB_MODULES_JSON}"
        )
    endif()
endif()
# gersemi: on
#===============================================================================

#===================================================
project(beman_execution VERSION 0.0.1 LANGUAGES CXX)
#===================================================

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds are not allowed!")
endif()

set(TARGET_NAME execution)
set(TARGET_NAMESPACE beman)
set(TARGET_PREFIX ${TARGET_NAMESPACE}.${TARGET_NAME})
set(TARGET_LIBRARY ${PROJECT_NAME})
set(TARGET_ALIAS ${TARGET_NAMESPACE}::${TARGET_NAME})
set(TARGET_PACKAGE_NAME ${PROJECT_NAME}-config)
set(TARGETS_EXPORT_NAME ${PROJECT_NAME}-config-targets)

#========================== post project settings ==============================
# Tell CMake that we explicitly want `import std`.
# This will initialize the property on all targets declared after this to 1
message(STATUS "CMAKE_CXX_COMPILER_IMPORT_STD=${CMAKE_CXX_COMPILER_IMPORT_STD}")
if(${CMAKE_CXX_STANDARD} IN_LIST CMAKE_CXX_COMPILER_IMPORT_STD)
    set(CMAKE_CXX_MODULE_STD ON)
    message(STATUS "CMAKE_CXX_MODULE_STD=${CMAKE_CXX_MODULE_STD}")
endif()

if(CMAKE_CXX_SCAN_FOR_MODULES AND ${CMAKE_GENERATOR} STREQUAL Ninja)
    set(BEMAN_USE_MODULES ON)
    message(STATUS "BEMAN_USE_MODULES=${BEMAN_USE_MODULES}")
else()
    message(WARNING "Missing support for CMAKE_CXX_SCAN_FOR_MODULES!")
endif()

# gersemi: off
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
    message(
        STATUS
        "CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES=${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}"
    )
endif()
# gersemi: on

# CMake requires the language standard to be specified as compile feature
# when a target provides C++23 modules and the target will be installed
add_library(${TARGET_NAME} STATIC)
target_compile_features(${TARGET_NAME} PUBLIC cxx_std_23)

if(BEMAN_USE_MODULES AND CMAKE_CXX_MODULE_STD)
    target_compile_definitions(${TARGET_NAME} PUBLIC BEMAN_HAS_IMPORT_STD)
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

set(CMAKE_CXX_SCAN_FOR_MODULES ON)
add_library(beman_execution)
target_sources(beman_execution PUBLIC FILE_SET modules_public TYPE CXX_MODULES
    FILES modules/execution.cppm)

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
