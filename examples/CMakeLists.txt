# cmake-format: off
# examples/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

cmake_minimum_required(VERSION 3.28...4.0)

project(beman.execution.examples DESCRIPTION "Examaples for Beman implementation of asynchronous components"
        LANGUAGES CXX VERSION 0.0.2
)

if(PROJECT_IS_TOP_LEVEL)
  if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
  endif()

  find_package(beman.execution 0.0.2 EXACT)
  if(NOT beman.execution_FOUND)
    add_subdirectory(.. beman_execution)
  endif()

  enable_testing()
endif()

include(CMakePrintHelpers)
cmake_print_variables(CMAKE_CXX_STANDARD PROJECT_NAME PROJECT_VERSION PROJECT_SOURCE_DIR)

set(EXAMPLES
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

foreach(EXAMPLE ${EXAMPLES})
  set(EXAMPLE_TARGET beman.execution.examples.${EXAMPLE})
  add_executable(${EXAMPLE_TARGET})
  target_sources(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE}.cpp)
  target_link_libraries(${EXAMPLE_TARGET} PRIVATE beman::execution)
  add_test(NAME ${EXAMPLE_TARGET} COMMAND $<TARGET_FILE:${EXAMPLE_TARGET}>)
endforeach()
