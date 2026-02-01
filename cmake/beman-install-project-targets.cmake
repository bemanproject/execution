# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
cmake_minimum_required(VERSION 4.0)

include_guard(GLOBAL)

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# beman_install_library
# =====================
#
# Installs a library (or set of targets) along with headers, C++ modules,
# and optional CMake package configuration files.
#
# Usage:
# ------
#   beman_install_library(<name>
#     TARGETS target1 [target2 ...]
#     [NAMESPACE <namespace>]
#     [EXPORT_NAME <export-name>]
#     [DESTINATION <install-prefix>]
#   )
#
# Arguments:
# ----------
# name
#   Logical package name (e.g. "beman.utility").
#   Used to derive config file names and cache variable prefixes.
#
# TARGETS (required)
#   List of CMake targets to install.
#
# NAMESPACE (optional)
#   Namespace for exported targets.
#   Defaults to "beman::".
#
# EXPORT_NAME (optional)
#   Name of the CMake export set.
#   Defaults to "<name>-targets".
#
# DESTINATION (optional)
#   Base install destination.
#   Defaults to CMAKE_INSTALL_INCLUDEDIR/beman/modules.
#
# Cache variables:
# ----------------
# BEMAN_INSTALL_CONFIG_FILE_PACKAGES
#   List of package names for which config files should be installed.
#
# <PREFIX>_INSTALL_CONFIG_FILE_PACKAGE
#   Per-package override to enable/disable config file installation.
#   <PREFIX> is the uppercased package name with dots replaced by underscores.
#
function(beman_install_library name)
    # ----------------------------
    # Argument parsing
    # ----------------------------
    set(options)
    set(oneValueArgs NAMESPACE EXPORT_NAME DESTINATION)
    set(multiValueArgs TARGETS)

    cmake_parse_arguments(
        BEMAN
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(NOT BEMAN_TARGETS)
        message(
            FATAL_ERROR
            "beman_install_library(${name}): TARGETS must be specified"
        )
    endif()

    # ----------------------------
    # Defaults
    # ----------------------------
    if(NOT BEMAN_NAMESPACE)
        set(BEMAN_NAMESPACE "beman::")
    endif()

    if(NOT BEMAN_EXPORT_NAME)
        set(BEMAN_EXPORT_NAME "${name}-targets")
    endif()

    if(NOT BEMAN_DESTINATION)
        set(BEMAN_DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/beman/modules")
    endif()

    if(CMAKE_SKIP_INSTALL_RULES)
        message(
            WARNING
            "beman_install_library(${name}): not installing targets '${BEMAN_TARGETS}' due to CMAKE_SKIP_INSTALL_RULES"
        )
        return()
    endif()

    # ----------------------------------------
    # Install targets and headers (always)
    # ----------------------------------------
    install(
        TARGETS ${BEMAN_TARGETS}
        EXPORT ${BEMAN_EXPORT_NAME}
        ARCHIVE # DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY # DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME # DESTINATION ${CMAKE_INSTALL_BINDIR}
        FILE_SET
            HEADERS # DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        FILE_SET CXX_MODULES DESTINATION "${BEMAN_DESTINATION}"
    )

    # --------------------------------------------------
    # Export targets
    # --------------------------------------------------
    install(
        EXPORT ${BEMAN_EXPORT_NAME}
        NAMESPACE ${BEMAN_NAMESPACE}
        CXX_MODULES_DIRECTORY
        cxx-modules
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${name}
    )

    # ----------------------------------------
    # Config file installation logic
    # ----------------------------------------
    string(TOUPPER "${name}" _pkg_upper)
    string(REPLACE "." "_" _pkg_prefix "${_pkg_upper}")

    set(_pkg_var "${_pkg_prefix}_INSTALL_CONFIG_FILE_PACKAGE")

    if(NOT DEFINED ${_pkg_var})
        set(${_pkg_var}
            OFF
            CACHE BOOL
            "Install CMake package config files for ${name}"
        )
    endif()

    set(_install_config OFF)

    if(${_pkg_var})
        set(_install_config ON)
    elseif(BEMAN_INSTALL_CONFIG_FILE_PACKAGES)
        list(FIND BEMAN_INSTALL_CONFIG_FILE_PACKAGES "${name}" _idx)
        if(NOT _idx EQUAL -1)
            set(_install_config ON)
        endif()
    endif()

    # ----------------------------------------
    # Generate + install config files
    # ----------------------------------------
    if(_install_config)
        set(_config_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${name}")

        configure_package_config_file(
            "${CMAKE_CURRENT_FUNCTION_LIST_DIR}/${name}-config.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/${name}-config.cmake"
            INSTALL_DESTINATION ${_config_install_dir}
        )

        write_basic_package_version_file(
            "${CMAKE_CURRENT_BINARY_DIR}/${name}-config-version.cmake"
            VERSION ${PROJECT_VERSION}
            COMPATIBILITY SameMajorVersion
        )

        install(
            FILES
                "${CMAKE_CURRENT_BINARY_DIR}/${name}-config.cmake"
                "${CMAKE_CURRENT_BINARY_DIR}/${name}-config-version.cmake"
            DESTINATION ${_config_install_dir}
        )
    endif()
endfunction()
