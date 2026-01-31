# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
include_guard(GLOBAL)

# This file defines the function `beman_install_library` which is used to
# install a library target and its headers, along with optional CMake
# configuration files.
#
# The function is designed to be reusable across different Beman libraries.

function(beman_install_library name interface)
    # Usage
    # -----
    #
    #     beman_install_library(NAME INTERFACE)
    #
    # Brief
    # -----
    #
    # This function installs the specified library target and its headers.
    # It also handles the installation of the CMake configuration files if needed.
    #
    # CMake variables
    # ---------------
    #
    # Note that configuration of the installation is generally controlled by CMake
    # cache variables so that they can be controlled by the user or tool running the
    # `cmake` command. Neither `CMakeLists.txt` nor `*.cmake` files should set these
    # variables directly.
    #
    # - BEMAN_INSTALL_CONFIG_FILE_PACKAGES:
    #      List of packages that require config file installation.
    #      If the package name is in this list, it will install the config file.
    #
    # - <PREFIX>_INSTALL_CONFIG_FILE_PACKAGE:
    #      Boolean to control config file installation for the specific library.
    #      The prefix `<PREFIX>` is the uppercased name of the library with dots
    #      replaced by underscores.
    #
    # if(NOT TARGET "${name}")
    #     message(FATAL_ERROR "Target '${name}' does not exist.")
    # endif()

    # if(NOT ARGN STREQUAL "")
    #     message(
    #         FATAL_ERROR
    #         "beman_install_library does not accept extra arguments: ${ARGN}"
    #     )
    # endif()

    # XXX Given foo.bar, the component name is bar
    # string(REPLACE "." ";" name_parts "${name}")
    # XXX fail if the name doesn't look like foo.bar
    # list(LENGTH name_parts name_parts_length)
    # if(NOT name_parts_length EQUAL 2)
    #     message(
    #         FATAL_ERROR
    #         "beman_install_library expects a name of the form 'beman.<name>', got '${name}'"
    #     )
    # endif()

    string(REPLACE "beman_" "" name_parts "${name}")
    set(component_name ${name_parts})

    set(target_name "${name}")

    # COMPONENT <component>
    # Specify an installation component name with which the install rule is associated,
    # such as Runtime or Development.
    set(install_component_name "${name}") # TODO(CK): this is not common name!

    set(export_name "${name}")
    set(package_name "${name}")
    # XXX list(GET name_parts -1 component_name)

    include(GNUInstallDirs)

    set(package_install_dir "${CMAKE_INSTALL_LIBDIR}/cmake/${package_name}")

    set(target_list)
    if(TARGET "${target_name}")
        set_target_properties(
            "${target_name}"
            PROPERTIES EXPORT_NAME "${component_name}"
        )
        message(
            VERBOSE
            "beman-install-library: COMPONENT ${component_name} for TARGET '${target_name}'"
        )
        list(APPEND target_list "${target_name}")

        get_target_property(
            INTERFACE_CXX_MODULE_SETS
            ${target_name}
            INTERFACE_CXX_MODULE_SETS
        )
        if(INTERFACE_CXX_MODULE_SETS)
            message(
                VERBOSE
                "beman-install-library: '${target_name}' has INTERFACE_CXX_MODULE_SETS=${INTERFACE_CXX_MODULE_SETS}"
            )
            set(__INSTALL_CXX_MODULES
                FILE_SET
                ${INTERFACE_CXX_MODULE_SETS}
                DESTINATION
                ${package_install_dir}/modules
            )
        endif()
    endif()

    if(interface AND TARGET "${target_name}_${interface}")
        set_target_properties(
            "${target_name}_${interface}"
            PROPERTIES EXPORT_NAME "${component_name}_${interface}"
        )
        message(
            VERBOSE
            "beman-install-library: COMPONENT ${component_name} for TARGET '${name}_${interface}'"
        )
        list(APPEND target_list "${target_name}_${interface}")

        get_target_property(
            INTERFACE_HEADER_SETS
            ${target_name}_${interface}
            INTERFACE_HEADER_SETS
        )
        if(INTERFACE_HEADER_SETS)
            message(
                VERBOSE
                "beman-install-library: '${target_name}_${interface}' has INTERFACE_HEADER_SETS=${INTERFACE_HEADER_SETS}"
            )
            set(__INSTALL_HEADER_SETS FILE_SET ${INTERFACE_HEADER_SETS})
        endif()
    endif()

    if(CMAKE_SKIP_INSTALL_RULES)
        message(
            DEBUG
            "beman-install-library: not installing targets '${target_list}' due to CMAKE_SKIP_INSTALL_RULES"
        )
        return()
    endif()

    # ============================================================
    install(
        TARGETS ${target_list}
        COMPONENT "${install_component_name}"
        EXPORT "${export_name}"
        FILE_SET HEADERS
            ${__INSTALL_HEADER_SETS}
            # FILE_SET CXX_MODULES
            #     DESTINATION ${package_install_dir}/modules
            ${__INSTALL_CXX_MODULES}
        # There's currently no convention for this location
        CXX_MODULES_BMI
            DESTINATION
                ${package_install_dir}/bmi-${CMAKE_CXX_COMPILER_ID}_$<CONFIG>
    )
    # ============================================================

    # Determine the prefix for project-specific variables
    string(TOUPPER "${name}" project_prefix)
    # XXX string(REPLACE "." "_" project_prefix "${project_prefix}")

    option(
        ${project_prefix}_INSTALL_CONFIG_FILE_PACKAGE
        "Enable creating and installing a CMake config-file package. Default: ON. Values: { ON, OFF }."
        ON
    )

    # By default, install the config package
    set(install_config_package ON)

    # Turn OFF installation of config package by default if,
    # in order of precedence:
    # 1. The specific package variable is set to OFF
    # 2. The package name is not in the list of packages to install config files
    if(DEFINED BEMAN_INSTALL_CONFIG_FILE_PACKAGES)
        if(
            NOT "${install_component_name}"
                IN_LIST
                BEMAN_INSTALL_CONFIG_FILE_PACKAGES
        )
            set(install_config_package OFF)
        endif()
    endif()
    if(DEFINED ${project_prefix}_INSTALL_CONFIG_FILE_PACKAGE)
        set(install_config_package
            ${${project_prefix}_INSTALL_CONFIG_FILE_PACKAGE}
        )
    endif()

    if(install_config_package)
        message(
            VERBOSE
            "beman-install-library: Export cmake config to ${package_install_dir} for '${name}'"
        )

        include(CMakePackageConfigHelpers)

        find_file(
            config_file_template
            NAMES "${package_name}-config.cmake.in"
            PATHS "${PROJECT_SOURCE_DIR}/cmake"
            NO_DEFAULT_PATH
            NO_CACHE
            REQUIRED
        )
        set(config_package_file
            "${CMAKE_CURRENT_BINARY_DIR}/${package_name}-config.cmake"
        )
        configure_package_config_file(
            "${config_file_template}"
            "${config_package_file}"
            INSTALL_DESTINATION "${package_install_dir}"
            PATH_VARS PROJECT_NAME PROJECT_VERSION
        )

        set(config_version_file
            "${CMAKE_CURRENT_BINARY_DIR}/${package_name}-config-version.cmake"
        )
        write_basic_package_version_file(
            "${config_version_file}"
            VERSION "${PROJECT_VERSION}"
            COMPATIBILITY ExactVersion
        )

        install(
            FILES "${config_package_file}" "${config_version_file}"
            DESTINATION "${package_install_dir}"
            COMPONENT "${install_component_name}"
        )

        # NOTE: must be same value as ${TARGETS_EXPORT_NAME}.cmake! CK
        set(config_targets_file "${package_name}-targets.cmake")
        install(
            EXPORT "${export_name}"
            DESTINATION "${package_install_dir}"
            NAMESPACE beman::
            FILE "${config_targets_file}"
            CXX_MODULES_DIRECTORY
            cxx-modules
            COMPONENT "${install_component_name}"
        )
    else()
        message(
            WARNING
            "beman-install-library: Not installing a config package for '${name}'"
        )
    endif()
endfunction()
