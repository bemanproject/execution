# Implementation Status

Meaning of the status indicators (in order best to worst):

- &#x274E;: reviewed and not applicable (e.g. because a section doesn't provide someting which would be reflected in code)
- &#x2705;: reviewed and OK
- &#x1F534;: reviewed and found incomplete
- &#x1F6A7;: no or incomplete review

The indicators come in groups of threes:
1. The item is implemented
2. The item is tested
3. The item is documented (for a somewhat loose definition of "documented": something is said in the [overview](https://github.com/bemanproject/execution/blob/main/docs/overview.md))

Each section containing subelements reflects the state of the "worst" element.

# &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec](https://wg21.link/exec) Execution control library
## &#x1F6A7;&#x2705;&#x2705; [exec.general](https://wg21.link/exec.general) General

- &#x1F6A7;&#x274E;&#x274E; [<code><i>MANDATE-NOTHROW</i>(expr)</code>](https://wg21.link/exec.general#5) &#x21d2; <code>noexcept(<i>expr</i>)</code> is `true`
- &#x2705;&#x2705;&#x2705; [<code><i>movable-value</i>&lt;T&gt;</code>](https://wg21.link/exec.general#6): [`movable_value.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/movable_value.hpp)
- &#x2705;&#x2705;&#x2705; [<code><i>MATCHING-SIG</i>&lt;F1, F2&gt;</code>](https://wg21.link/exec.general#7): [`matching_sig.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/matching_sig.hpp)
- &#x2705;&#x2705;&#x2705; [<code><i>AS-EXCEPT-PTR</i>(error)</code>](https://wg21.link/exec.general#8): [`as_except_ptr.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/as_except_ptr.hpp)
- &#x1F6A7;&#x274E;&#x274E; [<code><i>as-const</i>(error)</code>](https://wg21.link/exec.general#9): <code>[std::as_const](https://wg21.link/utility.as.const)(error)</code>

## &#x1F6A7;&#x2705;&#x2705; [exec.queryable](https://wg21.link/exec.queryable) Query and queryables
### &#x1F6A7;&#x274E;&#x274E; [exec.queryable.general](https://wg21.link/exec.queryable.general) General
### &#x1F6A7;&#x2705;&#x2705; [exec.queryable.concept](https://wg21.link/exec.queryable.concept) Queryable concept

- &#x1F6A7;&#x2705;&#x2705; [<code><i>queryable</i>&lt;Q&gt;</code>](https://wg21.link/exec.queryable.concept#1): [`queryable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/queryable.hpp)

## &#x274E;&#x274E;&#x274E; [exec.async.ops](https://wg21.link/exec.async.ops) Asynchronous operations
## &#x1F6A7;&#x1F6A7;&#x1F6A7; [execution.syn](https://wg21.link/execution.syn) Header `<execution>` synopsis

- &#x2705;&#x2705;&#x1F534; [<code>is_execution_policy&lt;T&gt;</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- &#x2705;&#x2705;&#x1F534; [<code>is_execution_policy_v&lt;T&gt;</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- &#x2705;&#x2705;&#x1F534; [<code>sequenced_policy&lt;T&gt;</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- &#x2705;&#x2705;&#x1F534; [<code>parallel_policy&lt;T&gt;</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- &#x2705;&#x2705;&#x1F534; [<code>parallel_unsequenced_policy&lt;T&gt;</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- &#x2705;&#x2705;&#x1F534; [<code>unsequenced_policy&lt;T&gt;</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- &#x2705;&#x2705;&#x1F534; [<code>seq</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- &#x2705;&#x2705;&#x1F534; [<code>par</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- &#x2705;&#x2705;&#x1F534; [<code>par_unseq</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- &#x2705;&#x2705;&#x1F534; [<code>unseq</code>](https://wg21.link/execution.syn): [`execution_policy.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/execution_policy.hpp)
- &#x2705;&#x2705;&#x2705; [<code><i>movable-value</i>&lt;T&gt;</code>](https://wg21.link/exec.general#6): [`movable_value.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/movable_value.hpp)
- &#x2705;&#x2705;&#x2705; [<code><i>decays-to</i></code>](https://wg21.link/execution.syn): [`decays_to.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/decays_to.hpp)
- &#x2705;&#x2705;&#x2705; [<code><i>class-type</i></code>](https://wg21.link/execution.syn): [`class_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/class_type.hpp)
- &#x2705;&#x2705;&#x2705; [<code><i>queryable</i>&lt;Q&gt;</code>](https://wg21.link/exec.queryable.concept#1): [`queryable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/queryable.hpp)

## &#x1F534;&#x1F534;&#x1F534; [exec.queries](https://wg21.link/exec.queries) Queries
### &#x2705;&#x2705;&#x2705; [exec.queries.expos](https://wg21.link/exec.queries.expos) Query utilities

- &#x2705;&#x2705;&#x2705; [<code><i>TRY-QUERY</i>(q, tag, a...)</code>](https://wg21.link/exec.queries.expos#2): [`try_query.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/try_query.hpp)
- &#x2705;&#x2705;&#x2705; [<code><i>HIDE-SCHED</i>(q, tag, a...)</code>](https://wg21.link/exec.queries.expos#3): [`hide_sched.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/hide_sched.hpp)

### &#x2705;&#x2705;&#x2705; [exec.fwd.env](https://wg21.link/exec.fwd.env) `forwarding_query`: [`forwarding_query.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/forwarding_query.hpp)
### &#x2705;&#x2705;&#x2705; [exec.get.allocator](https://wg21.link/exec.get.allocator) `get_allocator`: [`get_allocator.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_allocator.hpp)
### &#x2705;&#x2705;&#x2705; [exec.get.stop.token](https://wg21.link/exec.get.stop.token) `get_stop_token`: [`get_stop_token.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_stop_token.hpp)
### &#x2705;&#x2705;&#x2705; [exec.get.env](https://wg21.link/exec.get.env) `execution::get_env`: [`get_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_env.hpp)
### &#x2705;&#x2705;&#x2705; [exec.get.domain](https://wg21.link/exec.get.domain) `execution::get_domain`: [`get_domain.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_domain.hpp)
### &#x2705;&#x2705;&#x2705; [exec.get.scheduler](https://wg21.link/exec.get.scheduler) `execution::get_scheduler`: [`get_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_scheduler.hpp)
### &#x2705;&#x2705;&#x2705; [exec.get.start.scheduler](https://wg21.link/exec.get.start.scheduler) `execution::get_start_scheduler`: [`get_start_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_start_scheduler.hpp)
### &#x2705;&#x2705;&#x2705; [exec.get.delegation.scheduler](https://wg21.link/exec.get.delegation.scheduler) `execution::get_delegation_scheduler`: [`get_delegation_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_delegation_scheduler.hpp)
### &#x2705;&#x2705;&#x2705; [exec.get.fwd.progress](https://wg21.link/exec.get.fwd.progress) `execution::get_forward_progress_guarantee`

- &#x2705;&#x2705;&#x2705; [`forward_progress_guarantee`](https://wg21.link/exec.get.fwd.progress): [`get_forward_progress_guarantee.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_forward_progress_guarantee.hpp)
- &#x2705;&#x2705;&#x2705; [`get_forward_progress_guarantee`](https://wg21.link/exec.get.fwd.progress#2): [`get_forward_progress_guarantee.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_forward_progress_guarantee.hpp)

### &#x2705;&#x2705;&#x2705; [exec.get.compl.sched](https://wg21.link/exec.get.compl.sched) `execution::get_completion_scheduler`: [`get_completion_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_completion_scheduler.hpp)
### &#x2705;&#x2705;&#x1F534; [exec.get.compl.domain](https://wg21.link/exec.get.compl.domain) `execution::get_completion_domain`: [`get_completion_domain.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_completion_domain.hpp)
### &#x2705;&#x1F534;&#x1F534; [exec.get.await.adapt](https://wg21.link/exec.get.await.adapt) `execution::get_await_completion_adaptor`: [`get_await_completion_adaptor.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_await_completion_adaptor.hpp)
## &#x2705;&#x1F534;&#x2705; [exec.sched](https://wg21.link/exec.sched) Schedulers

- &#x2705;&#x2705;&#x2705; [`scheduler`](https://wg21.link/exec.sched#1): [`scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/scheduler.hpp)
- &#x2705;&#x1F534;&#x2705; [<code><i>infallible-scheduler</i></code>](https://wg21.link/exec.sched#8): [`infallible_scheduler.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/infallible_scheduler.hpp)

## &#x1F534;&#x1F534;&#x1F534; [exec.recv](https://wg21.link/exec.recv) Receivers
### &#x1F534;&#x1F534;&#x1F534; [exec.recv.concepts](https://wg21.link/exec.recv.concepts) Receiver concepts

- &#x2705;&#x2705;&#x2705; [`receiver`](https://wg21.link/exec.recv.concepts#1): [`receiver.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/receiver.hpp)
- &#x2705;&#x2705;&#x2705; [<code><i>valid-completion-for</i></code>](https://wg21.link/exec.recv.concepts#1): [`valid_completion_for.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/valid_completion_for.hpp)
- &#x2705;&#x2705;&#x2705; [<code><i>has-completion</i></code>](https://wg21.link/exec.recv.concepts#1): [`has_completions.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/has_completions.hpp)
- &#x2705;&#x2705;&#x2705; [<code><i>receiver-of</i></code>](https://wg21.link/exec.recv.concepts#1): [`receiver_of.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/receiver_of.hpp)
- &#x1F534;&#x1F534;&#x1F534; [<code><i>inlinable_receiver</i></code>](https://wg21.link/exec.recv.concepts#4)

### &#x2705;&#x2705;&#x2705; [exec.set.value](https://wg21.link/exec.set.value) `execution::set_value`: [`set_value.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_value.hpp)
### &#x2705;&#x2705;&#x2705; [exec.set.error](https://wg21.link/exec.set.error) `execution::set_error`: [`set_error.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_error.hpp)
### &#x2705;&#x2705;&#x2705; [exec.set.stopped](https://wg21.link/exec.set.stopped) `execution::set_stopped`: [`set_stopped.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_stopped.hpp)
## &#x2705;&#x2705;&#x2705; [exec.opstate](https://wg21.link/exec.opstate) Operation states
### &#x2705;&#x2705;&#x2705; [exec.opstate.general](https://wg21.link/exec.opstate.general) General: [`operation_state.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/operation_state.hpp)
### &#x2705;&#x2705;&#x2705; [exec.opstate.start](https://wg21.link/exec.opstate.start) `execution::start`: [`start.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/start.hpp)
## &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.snd](https://wg21.link/exec.snd) Senders
### &#x274E;&#x274E;&#x274E; [exec.snd.general](https://wg21.link/exec.snd.general) General
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.snd.expos](https://wg21.link/exec.snd.expos) Exposition-only entities

- &#x2705;&#x2705;&#x2705; [exec.snd.expos p4](https://wg21.link/exec.snd.expos#4) <code><i>FWD-ENV</i>(env)</code>: [`fwd_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/fwd_env.hpp)
- &#x2705;&#x2705;&#x2705; [exec.snd.expos p4](https://wg21.link/exec.snd.expos#4) <code><i>FWD-ENV-T</i>(Env)</code>: [`fwd_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/fwd_env.hpp)
- &#x2705;&#x2705;&#x2705; [exec.snd.expos p5](https://wg21.link/exec.snd.expos#5) <code><i>MAKE-ENV</i>(q, v)</code>: [`make_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/make_env.hpp)
- &#x2705;&#x2705;&#x2705; [exec.snd.expos p6](https://wg21.link/exec.snd.expos#6) <code><i>JOIN-ENV</i>(ev1, ev2)</code>: [`join_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/join_env.hpp)
- &#x2705;&#x1F534;&#x2705; [exec.snd.expos p8](https://wg21.link/exec.snd.expos#8) <code><i>COMMON-DOMAIN</i>(domains...)</code>: [`common_domain.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/common_domain.hpp)
- &#x2705;&#x2705;&#x2705; [exec.snd.expos p9](https://wg21.link/exec.snd.expos#9) <code><i>COMPL-DOMAIN</i>(sndr, ev...)</code>: [`compl_domain.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/compl_domain.hpp) &#x1F6A8; the name appears to be unused!
- &#x2705;&#x2705;&#x2705; [exec.snd.expos p10](https://wg21.link/exec.snd.expos#10) <code><i>SCHED-ENV</i>(sch)</code>: [`sched_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sched_env.hpp)
- &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.snd.expos p11](https://wg21.link/exec.snd.expos#11) <code><i>SET-VALUE</i>(rcvr, expr)</code>: [`set_value.hpp](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_value.hpp), <code><i>TRY-EVAL<i>(expr)</i></code>: [`common.hpp](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/common.hpp), <code><i>TRY-SET-VALUE</i>(rcvr, expr)</code>
- &#x2705;&#x2705;&#x2705; [exec.snd.expos p12](https://wg21.link/exec.snd.expos#12) <code><i>query-with-default</i>(tag, env, value)</code>: [`query_with_default.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/query_with_default.hpp)
- &#x2705;&#x1F534;&#x1F534; [exec.snd.expos p15](https://wg21.link/exec.snd.expos#15) <code><i>emplace-from</i>{fun}</code>: [`emplace_from.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/emplace_from.hpp)
- &#x2705;&#x1F534;&#x1F534; [exec.snd.expos p12](https://wg21.link/exec.snd.expos#16) <code><i>on-stop-request</i>{stop_source}</code>: [`on_stop_request.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/on_stop_request.hpp)
- &#x2705;&#x2705;&#x2705; [exec.snd.expos p17](https://wg21.link/exec.snd.expos#17) <code><i>product-type</i>{members...}</code>: [`product_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/product_type.hpp)
- &#x2705;&#x2705;&#x2705; [exec.snd.expos p23](https://wg21.link/exec.snd.expos#23) <code><i>valid-specialization</i>&lt;T, Args...&gt;</code>: [`valid_specialization.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/valid_specialization.hpp)
- &#x2705;&#x2705;&#x2705; [exec.snd.expos p24](https://wg21.link/exec.snd.expos#24) <code><i>make-sender</i>(tag, data = <i>empty</i>{}, child...)</code>: [`make_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/make_sender.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.snd.expos p27](https://wg21.link/exec.snd.expos#27) <code><i>state-type</i>&lt;Sndr, Rcvr&lt;</code>: [`state_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/state_type.hpp) <code><i>env-type</i>&lt;Index, Sndr, Rcvr&lt;</code>: [`env_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/env_type.hpp) <code><i>data-type</i>&lt;Sndr&lt;</code>: [`data_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/data_type.hpp) <code><i>child-type</i>&lt;Sndr, I = 0&lt;</code>: [`child_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/child_type.hpp) <code><i>indices-for</i>&lt;Sndr&lt;</code>: [`indices_for.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/indices_for.hpp) <code><i>basic-state</i>&lt;Sndr&lt;</code>: [`basic_state.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/basic_state.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.snd.expos p29](https://wg21.link/exec.snd.expos#29) <code><i>basic-receiver</i>&lt;Sndr, Rcvr, Index&gt;</code>: [`basic_receiver.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/basic_receiver.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.snd.expos p30](https://wg21.link/exec.snd.expos#30) <code><i>connect-all</i>{&op, sndr, indices}</code>: [`connect_all.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/connect_all.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.snd.expos p33](https://wg21.link/exec.snd.expos#33) <code><i>basic-operation</i>&lt;Sndr, Rcvr&gt;</code>: [`basic_operation.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/basic_operation.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.snd.expos p35](https://wg21.link/exec.snd.expos#35) <code><i>default-impls</i></code>: [`default_impls.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/default_impls.hpp) &#x1F6A8; verify <code><i>check-types</i></code> and <code><i>impls-for</i></code> members
- &#x2705;&#x2705;&#x1F534; [exec.snd.expos p43](https://wg21.link/exec.snd.expos#43) <code><i>basic-sender</i>&lt;Tag, Data, Child...&gt;</code>: [`basic_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/basic_sender.hpp)
- &#x1F534;&#x1F534;&#x1F534; [exec.snd.expos p50](https://wg21.link/exec.snd.expos#50) <code><i>overload-set</i>&lt;Funs...&gt;</code>
- &#x1F534;&#x1F534;&#x1F534; [exec.snd.expos p51](https://wg21.link/exec.snd.expos#51) <code><i>not-a-sender</i></code>
- &#x1F534;&#x1F534;&#x1F534; [exec.snd.expos p52](https://wg21.link/exec.snd.expos#52) <code><i>not-a-scheduler</i></code>
- &#x1F534;&#x1F534;&#x1F534; [exec.snd.expos p53](https://wg21.link/exec.snd.expos#53) <code><i>decay-copyable-result-datums</i></code>
- &#x1F534;&#x1F534;&#x1F534; [exec.snd.expos p54](https://wg21.link/exec.snd.expos#54) <code><i>allocator-aware-forward(obj, env)</i></code>
- &#x1F534;&#x1F534;&#x1F534; [exec.snd.expos p56](https://wg21.link/exec.snd.expos#56) <code><i>call-with-default(fn, value, args...)</i></code>
- &#x2705;&#x2705;&#x1F534; [exec.snd.expos p59](https://wg21.link/exec.snd.expos#59) <code><i>inline-attrs</i>&lt;T&gt;</code>: [`inline_attrs.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/inline_attrs.hpp)

### &#x1F534;&#x1F534;&#x1F534; [exec.snd.concepts](https://wg21.link/exec.snd.concepts) Sender concepts

- &#x2705;&#x1F534;&#x1F534; [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code><i>is-constant</i>&lt;Value&gt;</code>: [`is_constant.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/is_constant.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code><i>is-sender</i>&lt;Value&gt;</code>: [`is_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/is_sender.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code><i>enable-sender</i>&lt;Value&gt;</code>: [`enable_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/enable_sender.hpp)
- &#x1F534;&#x1F534;&#x1F534; [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code><i>is-dependent-sender-helper</i>&lt;Sndr&gt;</code>
- &#x2705;&#x2705;&#x2705; [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code>sender&lt;T&gt;</code>: [`sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sender.hpp)
- &#x2705;&#x2705;&#x2705; [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code>sender_in&lt;T, Env...&gt;</code>: [`sender_in.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sender_in.hpp)
- &#x2705;&#x2705;&#x2705; [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code>dependent_sender&lt;T, Env...&gt;</code>: [`sender_in.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sender_in.hpp)
- &#x2705;&#x2705;&#x2705; [exec.snd.concepts p1](https://wg21.link/exec.snd.concepts#1) <code>sender_to&lt;Sndr, Rcvr&gt;</code>: [`sender_to.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sender_to.hpp)
- &#x1F534;&#x1F534;&#x1F534; [exec.snd.concepts p5](https://wg21.link/exec.snd.concepts#5) <code><i>value-signature</i>&lt;As...&gt;</code>
- &#x1F534;&#x1F534;&#x1F534; [exec.snd.concepts p5](https://wg21.link/exec.snd.concepts#5) <code><i>sender-in-of-impl</i>&lt;Sndr, SetValue, Env...&gt;</code>
- &#x1F534;&#x1F534;&#x1F534; [exec.snd.concepts p5](https://wg21.link/exec.snd.concepts#5) <code><i>sender-in-of</i>&lt;Sndr, Env, Values...&gt;</code>
- &#x1F534;&#x1F534;&#x1F534; [exec.snd.concepts p5](https://wg21.link/exec.snd.concepts#5) <code><i>sender-of</i>&lt;Sndr, Values...&gt;</code>
- &#x2705;&#x2705;&#x2705; [exec.snd.concepts p6](https://wg21.link/exec.snd.concepts#6) <code>tag_of_t&lt;Sndr&gt;</code>: [`tag_of_t.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/tag_of_t.hpp)
- &#x2705;&#x2705;&#x2705; [exec.snd.concepts p7](https://wg21.link/exec.snd.concepts#7) <code><i>sender-for</i>&lt;Sndr, tag&gt;</code>: [`sender_for.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sender_for.hpp)
- &#x1F534;&#x1F534;&#x1F534; [exec.snd.concepts p8](https://wg21.link/exec.snd.concepts#8) <code><i>SET-VALUE-SIG(T)</i></code>

### &#x2705;&#x2705;&#x1F534; [exec.awaitable](https://wg21.link/exec.awaitable) Awaitable helpers

- &#x2705;&#x2705;&#x1F534; [exec.awaitable p2](https://wg21.link/exec.awitable#2) <code><i>GET-AWAITER(c, p)</i></code>: [`get_awaiter.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_awaiter.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.awaitable p3](https://wg21.link/exec.awitable#3) <code><i>await-suspend-result</i>&lt;T&gt;</code>: [`await_suspend_result.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/await_suspend_result.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.awaitable p3](https://wg21.link/exec.awitable#3) <code><i>is-awaiter</i>&lt;A, Promise...&gt;</code>: [`await_suspend_result.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/await_suspend_result.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.awaitable p3](https://wg21.link/exec.awitable#3) <code><i>is-awaitable</i>&lt;C, Promise...&gt;</code>: [`is_awaitable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/is_awaitable.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.awaitable p5](https://wg21.link/exec.awitable#5) <code><i>has-as-awaitable</i>&lt;T, Promise&gt;</code>: [`has_as_awaitable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/has_as_awaitable.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.awaitable p5](https://wg21.link/exec.awitable#5) <code><i>with-await-transform</i>&lt;Derived&gt;</code>: [`has_as_awaitable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/has_as_awaitable.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.awaitable p6](https://wg21.link/exec.awitable#6) <code><i>env-promise</i>&lt;Env&gt;</code>: [`env_promise.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/env_promise.hpp)

### &#x2705;&#x2705;&#x1F534; [exec.domain.indeterminate](https://wg21.link/exec.domain.indeterminate) `execution::indeterminate_domain`: [`indeterminate_domain.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/indeterminate_domain.hpp)
### &#x2705;&#x2705;&#x1F534; [exec.domain.default](https://wg21.link/exec.domain.default) `execution::default_domain`: [`default_domain.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/default_domain.hpp)
### &#x2705;&#x2705;&#x1F534; [exec.snd.transform](https://wg21.link/exec.snd.transform) `exuection::transform_sender`: [`transform_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/transform_sender.hpp)
### &#x2705;&#x2705;&#x1F534; [exec.snd.apply](https://wg21.link/exec.snd.apply) `execution::apply_sender`: [`apply_sender.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/apply_sender.hpp)
### &#x2705;&#x2705;&#x2705; [exec.getcomplsigs](https://wg21.link/exec.getcomplsigs) `execution::get_completion_signatures`: [`get_completion_signatures.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_completion_signatures.hpp)
### &#x1F534;&#x1F534;&#x1F534; [exec.connect](https://wg21.link/exec.connect) `execution::connect`

- &#x2705;&#x2705;&#x2705; [exec.connect p2](https://wg21.link/exec.connect#2) <code>connect(sndr, rcvr)<code>: [`connect.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/connect.hpp)
- &#x2705;&#x2705;&#x2705; [exec.connect p3](https://wg21.link/exec.connect#3) <code><i>connect-awaitable-promise</i><code>: [`connect_awaitable_promise.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/connect_awaitable_promise.hpp)
- &#x2705;&#x2705;&#x2705; [exec.connect p4](https://wg21.link/exec.connect#4) <code><i>operation-state-task</i><code>: [`operation_state_task.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/operation_state_task.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.connect p5](https://wg21.link/exec.connect#5) <code><i>await-result-type</i>&lt;DS, Promise&gt;<code>: [`await_result_type.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/await_result_type.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.connect p5](https://wg21.link/exec.connect#5) <code><i>suspend-complete</i>(fun, as...)<code>: [`suspend_complete.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/suspend_complete.hpp)
- &#x2705;&#x2705;&#x1F534; [exec.connect p5](https://wg21.link/exec.connect#5) <code><i>connect-awaitable</i>(sndr, rcvr)<code>: [`connect_awaitable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/connect_awaitable.hpp)

## &#x2705;&#x2705;&#x2705; [exec.factories](https://wg21.link/exec.factories) Sender factories
### &#x2705;&#x2705;&#x2705; [exec.schedule](https://wg21.link/exec.schedule) `execution::schedule`: [`schedule.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/schedule.hpp) 
### &#x2705;&#x2705;&#x2705; [exec.just](https://wg21.link/exec.just) `execution::just`, `exuection::just_error`, `execution::just_stopped`: [`just.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/just.hpp)
### &#x2705;&#x2705;&#x2705; [exec.read.env](https://wg21.link/exec.read.env) `execution::read_env`: [`read_env.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/read_env.hpp)
## &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.adapt](https://wg21.link/exec.adapt) Sender adaptors
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.adapt.general](https://wg21.link/exec.adapt.general) General
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.adapt.obj](https://wg21.link/exec.adapt.obj) Closer objects
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.write.env](https://wg21.link/exec.write.env) `execution::write_env`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.unstoppable](https://wg21.link/exec.unstoppable) `execution::unstoppable`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.starts.on](https://wg21.link/exec.starts.on) `execution::starts_on`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.continues.on](https://wg21.link/exec.continues.on) `execution::continues_on`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.schedule.from](https://wg21.link/exec.schedule.from) `execution::schedule_from`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.on](https://wg21.link/exec.on) `execution::on`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.then](https://wg21.link/exec.then) `execution::then`, `execution::upon_error`, `execution::upon_stopped`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.let](https://wg21.link/exec.let) `execution::let_value`, `execution::let_error`, `execution::let_stopped`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.bulk](https://wg21.link/exec.bulk) `execution::bulk`, `execution::bulk_chunked`, and `execution::bulk_unchunked`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.when.all](https://wg21.link/exec.when.all) `execution::when_all`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.into.variant](https://wg21.link/exec.into.variant) `execution::into_variant`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.stopped.opt](https://wg21.link/exec.stopped.opt) `execution::stopped_as_optional`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.stopped.err](https://wg21.link/exec.stopped.err) `execution::stopped_as_error`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.associate](https://wg21.link/exec.associate) `execution::associated`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.stop.when](https://wg21.link/exec.stop.when) Exposition-only <code><i>execution::stop_when</i></code>
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.spawn.future](https://wg21.link/exec.spawn.future) `execution::spawn_future`
## &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.consumers](https://wg21.link/exec.consumers) Sender consumers
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.sync.wait](https://wg21.link/exec.sync.wait) `this_thread::sync_wait`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.sync.wait.var](https://wg21.link/exec.sync.wait.var) `this_thread::sync_with_variant`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.spawn](https://wg21.link/exec.spawn) `execution::spawn`
## &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.cmplsig](https://wg21.link/exec.cmplsig) Completion signatures
## &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.envs](https://wg21.link/exec.envs) Queryable utilities
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.prop](https://wg21.link/exec.prop) Class template `prop`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.env](https://wg21.link/exec.env) Class template `env`
## &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.ctx](https://wg21.link/exec.ctx) Execution contexts
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.run.loop](https://wg21.link/exec.run.loop) `execution::run_loop`
#### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.run.loop.general](https://wg21.link/exec.run.loop.general) General
#### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.run.loop.types](https://wg21.link/exec.run.loop.types) Associated types
#### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.run.loop.ctor](https://wg21.link/exec.run.loop.ctor) Constructor and destructor
#### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.run.loop.members](https://wg21.link/exec.run.loop.members) Member functions
## &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.coro.util](https://wg21.link/exec.coro.util) Coroutine utilities
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.as.awaitable](https://wg21.link/exec.as.awaitable) `execution::as_waitable`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.with.awaitable.senders](https://wg21.link/exec.with.awaitable.senders) `execution::as_waitable`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.affine](https://wg21.link/exec.affine) `execution::as_waitable`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.inline.scheduler](https://wg21.link/exec.inline.scheduler) `execution::as_waitable`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.task.scheduler](https://wg21.link/exec.task.scheduler) `execution::as_waitable`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.task](https://wg21.link/exec.task) `execution::as_waitable`
#### &#x1F6A7;&#x1F6A7;&#x1F6A7; [task.overview](https://wg21.link/task.overview) `task` overview
#### &#x1F6A7;&#x1F6A7;&#x1F6A7; [task.class](https://wg21.link/task.class) Class template `task`
#### &#x1F6A7;&#x1F6A7;&#x1F6A7; [task.members](https://wg21.link/task.members) `task` members
#### &#x1F6A7;&#x1F6A7;&#x1F6A7; [task.state](https://wg21.link/task.state) Class template `task::state`
#### &#x1F6A7;&#x1F6A7;&#x1F6A7; [task.promise](https://wg21.link/task.promise) Class template `task::promise_type`
## &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.scope](https://wg21.link/exec.scope) Execution scope utilities
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.scope.concepts](https://wg21.link/exec.scope.concepts) Execution scope concepts
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.counting.scopes](https://wg21.link/exec.counting.scopes) Counting scopes
#### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.counting.scopes.general](https://wg21.link/exec.counting.scopes.general) General
#### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.scope.simple.counting](https://wg21.link/exec.scope.simple.counting) Simple Counting Scope
##### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.scope.simple.counting.general](https://wg21.link/exec.scope.simple.counting.general) General
##### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.simple.counting.ctor](https://wg21.link/exec.simple.counting.ctor) Constructor and Destructor
##### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.simple.counting.mem](https://wg21.link/exec.simple.counting.mem) Members
##### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.simple.counting.token](https://wg21.link/exec.simple.counting.token) Token
#### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.scope.counting](https://wg21.link/exec.scope.counting) Counting Scope
## &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.par.scheduler](https://wg21.link/exec.par.scheduler) Parallel scheduler
## &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.parschedrepl](https://wg21.link/exec.parschedrepl) Namespace `parallel_scheduler_replacement`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.parschedrepl.general](https://wg21.link/exec.parschedrepl.general) General
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.parschedrepl.recvproxy](https://wg21.link/exec.parschedrepl.recvproxy) Receiver proxies
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.parschedrepl.query](https://wg21.link/exec.parschedrepl.query) `query_parallel_scheduler_backend`
### &#x1F6A7;&#x1F6A7;&#x1F6A7; [exec.parschedrepl.psb](https://wg21.link/exec.parschedrepl.psb) Class `parallel_scheduler_backend`
