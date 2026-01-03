# gersemi: off
# examples/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# gersemi: on

list(
    APPEND EXAMPLES
sync_wait-just
sync_wait-just_error
sync_wait-just_stopped
just-then
just-then-throw
just_error-then
just_stopped-then
upon_error
expected
upon_stopped
when_all
)

if(BEMAN_USE_MODULES)
    list(APPEND EXAMPLES modules) # modules.cpp
endif()

foreach(EXAMPLE ${EXAMPLES})
    set(EXAMPLE_TARGET ${TARGET_PREFIX}.examples.${EXAMPLE})
    add_executable(${EXAMPLE_TARGET})
    target_sources(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE}.cpp)
    target_link_libraries(
        ${EXAMPLE_TARGET}
        PRIVATE ${TARGET_NAMESPACE}::${TARGET_NAME}
    )
endforeach()
