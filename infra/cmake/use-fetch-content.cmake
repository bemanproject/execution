# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
cmake_minimum_required(VERSION 3.24)

include(FetchContent)

if(NOT BEMAN_LOCKFILE)
    set(BEMAN_LOCKFILE
        "lockfile.json"
        CACHE FILEPATH
        "Path to the dependency lockfile for the Beman project."
    )
endif()

set(Beman_projectDir "${CMAKE_CURRENT_LIST_DIR}/../..")
message(TRACE "Beman_projectDir=\"${Beman_projectDir}\"")

message(TRACE "BEMAN_LOCKFILE=\"${BEMAN_LOCKFILE}\"")
file(
    REAL_PATH "${BEMAN_LOCKFILE}"
    Beman_lockfile
    BASE_DIRECTORY "${Beman_projectDir}"
    EXPAND_TILDE
)
message(DEBUG "Using lockfile: \"${Beman_lockfile}\"")

# Force CMake to reconfigure the project if the lockfile changes
set_property(
    DIRECTORY "${Beman_projectDir}"
    APPEND
    PROPERTY CMAKE_CONFIGURE_DEPENDS "${Beman_lockfile}"
)

# For more on the protocol for this function, see:
# https://cmake.org/cmake/help/latest/command/cmake_language.html#provider-commands
function(Beman_provideDependency method package_name)
    # Read the lockfile
    file(READ "${Beman_lockfile}" Beman_rootObj)

    # Get the "dependencies" field and store it in Beman_dependenciesObj
    string(
        JSON Beman_dependenciesObj
        ERROR_VARIABLE Beman_error
        GET "${Beman_rootObj}"
        "dependencies"
    )
    if(Beman_error)
        message(FATAL_ERROR "${Beman_lockfile}: ${Beman_error}")
    endif()

    # Get the length of the libraries array and store it in Beman_dependenciesObj
    string(
        JSON Beman_numDependencies
        ERROR_VARIABLE Beman_error
        LENGTH "${Beman_dependenciesObj}"
    )
    if(Beman_error)
        message(FATAL_ERROR "${Beman_lockfile}: ${Beman_error}")
    endif()

    if(Beman_numDependencies EQUAL 0)
        return()
    endif()

    # Loop over each dependency object
    math(EXPR Beman_maxIndex "${Beman_numDependencies} - 1")
    foreach(Beman_index RANGE "${Beman_maxIndex}")
        set(Beman_errorPrefix "${Beman_lockfile}, dependency ${Beman_index}")

        # Get the dependency object at Beman_index
        # and store it in Beman_depObj
        string(
            JSON Beman_depObj
            ERROR_VARIABLE Beman_error
            GET "${Beman_dependenciesObj}"
            "${Beman_index}"
        )
        if(Beman_error)
            message(FATAL_ERROR "${Beman_errorPrefix}: ${Beman_error}")
        endif()

        # Get the "name" field and store it in Beman_name
        string(
            JSON Beman_name
            ERROR_VARIABLE Beman_error
            GET "${Beman_depObj}"
            "name"
        )
        if(Beman_error)
            message(FATAL_ERROR "${Beman_errorPrefix}: ${Beman_error}")
        endif()

        # Get the "package_name" field and store it in Beman_pkgName
        string(
            JSON Beman_pkgName
            ERROR_VARIABLE Beman_error
            GET "${Beman_depObj}"
            "package_name"
        )
        if(Beman_error)
            message(FATAL_ERROR "${Beman_errorPrefix}: ${Beman_error}")
        endif()

        # Get the "git_repository" field and store it in Beman_repo
        string(
            JSON Beman_repo
            ERROR_VARIABLE Beman_error
            GET "${Beman_depObj}"
            "git_repository"
        )
        if(Beman_error)
            message(FATAL_ERROR "${Beman_errorPrefix}: ${Beman_error}")
        endif()

        # Get the "git_tag" field and store it in Beman_tag
        string(
            JSON Beman_tag
            ERROR_VARIABLE Beman_error
            GET "${Beman_depObj}"
            "git_tag"
        )
        if(Beman_error)
            message(FATAL_ERROR "${Beman_errorPrefix}: ${Beman_error}")
        endif()

        if(method STREQUAL "FIND_PACKAGE")
            if(package_name STREQUAL Beman_pkgName)
                string(
                    APPEND Beman_debug
                    "Redirecting find_package calls for ${Beman_pkgName} "
                    "to FetchContent logic.\n"
                )
                string(
                    APPEND Beman_debug
                    "Fetching ${Beman_repo} at "
                    "${Beman_tag} according to ${Beman_lockfile}."
                )
                message(DEBUG "${Beman_debug}")
                FetchContent_Declare(
                    "${Beman_name}"
                    GIT_REPOSITORY "${Beman_repo}"
                    GIT_TAG "${Beman_tag}"
                    EXCLUDE_FROM_ALL
                )

                # Apply per-dependency cmake_args from the lockfile
                string(
                    JSON Beman_cmakeArgs
                    ERROR_VARIABLE Beman_cmakeArgsError
                    GET "${Beman_depObj}"
                    "cmake_args"
                )
                if(NOT Beman_cmakeArgsError)
                    string(JSON Beman_numCmakeArgs LENGTH "${Beman_cmakeArgs}")
                    if(Beman_numCmakeArgs GREATER 0)
                        math(EXPR Beman_maxArgIndex "${Beman_numCmakeArgs} - 1")
                        foreach(Beman_argIndex RANGE "${Beman_maxArgIndex}")
                            string(
                                JSON Beman_argKey
                                MEMBER "${Beman_cmakeArgs}"
                                "${Beman_argIndex}"
                            )
                            string(
                                JSON Beman_argValue
                                GET "${Beman_cmakeArgs}"
                                "${Beman_argKey}"
                            )
                            message(
                                DEBUG
                                "Setting ${Beman_argKey}=${Beman_argValue} for ${Beman_name}"
                            )
                            set("${Beman_argKey}" "${Beman_argValue}")
                        endforeach()
                    endif()
                endif()

                FetchContent_MakeAvailable("${Beman_name}")

                # Catch2's CTest integration module isn't on CMAKE_MODULE_PATH
                # when brought in via FetchContent. Add it so that
                # `include(Catch)` works.
                if(Beman_pkgName STREQUAL "Catch2")
                    list(
                        APPEND CMAKE_MODULE_PATH
                        "${${Beman_name}_SOURCE_DIR}/extras"
                    )
                    set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE)
                endif()

                # Important! <PackageName>_FOUND tells CMake that `find_package` is
                # not needed for this package anymore
                set("${Beman_pkgName}_FOUND" TRUE PARENT_SCOPE)
            endif()
        endif()
    endforeach()
endfunction()

set(BEMAN_USE_FETCH_CONTENT_ENABLED ON)

cmake_language(
    SET_DEPENDENCY_PROVIDER Beman_provideDependency
    SUPPORTED_METHODS FIND_PACKAGE
)

# Add this dir to the module path so that `find_package(beman-install-library)` works
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}")
