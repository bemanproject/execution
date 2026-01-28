# gersemi: off
# docs/code/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# gersemi: on

list(APPEND EXAMPLES)

# if(BEMAN_USE_MODULES)
#     list(APPEND EXAMPLES modules) # missing modules.cpp
# endif()

foreach(EXAMPLE ${EXAMPLES})
    set(EXAMPLE_TARGET ${TARGET_PREFIX}.tutorial.${EXAMPLE})
    add_executable(${EXAMPLE_TARGET})
    target_sources(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE}.cpp)
    target_link_libraries(
        ${EXAMPLE_TARGET}
        PRIVATE ${TARGET_NAMESPACE}::${TARGET_NAME}
    )
endforeach()
