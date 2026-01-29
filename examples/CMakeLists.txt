# gersemi: off
# examples/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# gersemi: on

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
    # FIXME: not yet! list(APPEND EXAMPLES modules) # modules.cpp
endif()

foreach(EXAMPLE ${EXAMPLES})
    set(EXAMPLE_TARGET ${TARGET_PREFIX}.examples.${EXAMPLE})
    add_executable(${EXAMPLE_TARGET})
    target_sources(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE}.cpp)
    if(BEMAN_USE_MODULES)
        target_link_libraries(${EXAMPLE_TARGET} PRIVATE beman::execution)
    else()
        target_link_libraries(
            ${EXAMPLE_TARGET}
            PRIVATE beman::execution_headers
        )
    endif()
endforeach()
