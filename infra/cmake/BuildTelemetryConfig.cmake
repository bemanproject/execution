# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
include_guard(GLOBAL)

set(BUILD_TELEMETRY_DIR ${CMAKE_CURRENT_LIST_DIR})

function(configure_build_telemetry)
    if(NOT BUILD_TELEMETRY_CONFIGURATION)
        # Check if the CMake version is at least 4.3
        if(CMAKE_VERSION VERSION_LESS "4.3")
            message(
                STATUS
                "CMake version is less than 4.3, configuring cmake_instrumentation is unavailable."
            )
            return()
        else()
            message(STATUS "Configuring Build Telemetry")
        endif()

        # Find bash and jq for the telemetry callback script.
        # On Windows, Git for Windows provides bash if available.
        find_program(BEMAN_BASH bash)
        find_program(BEMAN_JQ jq)
        if(NOT BEMAN_BASH OR NOT BEMAN_JQ)
            message(
                STATUS
                "bash or jq not found, build telemetry disabled on this platform."
            )
            return()
        endif()

        # Telemetry query
        cmake_instrumentation(
            API_VERSION 1
            DATA_VERSION 1
            OPTIONS staticSystemInformation dynamicSystemInformation trace
            HOOKS
                postGenerate
                preBuild
                postBuild
                preCMakeBuild
                postCMakeBuild
                postCMakeInstall
                postCTest
            CALLBACK ${BEMAN_BASH}
            ${BUILD_TELEMETRY_DIR}/telemetry.sh
        )
        message(
            DEBUG
            "using callback script ${BUILD_TELEMETRY_DIR}/telemetry.sh via ${BEMAN_BASH}"
        )

        # Mark configuration as done in cache
        set(BUILD_TELEMETRY_CONFIGURATION
            TRUE
            CACHE INTERNAL
            "Flag to ensure Build Telemetry configured only once"
        )
    endif()
endfunction(configure_build_telemetry)
