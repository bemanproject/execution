// src/Beman/Execution26/tests/execution-syn.pass.cpp                 -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <Beman/Execution26/detail/schedule_result_t.hpp>
#include <Beman/Execution26/execution.hpp>
#include <test/execution.hpp>
#include <concepts>

// ----------------------------------------------------------------------------

namespace
{
    struct scheduler
    {
        struct env
        {
            template <test_detail::completion_tag Tag>
            auto query(test_std::get_completion_scheduler_t<Tag> const&) const noexcept
                -> scheduler { return {}; }
        };
        struct sender
        {
            using sender_concept = test_std::sender_t;
            auto get_env() const noexcept -> env { return {}; }
        };
        using scheduler_concept = test_std::scheduler_t;

        auto schedule() const noexcept -> sender { return {}; }
        auto operator== (scheduler const&) const -> bool = default;
    };

    auto test_schedule_result_t() -> void
    {
        static_assert(test_std::sender<scheduler::sender>);
        static_assert(test_std::scheduler<scheduler>);
        static_assert(std::same_as<scheduler::sender, test_std::schedule_result_t<scheduler>>);
    }
}

auto main() -> int
{
    test_schedule_result_t();
}