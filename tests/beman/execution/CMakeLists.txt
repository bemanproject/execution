# src/beman/execution/tests/CMakeLists.txt
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

if(BEMAN_USE_MODULES)
    list(APPEND execution_tests execution-module.test stop-token-module.test)
endif()

list(APPEND unsupported_execution_tests exec-split.test)

list(
    APPEND execution_tests
    allocator-requirements-general.test
    exec-affine-on.test
    exec-associate.test
    exec-awaitable.test
    exec-bulk.test
    exec-connect.test
    exec-continues-on.test
    exec-domain-default.test
    exec-env.test
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
    exec-on.test
    exec-opstate-start.test
    exec-opstate.test
    exec-prop.test
    exec-read-env.test
    exec-recv-concepts.test
    exec-recv.test
    exec-run-loop-general.test
    exec-run-loop-types.test
    exec-sched.test
    exec-schedule-from.test
    exec-schedule.test
    exec-scope-concepts.test
    exec-scope-counting.test
    exec-scope-simple-counting.test
    exec-sender-adaptor-closure.test
    exec-set-error.test
    exec-set-stopped.test
    exec-set-value.test
    exec-snd-apply.test
    exec-snd-concepts.test
    exec-snd-expos.test
    exec-snd-transform.test
    exec-spawn-future.test
    exec-spawn.test
    exec-starts-on.test
    exec-stop-when.test
    exec-sync-wait.test
    exec-then.test
    exec-utils-cmplsigs.test
    exec-when-all.test
    exec-with-awaitable-senders.test
    execution-queryable-concept.test
    execution-syn.test
    forward-like.test
    function-objects.test
    functional-syn.test
    issue-144.test
    issue-174.test
    issue-186.test
    meta-combine.test
    meta-contains.test
    meta-filter.test
    meta-prepend.test
    meta-transform.test
    meta-unique.test
    non_assignable.test
    notify.test
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
    set(TEST_EXE ${PROJECT_NAME}.${test})
    add_executable(${TEST_EXE} ${test}.cpp)
    if(BEMAN_USE_MODULES)
        target_compile_definitions(${TEST_EXE} PUBLIC BEMAN_HAS_MODULES)
        target_link_libraries(${TEST_EXE} PRIVATE beman::execution)
    else()
        target_link_libraries(${TEST_EXE} PRIVATE beman::execution_headers)
    endif()
    add_test(NAME ${TEST_EXE} COMMAND $<TARGET_FILE:${TEST_EXE}>)
endforeach()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    return()
endif()

#======================================================================
# NOTE: Only for Release build with enabled install rules useful! CK
#======================================================================
if(BEMAN_EXECUTION_INSTALL_CONFIG_FILE_PACKAGE AND NOT CMAKE_SKIP_INSTALL_RULES)
    # test if the targets are usable from the install directory
    add_test(
        NAME install-to-stagedir
        COMMAND
            ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix
            ${CMAKE_BINARY_DIR}/stagedir --config $<CONFIG>
    )
    add_test(
        NAME find-package-test
        COMMAND
            ${CMAKE_CTEST_COMMAND} # --verbose
            --output-on-failure -C $<CONFIG> #
            --build-and-test "${CMAKE_SOURCE_DIR}/examples"
            "${CMAKE_CURRENT_BINARY_DIR}/find-package-test" #
            --build-generator ${CMAKE_GENERATOR} #
            --build-makeprogram ${CMAKE_MAKE_PROGRAM} #
            --build-options #
            "-D BEMAN_USE_MODULES=${BEMAN_USE_MODULES}"
            "-D BEMAN_USE_STD_MODULE=${BEMAN_USE_STD_MODULE}"
            "-D CMAKE_BUILD_TYPE=$<CONFIG>"
            "-D CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-D CMAKE_CXX_EXTENSIONS=${CMAKE_CXX_EXTENSIONS}"
            "-D CMAKE_CXX_MODULE_STD=${CMAKE_CXX_MODULE_STD}"
            "-D CMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}"
            "-D CMAKE_CXX_STANDARD_REQUIRED=${CMAKE_CXX_STANDARD_REQUIRED}"
            "-D CMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/stagedir"
    )
endif()
