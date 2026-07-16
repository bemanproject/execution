# gersemi: off
# examples/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# gersemi: on

cmake_minimum_required(VERSION 3.30...4.4)

include(../cmake/prelude.cmake)

project(beman_execution.example LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL)
    include(../cmake/cxx-modules-rules.cmake)

    find_package(beman.execution 0.3.0 EXACT REQUIRED)

    enable_testing()
endif()

set(TODO stop_token) #-dk:TODO including that causes a linker error
set(TODO suspend_never) #-dk:TODO including that causes ASAN errors

set(EXAMPLES
    allocator
    doc_just
    doc_just_error
    doc_just_stopped
    inspect
    intro_1_hello_world
    intro_2_hello_async
    intro_5_consumer
    playground
    sender_demo
    stackoverflow
    stopping
    when_all_cancel
)

if(BEMAN_USE_MODULES)
    #-dk:TODO gcc doesn't like the modules: list(APPEND EXAMPLES modules modules_and_header)
    list(APPEND EXAMPLES modules_and_header)
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
