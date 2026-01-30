# gersemi: off
# examples/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# gersemi: on

cmake_minimum_required(VERSION 3.30...4.2)

include(../cmake/prelude.cmake)

project(beman_execution.example LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
    include(../cmake/cxx-modules-rules.cmake)

    find_package(beman_execution 0.1.0 EXACT REQUIRED)

    enable_testing()
endif()

set(EXAMPLES
    stackoverflow
    inspect
    playground
    sender-demo
    when_all-cancel
    stop_token
    stopping
    allocator
    intro-1-hello-world
    intro-2-hello-async
    intro-5-consumer
    doc-just
    doc-just_error
    doc-just_stopped
)

if(BEMAN_USE_MODULES)
    # FIXME: not yet!
    list(APPEND EXAMPLES modules) # modules.cpp
endif()

foreach(EXAMPLE ${EXAMPLES})
    set(EXAMPLE_TARGET ${PROJECT_NAME}.${EXAMPLE})
    add_executable(${EXAMPLE_TARGET})
    target_sources(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE}.cpp)
    if(BEMAN_USE_MODULES)
        target_link_libraries(${EXAMPLE_TARGET} PRIVATE beman::beman_execution)
    else()
        target_link_libraries(
            ${EXAMPLE_TARGET}
            PRIVATE beman::beman_execution_headers
        )
    endif()
    add_test(NAME ${EXAMPLE_TARGET} COMMAND ${EXAMPLE_TARGET})
endforeach()
