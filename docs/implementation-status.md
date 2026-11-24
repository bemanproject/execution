<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->
# Implementation Status

Meaning of the status indicators (in order best to worst):

- ❎: reviewed and not applicable (e.g. because a section doesn't provide something which would be reflected in code)
- ✅: reviewed and OK
- 🔴: reviewed and found incomplete
- 🚧: no or incomplete review

The indicators come in groups of threes:
1. The item is implemented
2. The item is tested
3. The item is documented (for a somewhat loose definition of "documented": something is said in the [overview](https://github.com/bemanproject/execution/blob/main/docs/overview.md))

Each section containing subelements reflects the state of the "worst" element.

# 🔴🔴🔴 [exec](https://wg21.link/exec) Execution control library
## ✅✅✅ [exec.general](https://wg21.link/exec.general) General

- ❎❎❎ [<code><i>MANDATE-NOTHROW</i>(expr)</code>](https://wg21.link/exec.general#5) ⇒ <code>noexcept(<i>expr</i>)</code> is `true`
- ✅✅✅ [<code><i>movable-value</i>&lt;T&gt;</code>](https://wg21.link/exec.general#6): [`movable_value.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/movable_value.hpp)
- ✅✅✅ [<code><i>MATCHING-SIG</i>&lt;F1, F2&gt;</code>](https://wg21.link/exec.general#7): [`matching_sig.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/matching_sig.hpp)
- ✅✅✅ [<code><i>AS-EXCEPT-PTR</i>(error)</code>](https://wg21.link/exec.general#8): [`as_except_ptr.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/as_except_ptr.hpp)
- ❎❎❎ [<code><i>as-const</i>(error)</code>](https://wg21.link/exec.general#9): <code>[std::as_const](https://wg21.link/utility.as.const)(error)</code>

## ✅✅✅ [exec.queryable](https://wg21.link/exec.queryable) Query and queryables
### ❎❎❎ [exec.queryable.general](https://wg21.link/exec.queryable.general) General
### ✅✅✅ [exec.queryable.concept](https://wg21.link/exec.queryable.concept) Queryable concept

- ✅✅✅ [<code><i>queryable</i>&lt;Q&gt;</code>](https://wg21.link/exec.queryable.concept#1): [`queryable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/queryable.hpp)

## ❎❎❎ [exec.async.ops](https://wg21.link/exec.async.ops) Asynchronous operations
## ✅✅🔴 [execution.syn](https://wg21.link/execution.syn) Header `<execution>` synopsis

- ✅✅🔴 [<code>is_execution_policy&lt;T&gt;</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- ✅✅🔴 [<code>is_execution_policy_v&lt;T&gt;</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- ✅✅🔴 [<code>sequenced_policy&lt;T&gt;</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- ✅✅🔴 [<code>parallel_policy&lt;T&gt;</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- ✅✅🔴 [<code>parallel_unsequenced_policy&lt;T&gt;</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- ✅✅🔴 [<code>unsequenced_policy&lt;T&gt;</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- ✅✅🔴 [<code>seq</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- ✅✅🔴 [<code>par</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- ✅✅🔴 [<code>par_unseq</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- ✅✅🔴 [<code>unseq</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- ✅✅✅ [<code><i>movable-value</i>&lt;T&gt;</code>](https://wg21.link/exec.general#6): [`movable_value.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/movable_value.hpp)
- ✅✅✅ [<code><i>decays-to</i></code>](https://wg21.link/execution.syn): [`decays_to.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/decays_to.hpp)
- ✅✅✅ [<code><i>class-type</i></code>](https://wg21.link/execution.syn): [`class_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/class_type.hpp)
- ✅✅✅ [<code><i>queryable</i>&lt;Q&gt;</code>](https://wg21.link/exec.queryable.concept#1): [`queryable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/queryable.hpp)
- ✅✅✅ [<code><i>single-sender-value_type</i>&lt;Sndr, Env...&gt;</code>](https://wg21.link/exec.queryable.concept#1): [`single_sender_value_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/single_sender_value_type.hpp)
- ✅✅✅ [<code><i>single-sender</i>&lt;Sndr, Env...&gt;</code>](https://wg21.link/exec.queryable.concept#1): [`single_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/single_sender.hpp)

## ✅✅✅ [exec.queries](https://wg21.link/exec.queries) Queries
### ✅✅✅ [exec.queries.expos](https://wg21.link/exec.queries.expos) Query utilities

- ✅✅✅ [<code><i>TRY-QUERY</i>(q, tag, a...)</code>](https://wg21.link/exec.queries.expos#2): [`try_query.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/try_query.hpp)
- ✅✅✅ [<code><i>HIDE-SCHED</i>(q, tag, a...)</code>](https://wg21.link/exec.queries.expos#3): [`hide_sched.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/hide_sched.hpp)

### ✅✅✅ [exec.fwd.env](https://wg21.link/exec.fwd.env) `forwarding_query`: [`forwarding_query.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/forwarding_query.hpp)
### ✅✅✅ [exec.get.allocator](https://wg21.link/exec.get.allocator) `get_allocator`: [`get_allocator.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_allocator.hpp)
### ✅✅✅ [exec.get.stop.token](https://wg21.link/exec.get.stop.token) `get_stop_token`: [`get_stop_token.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_stop_token.hpp)
### ✅✅✅ [exec.get.env](https://wg21.link/exec.get.env) `execution::get_env`: [`get_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_env.hpp)
### ✅✅✅ [exec.get.domain](https://wg21.link/exec.get.domain) `execution::get_domain`: [`get_domain.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_domain.hpp)
### ✅✅✅ [exec.get.scheduler](https://wg21.link/exec.get.scheduler) `execution::get_scheduler`: [`get_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_scheduler.hpp)
### ✅✅✅ [exec.get.start.scheduler](https://wg21.link/exec.get.start.scheduler) `execution::get_start_scheduler`: [`get_start_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_start_scheduler.hpp)
### ✅✅✅ [exec.get.delegation.scheduler](https://wg21.link/exec.get.delegation.scheduler) `execution::get_delegation_scheduler`: [`get_delegation_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_delegation_scheduler.hpp)
### ✅✅✅ [exec.get.fwd.progress](https://wg21.link/exec.get.fwd.progress) `execution::get_forward_progress_guarantee`

- ✅✅✅ [`forward_progress_guarantee`](https://wg21.link/exec.get.fwd.progress): [`get_forward_progress_guarantee.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_forward_progress_guarantee.hpp)
- ✅✅✅ [`get_forward_progress_guarantee`](https://wg21.link/exec.get.fwd.progress#2): [`get_forward_progress_guarantee.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_forward_progress_guarantee.hpp)

### ✅✅✅ [exec.get.compl.sched](https://wg21.link/exec.get.compl.sched) `execution::get_completion_scheduler`: [`get_completion_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_completion_scheduler.hpp)
### ✅✅✅ [exec.get.compl.domain](https://wg21.link/exec.get.compl.domain) `execution::get_completion_domain`: [`get_completion_domain.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_completion_domain.hpp)

### ✅✅✅ [exec.get.await.adapt](https://wg21.link/exec.get.await.adapt) `execution::get_await_completion_adaptor`: [`get_await_completion_adaptor.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_await_completion_adaptor.hpp)
## ✅✅✅ [exec.sched](https://wg21.link/exec.sched) Schedulers

- ✅✅✅ [`scheduler`](https://wg21.link/exec.sched#1): [`scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/scheduler.hpp)
- ✅✅✅ [<code><i>infallible-scheduler</i></code>](https://wg21.link/exec.sched#8): [`infallible_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/infallible_scheduler.hpp)

## ✅✅✅ [exec.recv](https://wg21.link/exec.recv) Receivers
### ✅✅✅ [exec.recv.concepts](https://wg21.link/exec.recv.concepts) Receiver concepts

- ✅✅✅ [`receiver`](https://wg21.link/exec.recv.concepts#1): [`receiver.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/receiver.hpp)
- ✅✅✅ [<code><i>valid-completion-for</i></code>](https://wg21.link/exec.recv.concepts#1): [`valid_completion_for.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/valid_completion_for.hpp)
- ✅✅✅ [<code><i>has-completion</i></code>](https://wg21.link/exec.recv.concepts#1): [`has_completions.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/has_completions.hpp)
- ✅✅✅ [<code><i>receiver-of</i></code>](https://wg21.link/exec.recv.concepts#1): [`receiver_of.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/receiver_of.hpp)
- ✅✅✅ [<code>inlinable_receiver</code>](https://wg21.link/exec.recv.concepts#4)

### ✅✅✅ [exec.set.value](https://wg21.link/exec.set.value) `execution::set_value`: [`set_value.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_value.hpp)
### ✅✅✅ [exec.set.error](https://wg21.link/exec.set.error) `execution::set_error`: [`set_error.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_error.hpp)
### ✅✅✅ [exec.set.stopped](https://wg21.link/exec.set.stopped) `execution::set_stopped`: [`set_stopped.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_stopped.hpp)
## ✅✅✅ [exec.opstate](https://wg21.link/exec.opstate) Operation states
### ✅✅✅ [exec.opstate.general](https://wg21.link/exec.opstate.general) General: [`operation_state.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/operation_state.hpp)
### ✅✅✅ [exec.opstate.start](https://wg21.link/exec.opstate.start) `execution::start`: [`start.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/start.hpp)
## 🔴🔴🔴 [exec.snd](https://wg21.link/exec.snd) Senders
### ❎❎❎ [exec.snd.general](https://wg21.link/exec.snd.general) General
### 🔴🔴🔴 [exec.snd.expos](https://wg21.link/exec.snd.expos) Exposition-only entities

- ✅✅✅ [exec.snd.expos p4](https://wg21.link/exec.snd.expos#4) <code><i>FWD-ENV</i>(env)</code>: [`fwd_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/fwd_env.hpp)
- ✅✅✅ [exec.snd.expos p4](https://wg21.link/exec.snd.expos#4) <code><i>FWD-ENV-T</i>(Env)</code>: [`fwd_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/fwd_env.hpp)
- ✅✅✅ [exec.snd.expos p5](https://wg21.link/exec.snd.expos#5) <code><i>MAKE-ENV</i>(q, v)</code>: [`make_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/make_env.hpp)
- ✅✅✅ [exec.snd.expos p6](https://wg21.link/exec.snd.expos#6) <code><i>JOIN-ENV</i>(ev1, ev2)</code>: [`join_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/join_env.hpp)
- ✅🔴✅ [exec.snd.expos p8](https://wg21.link/exec.snd.expos#8) <code><i>COMMON-DOMAIN</i>(domains...)</code>: [`common_domain.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/common_domain.hpp)
- ✅✅✅ [exec.snd.expos p9](https://wg21.link/exec.snd.expos#9) <code><i>COMPL-DOMAIN</i>(sndr, ev...)</code>: [`compl_domain.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/compl_domain.hpp) 🚨 the name appears to be unused!
- ✅✅✅ [exec.snd.expos p10](https://wg21.link/exec.snd.expos#10) <code><i>SCHED-ENV</i>(sch)</code>: [`sched_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sched_env.hpp)
- 🚧🚧🚧 [exec.snd.expos p11](https://wg21.link/exec.snd.expos#11) <code><i>SET-VALUE</i>(rcvr, expr)</code>: [`set_value.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_value.hpp), <code><i>TRY-EVAL</i>(expr)</code>: [`common.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/common.hpp), <code><i>TRY-SET-VALUE</i>(rcvr, expr)</code>
- ✅✅✅ [exec.snd.expos p12](https://wg21.link/exec.snd.expos#12) <code><i>query-with-default</i>(tag, env, value)</code>: [`query_with_default.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/query_with_default.hpp)
- ✅🔴🔴 [exec.snd.expos p15](https://wg21.link/exec.snd.expos#15) <code><i>emplace-from</i>{fun}</code>: [`emplace_from.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/emplace_from.hpp)
- ✅🔴🔴 [exec.snd.expos p12](https://wg21.link/exec.snd.expos#16) <code><i>on-stop-request</i>{stop_source}</code>: [`on_stop_request.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/on_stop_request.hpp)
- ✅✅✅ [exec.snd.expos p17](https://wg21.link/exec.snd.expos#17) <code><i>product-type</i>{members...}</code>: [`product_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/product_type.hpp)
- ✅✅✅ [exec.snd.expos p23](https://wg21.link/exec.snd.expos#23) <code><i>valid-specialization</i>&lt;T, Args...&gt;</code>: [`valid_specialization.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/valid_specialization.hpp)
- ✅✅✅ [exec.snd.expos p24](https://wg21.link/exec.snd.expos#24) <code><i>make-sender</i>(tag, data = <i>empty</i>{}, child...)</code>: [`make_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/make_sender.hpp)
- ✅✅🔴 [exec.snd.expos p27](https://wg21.link/exec.snd.expos#27)

    - ✅✅🔴 <code><i>state-type</i>&lt;Sndr, Rcvr&lt;</code>: [`state_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/state_type.hpp)
    - ✅✅🔴 <code><i>env-type</i>&lt;Index, Sndr, Rcvr&lt;</code>: [`env_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/env_type.hpp)
    - ✅✅🔴 <code><i>data-type</i>&lt;Sndr&gt;</code>: [`data_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/data_type.hpp)
    - ✅✅🔴 <code><i>child-type</i>&lt;Sndr, I = 0&gt;</code>: [`child_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/child_type.hpp)
    - ✅✅🔴 <code><i>indices-for</i>&lt;Sndr&gt;</code>: [`indices_for.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/indices_for.hpp)
    - ✅✅🔴 <code><i>basic-state</i>&lt;Sndr&gt;</code>: [`basic_state.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/basic_state.hpp)

- ✅✅🔴 [exec.snd.expos p29](https://wg21.link/exec.snd.expos#29) <code><i>basic-receiver</i>&lt;Sndr, Rcvr, Index&gt;</code>: [`basic_receiver.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/basic_receiver.hpp)
- ✅✅🔴 [exec.snd.expos p30](https://wg21.link/exec.snd.expos#30) <code><i>connect-all</i>{&op, sndr, indices}</code>: [`connect_all.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/connect_all.hpp)
- ✅✅🔴 [exec.snd.expos p33](https://wg21.link/exec.snd.expos#33) <code><i>basic-operation</i>&lt;Sndr, Rcvr&gt;</code>: [`basic_operation.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/basic_operation.hpp)
- ✅✅🔴 [exec.snd.expos p35](https://wg21.link/exec.snd.expos#35) <code><i>default-impls</i></code>: [`default_impls.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/default_impls.hpp) 🚨 verify <code><i>check-types</i></code> and <code><i>impls-for</i></code> members
- ✅✅🔴 [exec.snd.expos p43](https://wg21.link/exec.snd.expos#43) <code><i>basic-sender</i>&lt;Tag, Data, Child...&gt;</code>: [`basic_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/basic_sender.hpp)
- 🔴🔴🔴 [exec.snd.expos p50](https://wg21.link/exec.snd.expos#50) <code><i>overload-set</i>&lt;Funs...&gt;</code>
- 🔴🔴🔴 [exec.snd.expos p51](https://wg21.link/exec.snd.expos#51) <code><i>not-a-sender</i></code>
- 🔴🔴🔴 [exec.snd.expos p52](https://wg21.link/exec.snd.expos#52) <code><i>not-a-scheduler</i></code>
- 🔴🔴🔴 [exec.snd.expos p53](https://wg21.link/exec.snd.expos#53) <code><i>decay-copyable-result-datums</i></code>
- 🔴🔴🔴 [exec.snd.expos p54](https://wg21.link/exec.snd.expos#54) <code><i>allocator-aware-forward(obj, env)</i></code>
- ✅🔴🔴 [exec.snd.expos p56](https://wg21.link/exec.snd.expos#56) <code><i>call-with-default(fn, value, args...)</i></code>: [`call_with_default.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/call_with_default.hpp)
- ✅✅🔴 [exec.snd.expos p59](https://wg21.link/exec.snd.expos#59) <code><i>inline-attrs</i>&lt;T&gt;</code>: [`inline_attrs.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/inline_attrs.hpp)

### 🔴🔴🔴 [exec.snd.concepts](https://wg21.link/exec.snd.concepts) Sender concepts

- ✅🔴🔴 [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code><i>is-constant</i>&lt;Value&gt;</code>: [`is_constant.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/is_constant.hpp)
- ✅✅🔴 [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code><i>is-sender</i>&lt;Value&gt;</code>: [`is_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/is_sender.hpp)
- ✅✅🔴 [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code><i>enable-sender</i>&lt;Value&gt;</code>: [`enable_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/enable_sender.hpp)
- 🔴🔴🔴 [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code><i>is-dependent-sender-helper</i>&lt;Sndr&gt;</code>
- ✅✅✅ [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code>sender&lt;T&gt;</code>: [`sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sender.hpp)
- ✅✅✅ [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code>sender_in&lt;T, Env...&gt;</code>: [`sender_in.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sender_in.hpp)
- ✅✅✅ [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code>dependent_sender&lt;T, Env...&gt;</code>: [`sender_in.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sender_in.hpp)
- ✅✅✅ [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code>sender_to&lt;Sndr, Rcvr&gt;</code>: [`sender_to.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sender_to.hpp)
- 🔴🔴🔴 [exec.snd.concepts p5](https://wg21.link/exec.snd.concepts#5) <code><i>value-signature</i>&lt;As...&gt;</code>
- 🔴🔴🔴 [exec.snd.concepts p5](https://wg21.link/exec.snd.concepts#5) <code><i>sender-in-of-impl</i>&lt;Sndr, SetValue, Env...&gt;</code>
- 🔴🔴🔴 [exec.snd.concepts p5](https://wg21.link/exec.snd.concepts#5) <code><i>sender-in-of</i>&lt;Sndr, Env, Values...&gt;</code>
- 🔴🔴🔴 [exec.snd.concepts p5](https://wg21.link/exec.snd.concepts#5) <code><i>sender-of</i>&lt;Sndr, Values...&gt;</code>
- ✅✅✅ [exec.snd.concepts p6](https://wg21.link/exec.snd.concepts#6) <code>tag_of_t&lt;Sndr&gt;</code>: [`tag_of_t.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/tag_of_t.hpp)
- ✅✅✅ [exec.snd.concepts p7](https://wg21.link/exec.snd.concepts#7) <code><i>sender-for</i>&lt;Sndr, tag&gt;</code>: [`sender_for.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sender_for.hpp)
- 🔴🔴🔴 [exec.snd.concepts p8](https://wg21.link/exec.snd.concepts#8) <code><i>SET-VALUE-SIG(T)</i></code>

### ✅✅🔴 [exec.awaitable](https://wg21.link/exec.awaitable) Awaitable helpers

- ✅✅🔴 [exec.awaitable p2](https://wg21.link/exec.awitable#2) <code><i>GET-AWAITER(c, p)</i></code>: [`get_awaiter.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_awaiter.hpp)
- ✅✅🔴 [exec.awaitable p3](https://wg21.link/exec.awitable#3) <code><i>await-suspend-result</i>&lt;T&gt;</code>: [`await_suspend_result.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/await_suspend_result.hpp)
- ✅✅🔴 [exec.awaitable p3](https://wg21.link/exec.awitable#3) <code><i>is-awaiter</i>&lt;A, Promise...&gt;</code>: [`await_suspend_result.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/await_suspend_result.hpp)
- ✅✅🔴 [exec.awaitable p3](https://wg21.link/exec.awitable#3) <code><i>is-awaitable</i>&lt;C, Promise...&gt;</code>: [`is_awaitable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/is_awaitable.hpp)
- ✅✅🔴 [exec.awaitable p5](https://wg21.link/exec.awitable#5) <code><i>has-as-awaitable</i>&lt;T, Promise&gt;</code>: [`has_as_awaitable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/has_as_awaitable.hpp)
- ✅✅🔴 [exec.awaitable p5](https://wg21.link/exec.awitable#5) <code><i>with-await-transform</i>&lt;Derived&gt;</code>: [`has_as_awaitable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/has_as_awaitable.hpp)
- ✅✅🔴 [exec.awaitable p6](https://wg21.link/exec.awitable#6) <code><i>env-promise</i>&lt;Env&gt;</code>: [`env_promise.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/env_promise.hpp)

### ✅✅🔴 [exec.domain.indeterminate](https://wg21.link/exec.domain.indeterminate) `execution::indeterminate_domain`: [`indeterminate_domain.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/indeterminate_domain.hpp)
### ✅✅🔴 [exec.domain.default](https://wg21.link/exec.domain.default) `execution::default_domain`: [`default_domain.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/default_domain.hpp)
### ✅✅🔴 [exec.snd.transform](https://wg21.link/exec.snd.transform) `exuection::transform_sender`: [`transform_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/transform_sender.hpp)
### ✅✅🔴 [exec.snd.apply](https://wg21.link/exec.snd.apply) `execution::apply_sender`: [`apply_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/apply_sender.hpp)
### ✅✅✅ [exec.getcomplsigs](https://wg21.link/exec.getcomplsigs) `execution::get_completion_signatures`: [`get_completion_signatures.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_completion_signatures.hpp)
### 🔴🔴🔴 [exec.connect](https://wg21.link/exec.connect) `execution::connect`

- ✅✅✅ [exec.connect p2](https://wg21.link/exec.connect#2) <code>connect(sndr, rcvr)</code>: [`connect.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/connect.hpp)
- ✅✅✅ [exec.connect p3](https://wg21.link/exec.connect#3) <code><i>connect-awaitable-promise</i></code>: [`operation_state_task.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/operation_state_task.hpp)
- ✅✅✅ [exec.connect p4](https://wg21.link/exec.connect#4) <code><i>operation-state-task</i></code>: [`operation_state_task.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/operation_state_task.hpp)
- ✅✅🔴 [exec.connect p5](https://wg21.link/exec.connect#5) <code><i>await-result-type</i>&lt;DS, Promise&gt;</code>: [`await_result_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/await_result_type.hpp)
- ✅✅🔴 [exec.connect p5](https://wg21.link/exec.connect#5) <code><i>suspend-complete</i>(fun, as...)</code>: [`suspend_complete.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/suspend_complete.hpp)
- ✅✅🔴 [exec.connect p5](https://wg21.link/exec.connect#5) <code><i>connect-awaitable</i>(sndr, rcvr)</code>: [`connect_awaitable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/connect_awaitable.hpp)

## ✅✅✅ [exec.factories](https://wg21.link/exec.factories) Sender factories
### ✅✅✅ [exec.schedule](https://wg21.link/exec.schedule) `execution::schedule`: [`schedule.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/schedule.hpp)
### ✅✅✅ [exec.just](https://wg21.link/exec.just) `execution::just`, `exuection::just_error`, `execution::just_stopped`: [`just.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/just.hpp)
### ✅✅✅ [exec.read.env](https://wg21.link/exec.read.env) `execution::read_env`: [`read_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/read_env.hpp)
## ✅🔴🔴 [exec.adapt](https://wg21.link/exec.adapt) Sender adaptors
### ✅🔴🔴 [exec.adapt.general](https://wg21.link/exec.adapt.general) General

- 🚧 [exec.adapt.general p3.2](https://wg21.link/exec.adapt.general#3.2) verify single child env
- 🚧 [exec.adapt.general p3.3](https://wg21.link/exec.adapt.general#3.3) verify multi child env
- 🚧 [exec.adapt.general p3.4](https://wg21.link/exec.adapt.general#3.4) verify child's receiver's env
- 🚧 [exec.adapt.general p3.5](https://wg21.link/exec.adapt.general#3.5) sender non-dependent

### ✅✅🔴 [exec.adapt.obj](https://wg21.link/exec.adapt.obj) Closure objects

- 🚧 [exec.adapt.obj p1](https://wg21.link/exec.adapt.general#1) verify senders are pipeable
- 🚧 [exec.adapt.obj p1](https://wg21.link/exec.adapt.general#1) verify pipeable sender adaptor closure objects compose

### ✅✅✅ [exec.write.env](https://wg21.link/exec.write.env) `execution::write_env`: [`write_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/write_env.hpp)
### ✅✅✅ [exec.unstoppable](https://wg21.link/exec.unstoppable) `execution::unstoppable`: [`unstoppable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/unstoppable.hpp)
### ✅✅✅ [exec.starts.on](https://wg21.link/exec.starts.on) `execution::starts_on`: [`starts_on.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/starts_on.hpp)
### ✅✅✅ [exec.continues.on](https://wg21.link/exec.continues.on) `execution::continues_on`: [`continues_one.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/continues_one.hpp)
### ✅✅✅ [exec.schedule.from](https://wg21.link/exec.schedule.from) `execution::schedule_from`: [`schedule_from.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/schedule_from.hpp)
### ✅✅✅ [exec.on](https://wg21.link/exec.on) `execution::on`: [`on.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/on.hpp)
### ✅✅✅ [exec.then](https://wg21.link/exec.then) `execution::then`, `execution::upon_error`, `execution::upon_stopped`: [`then.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/then.hpp)

- 🔴 [exec.then p5](https://wg21.link/exec.then#5) verify <code><i>check-types</i></code>

### ✅✅✅ [exec.let](https://wg21.link/exec.let) `execution::let_value`, `execution::let_error`, `execution::let_stopped`: [`let.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/let.hpp)

- 🔴 [exec.let p9](https://wg21.link/exec.let#9) verify <code><i>check-types</i></code>

### ✅✅🔴 [exec.bulk](https://wg21.link/exec.bulk) `execution::bulk`, `execution::bulk_chunked`, and `execution::bulk_unchunked`: [`bulk.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/bulk.hpp)

- 🔴 [exec.bulk p6](https://wg21.link/exec.let#6) verify <code><i>check-types</i></code>
- 🔴 [exec.bulk p8](https://wg21.link/exec.let#8) verify <code><i>check-types</i></code>

### ✅✅✅ [exec.when.all](https://wg21.link/exec.when.all) `execution::when_all`: [`when_all.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/when_all.hpp)

- 🔴 [exec.adapt.general p3.2](https://wg21.link/exec.adapt.general#3.2) verify single child env
- 🔴 [exec.when.all p9](https://wg21.link/exec.when.all#9) check use of <code><i>count-of</i></code>

### ✅✅✅ [exec.into.variant](https://wg21.link/exec.into.variant) `execution::into_variant`: [`into_variant.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/into_variant.hpp)
### ✅✅🔴 [exec.stopped.opt](https://wg21.link/exec.stopped.opt) `execution::stopped_as_optional`: [`stopped_as_optional.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/stopped_as_optional.hpp)
### ✅✅🔴 [exec.stopped.err](https://wg21.link/exec.stopped.err) `execution::stopped_as_error`: [`stopped_as_error.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/stopped_as_error.hpp)

- 🚧 [exec.adapt.general p3.2](https://wg21.link/exec.adapt.general#3.2) verify single child env

### ✅✅🔴 [exec.associate](https://wg21.link/exec.associate) `execution::associate`: [`associate.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/associate.hpp)

- 🚧 [exec.adapt.general p3.2](https://wg21.link/exec.adapt.general#3.2) verify single child env

### ✅✅🔴 [exec.stop.when](https://wg21.link/exec.stop.when) Exposition-only <code><i>execution::stop_when</i></code>: [`stop_when.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/stop_when.hpp)

- 🚧 [exec.adapt.general p3.2](https://wg21.link/exec.adapt.general#3.2) verify single child env

### ✅✅✅ [exec.spawn.future](https://wg21.link/exec.spawn.future) `execution::spawn_future`: [`spawn_future.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/spawn_future.hpp)

- 🚧 [exec.adapt.general p3.2](https://wg21.link/exec.adapt.general#3.2) verify single child env

## ✅✅🔴 [exec.consumers](https://wg21.link/exec.consumers) Sender consumers
### ✅✅🔴 [exec.sync.wait](https://wg21.link/exec.sync.wait) `this_thread::sync_wait`: [`sync_wait.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sync_wait.hpp)
### ✅✅✅ [exec.sync.wait.var](https://wg21.link/exec.sync.wait.var) `this_thread::sync_with_variant`: [`sync_wait_with_variant.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sync_wait_with_variant.hpp)
### ✅✅🔴 [exec.spawn](https://wg21.link/exec.spawn) `execution::spawn`: [`spawn.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/spawn.hpp)
## ✅✅🔴 [exec.cmplsig](https://wg21.link/exec.cmplsig) Completion signatures

- ✅✅✅ [exec.cmplsig p4](https://wg21.link/exec.cmplsig#4) <code><i>completion-signature</i></code>: [`completion_signature.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/completion_signature.hpp)
- ✅✅🔴 [exec.cmplsig p5](https://wg21.link/exec.cmplsig#5) <code><i>indirect-meta-apply</i></code>: [`indirect_meta_apply.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/indirect_meta_apply.hpp)
- ✅✅🔴 [exec.cmplsig p5](https://wg21.link/exec.cmplsig#5) <code><i>always-true</i></code>: [`gather_signatures.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/gather_signatures.hpp)
- ❎❎🔴 [exec.cmplsig p6](https://wg21.link/exec.cmplsig#6) <code><i>META-APPLY</i></code>: [`gather_signatures.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/gather_signatures.hpp)
- ✅✅✅ [exec.cmplsig p6](https://wg21.link/exec.cmplsig#6) <code><i>gather-signatures</i></code>: [`gather_signatures.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/gather_signatures.hpp)
- ✅🔴✅ [exec.cmplsig p8](https://wg21.link/exec.cmplsig#8) <code>completion_signatures</code>: [`completion_signatures.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/completion_signatures.hpp)
- ✅✅✅ [exec.cmplsig p8](https://wg21.link/exec.cmplsig#8) <code>value_types_of_t</code>: [`value_types_of_t.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/value_types_of_t.hpp)
- ✅✅✅ [exec.cmplsig p8](https://wg21.link/exec.cmplsig#8) <code>error_types_of_t</code>: [`error_types_of_t.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/error_types_of_t.hpp)
- ✅✅✅ [exec.cmplsig p8](https://wg21.link/exec.cmplsig#8) <code>sends_stopped</code>: [`sends_stopped.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sends_stopped.hpp)

## ✅✅✅ [exec.envs](https://wg21.link/exec.envs) Queryable utilities
### ✅✅✅ [exec.prop](https://wg21.link/exec.prop) Class template `prop`: [`prop.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/prop.hpp)
### ✅✅✅ [exec.env](https://wg21.link/exec.env) Class template `env`: [`env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/env.hpp)
## ✅✅✅ [exec.ctx](https://wg21.link/exec.ctx) Execution contexts
### ✅✅✅ [exec.run.loop](https://wg21.link/exec.run.loop) `execution::run_loop`
#### ✅✅✅ [exec.run.loop.general](https://wg21.link/exec.run.loop.general) General: [`run_loop.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/run_loop.hpp)
#### ✅✅❎ [exec.run.loop.types](https://wg21.link/exec.run.loop.types) Associated types
#### ✅✅❎ [exec.run.loop.ctor](https://wg21.link/exec.run.loop.ctor) Constructor and destructor
#### ✅✅❎ [exec.run.loop.members](https://wg21.link/exec.run.loop.members) Member functions
## ✅✅🔴 [exec.coro.util](https://wg21.link/exec.coro.util) Coroutine utilities
### ✅✅🔴 [exec.as.awaitable](https://wg21.link/exec.as.awaitable) `execution::as_awaitable`

- ✅🔴✅ [exec.as.awaitable p1](https://wg21.link/exec.as.awaitable#1) <code><i>awaitable-sender</i></code>: [`awaitable_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/awaitable_sender.hpp)
- 🔴🔴🔴 [exec.as.awaitable p1](https://wg21.link/exec.as.awaitable#1) <code><i>has-queryable-await-completion-adaptor</i></code>: 🚨 this name appears to be is unused
- ✅🔴✅ [exec.as.awaitable p2](https://wg21.link/exec.as.awaitable#2) <code><i>sender-awaitable</i></code>: : [`sender_awaitable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sender_awaitable.hpp)
- ✅✅✅ [exec.as.awaitable p7](https://wg21.link/exec.as.awaitable#7) <code>as_awaitable</code>: : [`as_awaitable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/as_awaitable.hpp)
- ✅🔴✅ [exec.as.awaitable p8](https://wg21.link/exec.as.awaitable#8) <code><i>adapt-for-await-completion</i></code>: : [`as_awaitable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/as_awaitable.hpp)

### ✅✅🔴 [exec.with.awaitable.senders](https://wg21.link/exec.with.awaitable.senders) `execution::with_awaitable_senders`: [`with_awaitable_senders.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/with_awaitable_senders.hpp)
### ✅✅✅ [exec.affine](https://wg21.link/exec.affine) `execution::affine`: [`affine.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/affine.hpp)
### ✅✅🔴 [exec.inline.scheduler](https://wg21.link/exec.inline.scheduler) `execution::inline_scheduler`: [`inline_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/inline_scheduler.hpp)
### ✅✅🔴 [exec.task.scheduler](https://wg21.link/exec.task.scheduler) `execution::task_scheduler`: [`task_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/task_scheduler.hpp)
### ✅✅🔴 [exec.task](https://wg21.link/exec.task) `execution::task`
#### ✅✅🔴 [task.overview](https://wg21.link/task.overview) `task` overview
#### ✅✅🔴 [task.class](https://wg21.link/task.class) Class template `task`
#### ✅✅🔴 [task.members](https://wg21.link/task.members) `task` members
#### ✅✅🔴 [task.state](https://wg21.link/task.state) Class template `task::state`
#### ✅✅🔴 [task.promise](https://wg21.link/task.promise) Class template `task::promise_type`
## 🔴🔴🔴 [exec.scope](https://wg21.link/exec.scope) Execution scope utilities
### ✅🔴🔴 [exec.scope.concepts](https://wg21.link/exec.scope.concepts) Execution scope concepts

- ✅🔴🔴 [exec.scope.concepts p1](https://wg21.link/exec.scope.concepts#1) `scope_association`: [`scope_association.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/scope_association.hpp)
- ✅✅🔴 [exec.scope.concepts p3](https://wg21.link/exec.scope.concepts#3) `scope_token`: [`scope_token.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/scope_token.hpp)

### 🔴🔴🔴 [exec.counting.scopes](https://wg21.link/exec.counting.scopes) Counting scopes
#### 🔴🔴🔴 [exec.counting.scopes.general](https://wg21.link/exec.counting.scopes.general) General

- ✅✅✅ [exec.counting.scopes.general p3](https://wg21.link/exec.counting.scopes.general#3) <code><i>scope-join-t</i></code>: [`counting_scope_join.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/counting_scope_join.hpp)
- ✅✅✅ [exec.counting.scopes.general p3](https://wg21.link/exec.counting.scopes.general#3) <code><i>scope-state-type</i></code>: [`counting_scope_base.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/counting_scope_base.hpp)
- 🔴🔴🔴 [exec.counting.scopes.general p5](https://wg21.link/exec.counting.scopes.general#5) <code><i>association-t</i></code>

#### ✅✅🔴 [exec.scope.simple.counting](https://wg21.link/exec.scope.simple.counting) Simple Counting Scope
##### ✅✅🔴 [exec.scope.simple.counting.general](https://wg21.link/exec.scope.simple.counting.general) `simple_counting_scope`: [`simple_counting_scope.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/simple_counting_scope.hpp)
##### ✅✅🔴 [exec.simple.counting.ctor](https://wg21.link/exec.simple.counting.ctor) Constructor and Destructor
##### ✅✅🔴 [exec.simple.counting.mem](https://wg21.link/exec.simple.counting.mem) Members
##### ✅✅🔴 [exec.simple.counting.token](https://wg21.link/exec.simple.counting.token) Token
#### ✅✅🔴 [exec.scope.counting](https://wg21.link/exec.scope.counting) Counting Scope
## ✅✅🔴 [exec.par.scheduler](https://wg21.link/exec.par.scheduler) Parallel scheduler: [`parallel_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/parallel_scheduler.hpp)
## ✅✅🔴 [exec.parschedrepl](https://wg21.link/exec.parschedrepl) Namespace `parallel_scheduler_replacement`
### ❎❎❎ [exec.parschedrepl.general](https://wg21.link/exec.parschedrepl.general) General
### ✅✅🔴 [exec.parschedrepl.recvproxy](https://wg21.link/exec.parschedrepl.recvproxy) Receiver proxies: [`parallel_scheduler_replacement.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/parallel_scheduler_replacement.hpp)
### ✅✅🔴 [exec.parschedrepl.query](https://wg21.link/exec.parschedrepl.query) `query_parallel_scheduler_backend`: [`parallel_scheduler_replacement.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/parallel_scheduler_replacement.hpp)
### ✅✅🔴 [exec.parschedrepl.psb](https://wg21.link/exec.parschedrepl.psb) Class `parallel_scheduler_backend`: [`parallel_scheduler_replacement.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/parallel_scheduler_replacement.hpp)
