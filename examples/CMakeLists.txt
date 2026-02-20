# gersemi: off
# examples/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# gersemi: on

cmake_minimum_required(VERSION 3.30...4.3)

include(../cmake/prelude.cmake)

project(beman_execution.example LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
    include(../cmake/cxx-modules-rules.cmake)

    find_package(beman.execution 0.2.0 EXACT REQUIRED)

    enable_testing()
endif()

set(EXAMPLES
    allocator
    doc-just
    doc-just_error
    doc-just_stopped
    inspect
    intro-1-hello-world
    intro-2-hello-async
    intro-5-consumer
    playground
    sender-demo
    stackoverflow
    stop_token
    stopping
    when_all-cancel
)

if(BEMAN_USE_MODULES)
    #-dk:TODO gcc doesn't like the modules: list(APPEND EXAMPLES modules modules-and-header)
    list(APPEND EXAMPLES modules-and-header)
endif()

foreach(EXAMPLE ${EXAMPLES})
    set(EXAMPLE_TARGET ${PROJECT_NAME}.${EXAMPLE})
    add_executable(${EXAMPLE_TARGET})
    target_sources(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE}.cpp)
    if(BEMAN_USE_MODULES)
        target_compile_definitions(${EXAMPLE_TARGET} PUBLIC BEMAN_HAS_MODULES)
        target_link_libraries(${EXAMPLE_TARGET} PRIVATE beman::execution)
    else()
        target_link_libraries(
            ${EXAMPLE_TARGET}
            PRIVATE beman::execution_headers
        )
    endif()
    add_test(NAME ${EXAMPLE_TARGET} COMMAND ${EXAMPLE_TARGET})
endforeach()
