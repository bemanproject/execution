# src/beman/execution/tests/CMakeLists.txt
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
cmake_minimum_required(VERSION 3.25...3.31)

project(beman_execution_tests LANGUAGES CXX)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(FATAL_ERROR "In-source builds are not allowed!")
endif()

list(
  APPEND
  execution_tests
  exec-on.test
  notify.test
  exec-scounting.test
  exec-awaitable.test
  allocator-requirements-general.test
  exec-connect.test
  exec-continues-on.test
  exec-domain-default.test
  exec-fwd-env.test
  exec-general.test
  exec-get-allocator.test
  exec-get-compl-sched.test
  exec-get-delegation-scheduler.test
  exec-get-domain.test
  exec-get-env.test
  exec-get-scheduler.test
  exec-get-stop-token.test
  exec-getcomplsigs.test
  exec-into-variant.test
  exec-just.test
  exec-let.test
  exec-opstate-start.test
  exec-opstate.test
  exec-read-env.test
  exec-recv-concepts.test
  exec-recv.test
  exec-run-loop-general.test
  exec-run-loop-types.test
  exec-sched.test
  exec-schedule-from.test
  exec-schedule.test
  exec-set-error.test
  exec-set-stopped.test
  exec-set-value.test
  exec-snd-apply.test
  exec-snd-concepts.test
  exec-snd-expos.test
  exec-snd-transform.test
  exec-starts-on.test
  exec-split.test
  exec-sync-wait.test
  exec-then.test
  exec-utils-cmplsigs.test
  exec-when-all.test
  exec-with-awaitable-senders.test
  execution-queryable-concept.test
  exec-bulk.test
  execution-syn.test
  forward-like.test
  function-objects.test
  functional-syn.test
  meta-combine.test
  meta-contains.test
  meta-filter.test
  meta-prepend.test
  meta-transform.test
  meta-unique.test
  stopcallback-cons.test
  stopcallback-general.test
  stopcallback-inplace-cons.test
  stopcallback-inplace-general.test
  stopcallback-inplace.test
  stopcallback.test
  stopsource-cons.test
  stopsource-general.test
  stopsource-inplace-cons.test
  stopsource-inplace-general.test
  stopsource-inplace-mem.test
  stopsource-inplace.test
  stopsource-mem.test
  stopsource.test
  stoptoken-concepts.test
  stoptoken-general.test
  stoptoken-inplace-general.test
  stoptoken-inplace-members.test
  stoptoken-inplace.test
  stoptoken-mem.test
  stoptoken-never-general.test
  stoptoken-never.test
  stoptoken.test
  thread-stoptoken-intro.test
  thread-stoptoken-syn.compile.test
  thread-stoptoken.test
  thread.test
  utilities.test
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 23)
endif()

if(NOT DEFINED CMAKE_PREFIX_PATH)
  set(CMAKE_PREFIX_PATH ${CMAKE_INSTALL_PREFIX})
endif()

if(PROJECT_IS_TOP_LEVEL)
  enable_testing()
  find_package(beman_execution 0.0.1 EXACT QUIET)
  if(beman_execution_FOUND)
    set(execution_tests exec-awaitable.test) # only one sample to save time! CK
  else()
    add_subdirectory(../../.. beman_execution)

    include(CMakePrintHelpers)
    cmake_print_variables(TARGET_ALIAS TARGET_LIBRARY TARGET_PREFIX PROJECT_SOURCE_DIR)
  endif()
endif()

remove_definitions(-DNDEBUG) # NOTE: we want ASSERT statements in Release too! CK

foreach(test ${execution_tests})
  set(TEST_EXE ${TARGET_PREFIX}.${test})
  add_executable(${TEST_EXE} ${test}.cpp)
  target_link_libraries(${TEST_EXE} PRIVATE beman::execution)
  add_test(NAME ${TEST_EXE} COMMAND $<TARGET_FILE:${TEST_EXE}>)
endforeach()

if(NOT PROJECT_IS_TOP_LEVEL AND BEMAN_EXECUTION_ENABLE_TESTING)
  # test if the targets are findable from the build directory
  # cmake-format: off
  add_test(NAME find-package-test
    COMMAND ${CMAKE_CTEST_COMMAND}
      # --verbose
      --output-on-failure
      -C $<CONFIG>
      --build-and-test
          "${CMAKE_CURRENT_SOURCE_DIR}"
          "${CMAKE_CURRENT_BINARY_DIR}/find-package-test"
      --build-generator ${CMAKE_GENERATOR}
      --build-makeprogram ${CMAKE_MAKE_PROGRAM}
      --build-options
          "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
          "-DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
          "-DCMAKE_BUILD_TYPE=$<CONFIG>"
          "-DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}"
          # TODO(CK): Needed too? "--config $<CONFIG>"
  )
  # cmake-format: on
endif()
