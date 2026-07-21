# Implementation Status

# &#x1F534;&#x1F534;&#x1F534; [exec](https://wg21.link/exec) Execution control library
## &#x1F534;&#x2705;&#x2705; [exec.general](https://wg21.link/exec.general) General

- &#x1F534;&#x274E;&#x274E; [<code><i>MANDATE-NOTHROW</i>(expr)</code>](https://wg21.link/exec.general#5) &#x21d2; <code>noexcept(<i>expr</i>)</code> is `true`
- &#x2705;&#x2705;&#x2705; [<code><i>movable-value</i>&lt;T&gt;</code>](https://wg21.link/exec.general#6): [`movable_value.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/movable_value.hpp)
- &#x2705;&#x2705;&#x2705; [<code><i>MATCHING-SIG</i>&lt;F1, F2&gt;</code>](https://wg21.link/exec.general#7): [`matching_sig.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/matching_sig.hpp)
- &#x2705;&#x2705;&#x2705; [<code><i>AS-EXCEPT-PTR</i>(error)</code>](https://wg21.link/exec.general#8): [`as_except_ptr.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/as_except_ptr.hpp)
- &#x1F534;&#x274E;&#x274E; [<code><i>as-const</i>(error)</code>](https://wg21.link/exec.general#9): <code>[std::as_const](https://wg21.link/utility.as.const)(error)</code>

## &#x1F534;&#x2705;&#x2705; [exec.queryable](https://wg21.link/exec.queryable) Query and queryables
### &#x1F534;&#x274E;&#x274E; [exec.queryable.general](https://wg21.link/exec.queryable.general) General
### &#x1F534;&#x2705;&#x2705; [exec.queryable.concept](https://wg21.link/exec.queryable.concept) Queryable concept

- &#x1F534;&#x2705;&#x2705; [<code><i>queryable</i>&lt;Q&gt;</code>](https://wg21.link/exec.queryable.concept#1): [`queryable.hpp`](https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/queryable.hpp)

## &#x274E;&#x274E;&#x274E; [exec.async.ops](https://wg21.link/exec.async.ops) Asynchronous operations
## &#x1F534;&#x1F534;&#x1F534; [execution.syn](https://wg21.link/execution.syn) Header `<execution>` synopsis

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

### &#x2705;&#x2705;&#x2705; [exec.fwd.env](https://wg21.link/exec.fwd.env) `forwarding_query`
### &#x2705;&#x2705;&#x2705; [exec.get.allocator](https://wg21.link/exec.get.allocator) `get_allocator`
### &#x2705;&#x2705;&#x2705; [exec.get.stop.token](https://wg21.link/exec.get.stop.token) `get_stop_token`
### &#x2705;&#x2705;&#x2705; [exec.get.env](https://wg21.link/exec.get.env) `execution::get_env`
### &#x2705;&#x2705;&#x2705; [exec.get.domain](https://wg21.link/exec.get.domain) `execution::get_domain`
### &#x2705;&#x2705;&#x2705; [exec.get.scheduler](https://wg21.link/exec.get.scheduler) `execution::get_scheduler`
### &#x2705;&#x2705;&#x2705; [exec.get.start.scheduler](https://wg21.link/exec.get.start.scheduler) `execution::get_start_scheduler`
### &#x2705;&#x2705;&#x2705; [exec.get.delegation.scheduler](https://wg21.link/exec.get.delegation.scheduler) `execution::get_delegation_scheduler`
### &#x1F534;&#x1F534;&#x1F534; [exec.get.fwd.progress](https://wg21.link/exec.get.fwd.progress) `execution::get_forward_progress_guarantee`

- &#x1F534;&#x1F534;&#x1F534; [`forward_progress_guarantee`](https://wg21.link/exec.get.fwd.progress)
- &#x1F534;&#x1F534;&#x1F534; [`get_forward_progress_guarantee`](https://wg21.link/exec.get.fwd.progress#2)

### &#x2705;&#x2705;&#x2705; [exec.get.compl.sched](https://wg21.link/exec.get.compl.sched) `execution::get_completion_scheduler`
### &#x2705;&#x2705;&#x1F534; [exec.get.compl.domain](https://wg21.link/exec.get.compl.domain) `execution::get_completion_domain`
### &#x2705;&#x1F534;&#x1F534; [exec.get.await.adapt](https://wg21.link/exec.get.await.adapt) `execution::get_await_completion_adaptor`
## &#x1F534;&#x1F534;&#x1F534; [exec.sched](https://wg21.link/exec.sched) Schedulers
## &#x1F534;&#x1F534;&#x1F534; [exec.recv](https://wg21.link/exec.recv) Receivers
### &#x1F534;&#x1F534;&#x1F534; [exec.recv.concepts](https://wg21.link/exec.recv.concepts) Receiver concepts
### &#x1F534;&#x1F534;&#x1F534; [exec.set.value](https://wg21.link/exec.set.value) `execution::set_value`
### &#x1F534;&#x1F534;&#x1F534; [exec.set.error](https://wg21.link/exec.set.error) `execution::set_error`
### &#x1F534;&#x1F534;&#x1F534; [exec.set.stopped](https://wg21.link/exec.set.stopped) `execution::set_stopped`
## &#x1F534;&#x1F534;&#x1F534; [exec.opstate](https://wg21.link/exec.opstate) Operation states
### &#x1F534;&#x1F534;&#x1F534; [exec.opstate.general](https://wg21.link/exec.opstate.general) General
### &#x1F534;&#x1F534;&#x1F534; [exec.opstate.start](https://wg21.link/exec.opstate.start) `execution::start`
## &#x1F534;&#x1F534;&#x1F534; [exec.snd](https://wg21.link/exec.snd) Senders
### &#x1F534;&#x1F534;&#x1F534; [exec.snd.general](https://wg21.link/exec.snd.general) General
### &#x1F534;&#x1F534;&#x1F534; [exec.snd.expos](https://wg21.link/exec.snd.expos) Exposition-only entities
### &#x1F534;&#x1F534;&#x1F534; [exec.snd.concepts](https://wg21.link/exec.snd.concepts) Sender concepts
### &#x1F534;&#x1F534;&#x1F534; [exec.awaitable](https://wg21.link/exec.awaitable) Awaitable helpers
### &#x1F534;&#x1F534;&#x1F534; [exec.domain.indeterminate](https://wg21.link/exec.domain.indeterminate) `execution::indeterminate_domain`
### &#x1F534;&#x1F534;&#x1F534; [exec.domain.default](https://wg21.link/exec.domain.default) `execution::default_domain`
### &#x1F534;&#x1F534;&#x1F534; [exec.snd.transform](https://wg21.link/exec.snd.transform) `exuection::transform_sender`
### &#x1F534;&#x1F534;&#x1F534; [exec.snd.apply](https://wg21.link/exec.snd.apply) `execution::apply_sender`
### &#x1F534;&#x1F534;&#x1F534; [exec.getcomplsigs](https://wg21.link/exec.getcomplsigs) `execution::get_completion_signatures`
### &#x1F534;&#x1F534;&#x1F534; [exec.connect](https://wg21.link/exec.connect) `execution::connect`
## &#x1F534;&#x1F534;&#x1F534; [exec.factories](https://wg21.link/exec.factories) Sender factories
### &#x1F534;&#x1F534;&#x1F534; [exec.schedule](https://wg21.link/exec.schedule) `execution::schedule`
### &#x1F534;&#x1F534;&#x1F534; [exec.just](https://wg21.link/exec.just) `execution::just`, `exuection::just_error`, `execution::just_stopped`
### &#x1F534;&#x1F534;&#x1F534; [exec.read.env](https://wg21.link/exec.read.env) `execution::read_env`
## &#x1F534;&#x1F534;&#x1F534; [exec.adapt](https://wg21.link/exec.adapt) Sender adaptors
### &#x1F534;&#x1F534;&#x1F534; [exec.adapt.general](https://wg21.link/exec.adapt.general) General
### &#x1F534;&#x1F534;&#x1F534; [exec.adapt.obj](https://wg21.link/exec.adapt.obj) Closer objects
### &#x1F534;&#x1F534;&#x1F534; [exec.write.env](https://wg21.link/exec.write.env) `execution::write_env`
### &#x1F534;&#x1F534;&#x1F534; [exec.unstoppable](https://wg21.link/exec.unstoppable) `execution::unstoppable`
### &#x1F534;&#x1F534;&#x1F534; [exec.starts.on](https://wg21.link/exec.starts.on) `execution::starts_on`
### &#x1F534;&#x1F534;&#x1F534; [exec.continues.on](https://wg21.link/exec.continues.on) `execution::continues_on`
### &#x1F534;&#x1F534;&#x1F534; [exec.schedule.from](https://wg21.link/exec.schedule.from) `execution::schedule_from`
### &#x1F534;&#x1F534;&#x1F534; [exec.on](https://wg21.link/exec.on) `execution::on`
### &#x1F534;&#x1F534;&#x1F534; [exec.then](https://wg21.link/exec.then) `execution::then`, `execution::upon_error`, `execution::upon_stopped`
### &#x1F534;&#x1F534;&#x1F534; [exec.let](https://wg21.link/exec.let) `execution::let_value`, `execution::let_error`, `execution::let_stopped`
### &#x1F534;&#x1F534;&#x1F534; [exec.bulk](https://wg21.link/exec.bulk) `execution::bulk`, `execution::bulk_chunked`, and `execution::bulk_unchunked`
### &#x1F534;&#x1F534;&#x1F534; [exec.when.all](https://wg21.link/exec.when.all) `execution::when_all`
### &#x1F534;&#x1F534;&#x1F534; [exec.into.variant](https://wg21.link/exec.into.variant) `execution::into_variant`
### &#x1F534;&#x1F534;&#x1F534; [exec.stopped.opt](https://wg21.link/exec.stopped.opt) `execution::stopped_as_optional`
### &#x1F534;&#x1F534;&#x1F534; [exec.stopped.err](https://wg21.link/exec.stopped.err) `execution::stopped_as_error`
### &#x1F534;&#x1F534;&#x1F534; [exec.associate](https://wg21.link/exec.associate) `execution::associated`
### &#x1F534;&#x1F534;&#x1F534; [exec.stop.when](https://wg21.link/exec.stop.when) Exposition-only <code><i>execution::stop_when</i></code>
### &#x1F534;&#x1F534;&#x1F534; [exec.spawn.future](https://wg21.link/exec.spawn.future) `execution::spawn_future`
## &#x1F534;&#x1F534;&#x1F534; [exec.consumers](https://wg21.link/exec.consumers) Sender consumers
### &#x1F534;&#x1F534;&#x1F534; [exec.sync.wait](https://wg21.link/exec.sync.wait) `this_thread::sync_wait`
### &#x1F534;&#x1F534;&#x1F534; [exec.sync.wait.var](https://wg21.link/exec.sync.wait.var) `this_thread::sync_with_variant`
### &#x1F534;&#x1F534;&#x1F534; [exec.spawn](https://wg21.link/exec.spawn) `execution::spawn`
## &#x1F534;&#x1F534;&#x1F534; [exec.cmplsig](https://wg21.link/exec.cmplsig) Completion signatures
## &#x1F534;&#x1F534;&#x1F534; [exec.envs](https://wg21.link/exec.envs) Queryable utilities
### &#x1F534;&#x1F534;&#x1F534; [exec.prop](https://wg21.link/exec.prop) Class template `prop`
### &#x1F534;&#x1F534;&#x1F534; [exec.env](https://wg21.link/exec.env) Class template `env`
## &#x1F534;&#x1F534;&#x1F534; [exec.ctx](https://wg21.link/exec.ctx) Execution contexts
### &#x1F534;&#x1F534;&#x1F534; [exec.run.loop](https://wg21.link/exec.run.loop) `execution::run_loop`
#### &#x1F534;&#x1F534;&#x1F534; [exec.run.loop.general](https://wg21.link/exec.run.loop.general) General
#### &#x1F534;&#x1F534;&#x1F534; [exec.run.loop.types](https://wg21.link/exec.run.loop.types) Associated types
#### &#x1F534;&#x1F534;&#x1F534; [exec.run.loop.ctor](https://wg21.link/exec.run.loop.ctor) Constructor and destructor
#### &#x1F534;&#x1F534;&#x1F534; [exec.run.loop.members](https://wg21.link/exec.run.loop.members) Member functions
## &#x1F534;&#x1F534;&#x1F534; [exec.coro.util](https://wg21.link/exec.coro.util) Coroutine utilities
### &#x1F534;&#x1F534;&#x1F534; [exec.as.awaitable](https://wg21.link/exec.as.awaitable) `execution::as_waitable`
### &#x1F534;&#x1F534;&#x1F534; [exec.with.awaitable.senders](https://wg21.link/exec.with.awaitable.senders) `execution::as_waitable`
### &#x1F534;&#x1F534;&#x1F534; [exec.affine](https://wg21.link/exec.affine) `execution::as_waitable`
### &#x1F534;&#x1F534;&#x1F534; [exec.inline.scheduler](https://wg21.link/exec.inline.scheduler) `execution::as_waitable`
### &#x1F534;&#x1F534;&#x1F534; [exec.task.scheduler](https://wg21.link/exec.task.scheduler) `execution::as_waitable`
### &#x1F534;&#x1F534;&#x1F534; [exec.task](https://wg21.link/exec.task) `execution::as_waitable`
#### &#x1F534;&#x1F534;&#x1F534; [task.overview](https://wg21.link/task.overview) `task` overview
#### &#x1F534;&#x1F534;&#x1F534; [task.class](https://wg21.link/task.class) Class template `task`
#### &#x1F534;&#x1F534;&#x1F534; [task.members](https://wg21.link/task.members) `task` members
#### &#x1F534;&#x1F534;&#x1F534; [task.state](https://wg21.link/task.state) Class template `task::state`
#### &#x1F534;&#x1F534;&#x1F534; [task.promise](https://wg21.link/task.promise) Class template `task::promise_type`
## &#x1F534;&#x1F534;&#x1F534; [exec.scope](https://wg21.link/exec.scope) Execution scope utilities
### &#x1F534;&#x1F534;&#x1F534; [exec.scope.concepts](https://wg21.link/exec.scope.concepts) Execution scope concepts
### &#x1F534;&#x1F534;&#x1F534; [exec.counting.scopes](https://wg21.link/exec.counting.scopes) Counting scopes
#### &#x1F534;&#x1F534;&#x1F534; [exec.counting.scopes.general](https://wg21.link/exec.counting.scopes.general) General
#### &#x1F534;&#x1F534;&#x1F534; [exec.scope.simple.counting](https://wg21.link/exec.scope.simple.counting) Simple Counting Scope
##### &#x1F534;&#x1F534;&#x1F534; [exec.scope.simple.counting.general](https://wg21.link/exec.scope.simple.counting.general) General
##### &#x1F534;&#x1F534;&#x1F534; [exec.simple.counting.ctor](https://wg21.link/exec.simple.counting.ctor) Constructor and Destructor
##### &#x1F534;&#x1F534;&#x1F534; [exec.simple.counting.mem](https://wg21.link/exec.simple.counting.mem) Members
##### &#x1F534;&#x1F534;&#x1F534; [exec.simple.counting.token](https://wg21.link/exec.simple.counting.token) Token
#### &#x1F534;&#x1F534;&#x1F534; [exec.scope.counting](https://wg21.link/exec.scope.counting) Counting Scope
## &#x1F534;&#x1F534;&#x1F534; [exec.par.scheduler](https://wg21.link/exec.par.scheduler) Parallel scheduler
## &#x1F534;&#x1F534;&#x1F534; [exec.parschedrepl](https://wg21.link/exec.parschedrepl) Namespace `parallel_scheduler_replacement`
### &#x1F534;&#x1F534;&#x1F534; [exec.parschedrepl.general](https://wg21.link/exec.parschedrepl.general) General
### &#x1F534;&#x1F534;&#x1F534; [exec.parschedrepl.recvproxy](https://wg21.link/exec.parschedrepl.recvproxy) Receiver proxies
### &#x1F534;&#x1F534;&#x1F534; [exec.parschedrepl.query](https://wg21.link/exec.parschedrepl.query) `query_parallel_scheduler_backend`
### &#x1F534;&#x1F534;&#x1F534; [exec.parschedrepl.psb](https://wg21.link/exec.parschedrepl.psb) Class `parallel_scheduler_backend`
