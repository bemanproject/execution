// tests/beman/execution/exec-queries-expos.test.cpp                  -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.get_domain;
import beman.execution.detail.get_scheduler;
import beman.execution.detail.hide_query;
import beman.execution.detail.try_query;
#else
#include <beman/execution/detail/get_domain.hpp>
#include <beman/execution/detail/get_scheduler.hpp>
#include <beman/execution/detail/hide_query.hpp>
#include <beman/execution/detail/try_query.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
template <::std::size_t>
struct arg {
    int value{};
};
template <::std::size_t>
struct get_arg {};

struct test_queryable {
    int query(get_arg<0>) const noexcept { return 42; }

    int query(get_arg<1>) const noexcept { return 43; }
    int query(get_arg<1>, arg<0>) const noexcept { return 44; }

    int query(get_arg<2>, arg<0>) const noexcept { return 45; }
    int query(get_arg<2>, arg<0>, arg<1>) const noexcept { return 46; }

    int query(test_std::get_domain_t) const noexcept { return 47; }
    int query(test_std::get_domain_t, arg<0>) const noexcept { return 48; }
    int query(test_std::get_domain_t, arg<0>, arg<1>) const noexcept { return 49; }

    int query(test_std::get_scheduler_t) const noexcept { return 50; }
    int query(test_std::get_scheduler_t, arg<0>) const noexcept { return 51; }
    int query(test_std::get_scheduler_t, arg<0>, arg<1>) const noexcept { return 52; }
};

template <bool Value, typename Q, typename Tag, typename... Args>
auto test_try_query_exists(int expect, Q q, Tag&& tag, Args&&... args) -> void {
    static_assert(Value ==
                  requires { test_detail::try_query(q, ::std::forward<Tag>(tag), ::std::forward<Args>(args)...); });
    if constexpr (Value) {
        ASSERT(expect == test_detail::try_query(q, ::std::forward<Tag>(tag), ::std::forward<Args>(args)...));
    }
}

auto test_try_query() -> void {
    test_try_query_exists<true>(42, test_queryable{}, get_arg<0>{});
    test_try_query_exists<true>(42, test_queryable{}, get_arg<0>{}, arg<0>{});
    test_try_query_exists<true>(42, test_queryable{}, get_arg<0>{}, arg<0>{}, arg<1>{});

    test_try_query_exists<true>(43, test_queryable{}, get_arg<1>{});
    test_try_query_exists<true>(44, test_queryable{}, get_arg<1>{}, arg<0>{});
    test_try_query_exists<true>(43, test_queryable{}, get_arg<1>{}, arg<0>{}, arg<1>{});

    test_try_query_exists<false>(0, test_queryable{}, get_arg<2>{});
    test_try_query_exists<true>(45, test_queryable{}, get_arg<2>{}, arg<0>{});
    test_try_query_exists<true>(46, test_queryable{}, get_arg<2>{}, arg<0>{}, arg<1>{});
}

template <bool Value, typename Q, typename Tag, typename... Args>
auto test_hide_query_exists(int expect, Q q, Tag&& tag, Args&&... args) -> void {
    static_assert(Value == requires {
        test_detail::hide_query(q).query(::std::forward<Tag>(tag), ::std::forward<Args>(args)...);
    });
    if constexpr (Value) {
        ASSERT(expect == test_detail::hide_query(q).query(::std::forward<Tag>(tag), ::std::forward<Args>(args)...));
    }
}

auto test_hide_query() -> void {
    test_hide_query_exists<true>(42, test_queryable{}, get_arg<0>{});
    test_hide_query_exists<true>(44, test_queryable{}, get_arg<1>{}, arg<0>{});
    test_hide_query_exists<true>(46, test_queryable{}, get_arg<2>{}, arg<0>{}, arg<1>{});

    test_hide_query_exists<false>(47, test_queryable{}, test_std::get_domain);
    test_hide_query_exists<false>(48, test_queryable{}, test_std::get_domain, arg<0>{});
    test_hide_query_exists<false>(49, test_queryable{}, test_std::get_domain, arg<0>{}, arg<1>{});

    test_hide_query_exists<false>(50, test_queryable{}, test_std::get_scheduler);
    test_hide_query_exists<false>(51, test_queryable{}, test_std::get_scheduler, arg<0>{});
    test_hide_query_exists<false>(52, test_queryable{}, test_std::get_scheduler, arg<0>{}, arg<1>{});
}
} // namespace

TEST(exec_queries_expos) {
    test_try_query();
    test_hide_query();
}
