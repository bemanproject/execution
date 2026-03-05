// tests/beman/execution/exec.get.compl.domain.test.cpp               -*-C++-*-
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <test/execution.hpp>
#ifdef BEMAN_HAS_MODULES
import beman.execution.detail.get_completion_domain;
import beman.execution.detail.set_error;
import beman.execution.detail.set_stopped;
import beman.execution.detail.set_value;
#else
#include <beman/execution/detail/get_completion_domain.hpp>
#include <beman/execution/detail/set_error.hpp>
#include <beman/execution/detail/set_stopped.hpp>
#include <beman/execution/detail/set_value.hpp>
#endif

// ----------------------------------------------------------------------------

namespace {
template <bool Value, typename CPO>
void test_get_completion_domain_template() {
    static_assert(Value == requires { beman::execution::get_completion_domain<CPO>; });
}
} // namespace

TEST(exec_with_awaitable_senders) {
    test_get_completion_domain_template<true, void>();
    test_get_completion_domain_template<true, test_std::set_error_t>();
    test_get_completion_domain_template<true, test_std::set_stopped_t>();
    test_get_completion_domain_template<true, test_std::set_value_t>();
    //-dk:TODO test compilation failure: test_get_completion_domain_template<false, int>();
}
