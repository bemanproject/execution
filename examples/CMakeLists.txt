# cmake-format: off
# examples/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

list(
  APPEND
  EXAMPLES
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
  set(EXAMPLE_TARGET ${TARGET_PREFIX}.examples.${EXAMPLE})
  add_executable(${EXAMPLE_TARGET})
  target_sources(${EXAMPLE_TARGET} PRIVATE ${EXAMPLE}.cpp)
  target_link_libraries(${EXAMPLE_TARGET} PRIVATE ${TARGET_NAMESPACE}::${TARGET_NAME})
  add_test(NAME ${EXAMPLE_TARGET} COMMAND $<TARGET_FILE:${EXAMPLE_TARGET}>)
endforeach()
