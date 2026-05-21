// src/beman/execution/tests/exec-parallel-scheduler.test.cpp       -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <concepts>
#include <cstddef>
#include <exception>
#include <memory>
#include <optional>
#include <span>
#include <type_traits>
#include <utility>
#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES 
import beman.execution;
import beman.execution.detail.schedule_result_t;
#else
#include <beman/execution/execution.hpp>
#endif

namespace {
namespace replaceability = test_std::system_context_replaceability;

struct proxy : replaceability::receiver_proxy {
    auto set_value() noexcept -> void override {}
    auto set_error(::std::exception_ptr) noexcept -> void override {}
    auto set_stopped() noexcept -> void override {}
};

struct bulk_proxy : replaceability::bulk_item_receiver_proxy {
    auto set_value() noexcept -> void override {}
    auto set_error(::std::exception_ptr) noexcept -> void override {}
    auto set_stopped() noexcept -> void override {}
    auto execute(::std::size_t, ::std::size_t) noexcept -> void override {}
};

struct backend : replaceability::parallel_scheduler_backend {
    auto schedule(replaceability::receiver_proxy&, ::std::span<::std::byte>) noexcept -> void override {}
    auto schedule_bulk_chunked(::std::size_t,
                               replaceability::bulk_item_receiver_proxy&,
                               ::std::span<::std::byte>) noexcept -> void override {}
    auto schedule_bulk_unchunked(::std::size_t,
                                 replaceability::bulk_item_receiver_proxy&,
                                 ::std::span<::std::byte>) noexcept -> void override {}
};

auto test_parallel_scheduler_synopsis() -> void {
    static_assert(!::std::default_initializable<test_std::parallel_scheduler>);
    static_assert(::std::copy_constructible<test_std::parallel_scheduler>);
    static_assert(::std::move_constructible<test_std::parallel_scheduler>);
    static_assert(test_std::scheduler<test_std::parallel_scheduler>);

    static_assert(::std::same_as<decltype(test_std::get_parallel_scheduler()), test_std::parallel_scheduler>);
    static_assert(::std::same_as<test_std::schedule_result_t<test_std::parallel_scheduler>,
                                 test_std::parallel_scheduler::sender>);
    static_assert(test_std::sender<test_std::parallel_scheduler::sender>);
    static_assert(::std::same_as<
                  decltype(test_std::get_completion_signatures<test_std::parallel_scheduler::sender>()),
                  test_std::completion_signatures<test_std::set_value_t(),
                                                  test_std::set_error_t(::std::exception_ptr),
                                                  test_std::set_stopped_t()>>);

    static_assert(noexcept(test_std::get_forward_progress_guarantee(
        ::std::declval<const test_std::parallel_scheduler&>())));
    static_assert(::std::same_as<decltype(test_std::get_forward_progress_guarantee(
                                      ::std::declval<const test_std::parallel_scheduler&>())),
                                 test_std::forward_progress_guarantee>);
}

auto test_replaceability_synopsis() -> void {
    static_assert(::std::is_abstract_v<replaceability::receiver_proxy>);
    static_assert(::std::is_abstract_v<replaceability::bulk_item_receiver_proxy>);
    static_assert(::std::is_abstract_v<replaceability::parallel_scheduler_backend>);
    static_assert(::std::derived_from<bulk_proxy, replaceability::receiver_proxy>);
    static_assert(::std::derived_from<backend, replaceability::parallel_scheduler_backend>);
    static_assert(::std::same_as<decltype(::std::declval<proxy&>().template try_query<int>(0)),
                                 ::std::optional<int>>);
    static_assert(::std::same_as<decltype(replaceability::query_parallel_scheduler_backend()),
                                 ::std::shared_ptr<replaceability::parallel_scheduler_backend>>);
}
} // namespace

TEST(exec_parallel_scheduler) {
    test_parallel_scheduler_synopsis();
    test_replaceability_synopsis();
}
