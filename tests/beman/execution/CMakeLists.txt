# src/beman/execution/tests/CMakeLists.txt
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
cmake_minimum_required(VERSION 3.25...4.2)

project(beman_execution_tests LANGUAGES CXX)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds are not allowed!")
endif()

list(APPEND todo exec-associate.test)

if(BEMAN_USE_MODULES)
    list(
        APPEND execution_tests
        execution-module.test # execution-module.test.cpp
        stop-token-module.test # stop-token-module.test.cpp
    )
endif()

list(
    APPEND execution_tests
    issue-174.test
    issue-186.test
    exec-scope-counting.test
    exec-spawn.test
    exec-stop-when.test
    non_assignable.test
    exec-prop.test
    exec-env.test
    exec-scope-simple-counting.test
    exec-spawn-future.test
    exec-scope-concepts.test
    issue-144.test
    exec-on.test
    notify.test
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

foreach(test ${execution_tests})
    set(TEST_EXE ${TARGET_PREFIX}.${test})
    add_executable(${TEST_EXE} ${test}.cpp)
    target_link_libraries(${TEST_EXE} PRIVATE beman::execution)
    add_test(NAME ${TEST_EXE} COMMAND $<TARGET_FILE:${TEST_EXE}>)
endforeach()
