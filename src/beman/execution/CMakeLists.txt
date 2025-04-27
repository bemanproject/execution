# cmake-format: off
# src/beman/execution/CMakeLists.txt -*-makefile-*-
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
# cmake-format: on

add_library(${TARGET_NAME} STATIC)
add_library(${TARGET_ALIAS} ALIAS ${TARGET_NAME})

if(NOT BEMAN_EXECUTION_ENABLE_INSTALL OR CMAKE_SKIP_INSTALL_RULES)
  target_link_libraries(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${TARGET_NAME}_project_options>)
  target_link_libraries(${TARGET_NAME} PUBLIC $<BUILD_INTERFACE:${TARGET_NAME}_project_warnings>)
endif()

include(CMakePrintHelpers)
cmake_print_variables(TARGET_ALIAS TARGET_NAME TARGET_PREFIX PROJECT_SOURCE_DIR)

target_sources(
  ${TARGET_NAME}
  PRIVATE execution.cpp
  PUBLIC FILE_SET
         ${TARGET_NAME}_public_headers
         TYPE
         HEADERS
         BASE_DIRS
         ${PROJECT_SOURCE_DIR}/include
         FILES
         ${PROJECT_SOURCE_DIR}/include/beman/execution/execution.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/functional.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/stop_token.hpp
  PUBLIC FILE_SET
         ${TARGET_NAME}_detail_headers
         TYPE
         HEADERS
         BASE_DIRS
         ${PROJECT_SOURCE_DIR}/include
         FILES
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/allocator_aware_move.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/almost_scheduler.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/apply_sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/as_awaitable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/as_except_ptr.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/atomic_intrusive_stack.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/await_result_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/await_suspend_result.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/awaitable_sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/basic_operation.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/basic_receiver.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/basic_sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/basic_state.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/call_result_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/callable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/check_type_alias_exist.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/child_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/class_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/common.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/completion_domain.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/completion_signature.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/completion_signatures.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/completion_signatures_for.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/completion_signatures_of_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/completion_tag.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/connect.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/connect_all.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/connect_all_result.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/connect_awaitable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/connect_result_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/continues_on.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/decayed_same_as.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/decayed_tuple.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/decayed_type_list.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/decayed_typeof.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/decays_to.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/default_domain.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/default_impls.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/emplace_from.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/empty_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/env_of_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/env_promise.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/env_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/error_types_of_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/forward_like.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/forwarding_query.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/fwd_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/gather_signatures.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/get_allocator.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/get_awaiter.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/get_completion_scheduler.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/get_completion_signatures.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/get_delegation_scheduler.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/get_domain.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/get_domain_early.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/get_domain_late.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/get_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/get_scheduler.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/get_stop_token.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/has_as_awaitable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/has_completions.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/immovable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/impls_for.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/indices_for.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/indirect_meta_apply.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/inplace_stop_source.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/into_variant.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/intrusive_stack.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/is_awaitable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/is_awaiter.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/join_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/just.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/let.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/make_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/make_sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/matching_sig.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/meta_combine.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/meta_contains.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/meta_filter.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/meta_prepend.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/meta_size.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/meta_to.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/meta_transform.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/meta_unique.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/movable_value.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/never_stop_token.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/nostopstate.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/nothrow_callable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/notify.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/on.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/on_stop_request.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/operation_state.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/operation_state_task.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/product_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/query_with_default.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/queryable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/read_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/receiver.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/receiver_of.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/run_loop.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/sched_attrs.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/sched_env.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/schedule.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/schedule_from.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/schedule_result_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/scheduler.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/scheduler_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/sender_adaptor.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/sender_adaptor_closure.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/sender_awaitable.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/sender_decompose.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/sender_for.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/sender_in.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/sends_stopped.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/set_error.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/set_stopped.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/set_value.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/simple_allocator.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/simple_counting_scope.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/single_sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/single_sender_value_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/split.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/start.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/starts_on.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/state_type.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/stop_callback_for_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/stop_source.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/stop_token_of_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/stoppable_source.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/stoppable_token.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/suppress_pop.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/suppress_push.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/suspend_complete.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/sync_wait.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/tag_of_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/then.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/transform_sender.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/type_list.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/unspecified_promise.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/unstoppable_token.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/valid_completion_for.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/valid_completion_signatures.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/valid_specialization.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/value_types_of_t.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/variant_or_empty.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/when_all.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/when_all_with_variant.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/with_await_transform.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/with_awaitable_senders.hpp
         ${PROJECT_SOURCE_DIR}/include/beman/execution/detail/write_env.hpp
)

# cmake-format: off
get_property(DETAIL_HEADER_FILES TARGET ${TARGET_NAME} PROPERTY HEADER_SET_${TARGET_NAME}_detail_headers)
source_group("Header Files\\detail" FILES ${DETAIL_HEADER_FILES})

set_target_properties(${TARGET_NAME} PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON)

target_compile_features(${TARGET_NAME} PUBLIC
  "$<$<COMPILE_FEATURES:cxx_std_26>:cxx_std_26>"
  "$<$<NOT:$<COMPILE_FEATURES:cxx_std_26>>:cxx_std_23>"
)

if(NOT BEMAN_EXECUTION_ENABLE_INSTALL OR CMAKE_SKIP_INSTALL_RULES)
  return()
endif()

install(
  TARGETS ${TARGET_NAME}
  EXPORT ${TARGETS_EXPORT_NAME}1
  ARCHIVE DESTINATION lib/$<CONFIG>
  FILE_SET ${TARGET_NAME}_public_headers
  FILE_SET ${TARGET_NAME}_detail_headers
)
# cmake-format: on

install(
  EXPORT ${TARGETS_EXPORT_NAME}1
  FILE ${TARGETS_EXPORT_NAME}.cmake
  DESTINATION "${INSTALL_CONFIGDIR}"
  NAMESPACE ${TARGET_NAMESPACE}::
)
