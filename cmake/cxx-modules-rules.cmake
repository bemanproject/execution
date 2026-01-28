#
# A CMake language file to be included as the last step of all project() command calls.
# This file must be included/used as CMAKE_PROJECT_INCLUDE -> after project()
#

# ---- The include guard applies within the current directory and below ----
include_guard(DIRECTORY)

if(NOT PROJECT_NAME)
    message(
        FATAL_ERROR
        "This CMake file has to be included as the last step of all project() command calls!"
    )
endif()

# Use modules? default NO!
if(NOT DEFINED CMAKE_CXX_SCAN_FOR_MODULES)
    set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
endif()

# Control whether the test target depends on the all target.
set(CMAKE_SKIP_TEST_ALL_DEPENDENCY OFF)

# gersemi: off
option(CMAKE_EXPORT_COMPILE_COMMANDS "Prepare run-clang-tidy" ${PROJECT_IS_TOP_LEVEL})
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    message(
        STATUS
        "CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES=${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}"
    )
    set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()
# gersemi: on

# Ensure non-empty default build type for single-config
get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(NOT isMultiConfig)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
endif()
set(CMAKE_DEBUG_POSTFIX _d)

# ------------------------------------------------------------------------------
# This property setting also needs to be consistent between the installed shared
# library and its consumer, otherwise most toolchains will once again reject the
# consumer's generated BMI.
# ------------------------------------------------------------------------------
if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
endif()

# Neither of these two are technically needed, but they make the expectation clear
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# NOTE: only with Ninja generator install of bmi files works yet!
if(CMAKE_GENERATOR MATCHES "Ninja")
    if(
        CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
        AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.0
    )
        set(CMAKE_CXX_SCAN_FOR_MODULES ON)

        if(NOT LINUX)
            string(APPEND CMAKE_CXX_MODULE_MAP_FLAG " -fmodules-reduced-bmi")
        endif()

        add_compile_options($ENV{CXXFLAGS})
        add_link_options($ENV{CXXFLAGS})
    elseif(
        CMAKE_CXX_COMPILER_ID STREQUAL "GNU"
        AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 15.0
    )
        set(CMAKE_CXX_SCAN_FOR_MODULES ON)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(CMAKE_CXX_SCAN_FOR_MODULES ON)
    else()
        set(CMAKE_CXX_SCAN_FOR_MODULES OFF)
    endif()
endif()

if(CMAKE_CXX_STDLIB_MODULES_JSON)
    message(
        STATUS
        "CMAKE_CXX_STDLIB_MODULES_JSON=${CMAKE_CXX_STDLIB_MODULES_JSON}"
    )
endif()

if(NOT DEFINED CMAKE_CXX_MODULE_STD)
    set(CMAKE_CXX_MODULE_STD OFF)
endif()

option(
    BEMAN_USE_STD_MODULE
    "Check if 'import std;' is possible with the toolchain"
    OFF
)
if(BEMAN_USE_STD_MODULE)
    # -------------------------------------------------------------------------
    # Tell CMake that we explicitly want `import std`.
    # This will initialize the property on all targets declared after this to 1
    # -------------------------------------------------------------------------
    message(
        STATUS
        "CMAKE_CXX_COMPILER_IMPORT_STD=${CMAKE_CXX_COMPILER_IMPORT_STD}"
    )
    if(${CMAKE_CXX_STANDARD} IN_LIST CMAKE_CXX_COMPILER_IMPORT_STD)
        set(CMAKE_CXX_MODULE_STD ON)
        option(
            BEMAN_HAS_IMPORT_STD
            "Build with import std; possible"
            ${CMAKE_CXX_MODULE_STD}
        )
        message(STATUS "CMAKE_CXX_MODULE_STD=${CMAKE_CXX_MODULE_STD}")
    else()
        set(CMAKE_CXX_MODULE_STD OFF)
        message(WARNING "CMAKE_CXX_MODULE_STD=${CMAKE_CXX_MODULE_STD}")
    endif()
endif()

message(STATUS "CMAKE_CXX_SCAN_FOR_MODULES=${CMAKE_CXX_SCAN_FOR_MODULES}")

if(CMAKE_CXX_STANDARD GREATER_EQUAL 20)
    option(BEMAN_HAS_MODULES "Build CXX_MODULES" ${CMAKE_CXX_SCAN_FOR_MODULES})
endif()

# ------------------------------------------------------------------------------
# Avoid creating CMAKE_..._OUTPUT_DIRECTORY as cache variables, they should not
# be under the control of the developer. They should be controlled by the
# project because parts of the project may make assumptions about the relative
# layout of the binaries. More importantly, leaving them as ordinary variables
# also means they can be unset within subdirectories where test executables are
# defined, allowing them to avoid being collected with the other main binaries
# and cluttering up that area.
# ------------------------------------------------------------------------------
set(stageDir ${CMAKE_CURRENT_BINARY_DIR}/stagedir)
include(GNUInstallDirs)

if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${stageDir}/${CMAKE_INSTALL_BINDIR})
endif()
if(NOT CMAKE_LIBRARY_OUTPUT_DIRECTORY)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${stageDir}/${CMAKE_INSTALL_LIBDIR})
endif()
if(NOT CMAKE_ARCHIVE_OUTPUT_DIRECTORY)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${stageDir}/${CMAKE_INSTALL_LIBDIR})
endif()
