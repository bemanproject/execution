/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "beman::execution", "index.html", [
    [ "Asynchronous Operation Support", "index.html", null ],
    [ "contributing", "md_docs_contributing.html", [
      [ "Contributing to <tt>beman::execution</tt>", "md_docs_contributing.html#autotoc_md0", null ]
    ] ],
    [ "implementation-status", "md_docs_implementation_status.html", [
      [ "Implementation Status", "md_docs_implementation_status.html#autotoc_md1", null ],
      [ "&#x1F534;&#x1F534;&#x1F534; <a href=\"https://wg21.link/exec\" >exec</a> Execution control library", "md_docs_implementation_status.html#autotoc_md2", [
        [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.general\" >exec.general</a> General", "md_docs_implementation_status.html#autotoc_md3", null ],
        [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.queryable\" >exec.queryable</a> Query and queryables", "md_docs_implementation_status.html#autotoc_md4", [
          [ "&#x274E;&#x274E;&#x274E; <a href=\"https://wg21.link/exec.queryable.general\" >exec.queryable.general</a> General", "md_docs_implementation_status.html#autotoc_md5", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.queryable.concept\" >exec.queryable.concept</a> Queryable concept", "md_docs_implementation_status.html#autotoc_md6", null ]
        ] ],
        [ "&#x274E;&#x274E;&#x274E; <a href=\"https://wg21.link/exec.async.ops\" >exec.async.ops</a> Asynchronous operations", "md_docs_implementation_status.html#autotoc_md7", null ],
        [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/execution.syn\" >execution.syn</a> Header <tt><execution></tt> synopsis", "md_docs_implementation_status.html#autotoc_md8", null ],
        [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.queries\" >exec.queries</a> Queries", "md_docs_implementation_status.html#autotoc_md9", [
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.queries.expos\" >exec.queries.expos</a> Query utilities", "md_docs_implementation_status.html#autotoc_md10", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.fwd.env\" >exec.fwd.env</a> <tt>forwarding_query</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/forwarding_query.hpp\" ><tt>forwarding_query.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md11", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.get.allocator\" >exec.get.allocator</a> <tt>get_allocator</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_allocator.hpp\" ><tt>get_allocator.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md12", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.get.stop.token\" >exec.get.stop.token</a> <tt>get_stop_token</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_stop_token.hpp\" ><tt>get_stop_token.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md13", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.get.env\" >exec.get.env</a> <tt>execution::get_env</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_env.hpp\" ><tt>get_env.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md14", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.get.domain\" >exec.get.domain</a> <tt>execution::get_domain</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_domain.hpp\" ><tt>get_domain.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md15", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.get.scheduler\" >exec.get.scheduler</a> <tt>execution::get_scheduler</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_scheduler.hpp\" ><tt>get_scheduler.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md16", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.get.start.scheduler\" >exec.get.start.scheduler</a> <tt>execution::get_start_scheduler</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_start_scheduler.hpp\" ><tt>get_start_scheduler.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md17", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.get.delegation.scheduler\" >exec.get.delegation.scheduler</a> <tt>execution::get_delegation_scheduler</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_delegation_scheduler.hpp\" ><tt>get_delegation_scheduler.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md18", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.get.fwd.progress\" >exec.get.fwd.progress</a> <tt>execution::get_forward_progress_guarantee</tt>", "md_docs_implementation_status.html#autotoc_md19", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.get.compl.sched\" >exec.get.compl.sched</a> <tt>execution::get_completion_scheduler</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_completion_scheduler.hpp\" ><tt>get_completion_scheduler.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md20", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.get.compl.domain\" >exec.get.compl.domain</a> <tt>execution::get_completion_domain</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_completion_domain.hpp\" ><tt>get_completion_domain.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md21", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.get.await.adapt\" >exec.get.await.adapt</a> <tt>execution::get_await_completion_adaptor</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_await_completion_adaptor.hpp\" ><tt>get_await_completion_adaptor.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md22", null ]
        ] ],
        [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.sched\" >exec.sched</a> Schedulers", "md_docs_implementation_status.html#autotoc_md23", null ],
        [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.recv\" >exec.recv</a> Receivers", "md_docs_implementation_status.html#autotoc_md24", [
          [ "&#x2705;&#x2705;&#x12705; <a href=\"https://wg21.link/exec.recv.concepts\" >exec.recv.concepts</a> Receiver concepts", "md_docs_implementation_status.html#autotoc_md25", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.set.value\" >exec.set.value</a> <tt>execution::set_value</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_value.hpp\" ><tt>set_value.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md26", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.set.error\" >exec.set.error</a> <tt>execution::set_error</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_error.hpp\" ><tt>set_error.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md27", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.set.stopped\" >exec.set.stopped</a> <tt>execution::set_stopped</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_stopped.hpp\" ><tt>set_stopped.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md28", null ]
        ] ],
        [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.opstate\" >exec.opstate</a> Operation states", "md_docs_implementation_status.html#autotoc_md29", [
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.opstate.general\" >exec.opstate.general</a> General: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/operation_state.hpp\" ><tt>operation_state.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md30", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.opstate.start\" >exec.opstate.start</a> <tt>execution::start</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/start.hpp\" ><tt>start.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md31", null ]
        ] ],
        [ "&#x1F534;&#x1F534;&#x1F534; <a href=\"https://wg21.link/exec.snd\" >exec.snd</a> Senders", "md_docs_implementation_status.html#autotoc_md32", [
          [ "&#x274E;&#x274E;&#x274E; <a href=\"https://wg21.link/exec.snd.general\" >exec.snd.general</a> General", "md_docs_implementation_status.html#autotoc_md33", null ],
          [ "&#x1F534;&#x1F534;&#x1F534; <a href=\"https://wg21.link/exec.snd.expos\" >exec.snd.expos</a> Exposition-only entities", "md_docs_implementation_status.html#autotoc_md34", null ],
          [ "&#x1F534;&#x1F534;&#x1F534; <a href=\"https://wg21.link/exec.snd.concepts\" >exec.snd.concepts</a> Sender concepts", "md_docs_implementation_status.html#autotoc_md35", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.awaitable\" >exec.awaitable</a> Awaitable helpers", "md_docs_implementation_status.html#autotoc_md36", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.domain.indeterminate\" >exec.domain.indeterminate</a> <tt>execution::indeterminate_domain</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/indeterminate_domain.hpp\" ><tt>indeterminate_domain.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md37", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.domain.default\" >exec.domain.default</a> <tt>execution::default_domain</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/default_domain.hpp\" ><tt>default_domain.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md38", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.snd.transform\" >exec.snd.transform</a> <tt>exuection::transform_sender</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/transform_sender.hpp\" ><tt>transform_sender.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md39", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.snd.apply\" >exec.snd.apply</a> <tt>execution::apply_sender</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/apply_sender.hpp\" ><tt>apply_sender.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md40", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.getcomplsigs\" >exec.getcomplsigs</a> <tt>execution::get_completion_signatures</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_completion_signatures.hpp\" ><tt>get_completion_signatures.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md41", null ],
          [ "&#x1F534;&#x1F534;&#x1F534; <a href=\"https://wg21.link/exec.connect\" >exec.connect</a> <tt>execution::connect</tt>", "md_docs_implementation_status.html#autotoc_md42", null ]
        ] ],
        [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.factories\" >exec.factories</a> Sender factories", "md_docs_implementation_status.html#autotoc_md43", [
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.schedule\" >exec.schedule</a> <tt>execution::schedule</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/schedule.hpp\" ><tt>schedule.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md44", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.just\" >exec.just</a> <tt>execution::just</tt>, <tt>exuection::just_error</tt>, <tt>execution::just_stopped</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/just.hpp\" ><tt>just.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md45", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.read.env\" >exec.read.env</a> <tt>execution::read_env</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/read_env.hpp\" ><tt>read_env.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md46", null ]
        ] ],
        [ "&#x2705;&#x1F534;&#x1F534; <a href=\"https://wg21.link/exec.adapt\" >exec.adapt</a> Sender adaptors", "md_docs_implementation_status.html#autotoc_md47", [
          [ "&#x2705;&#x1F534;&#x1F534; <a href=\"https://wg21.link/exec.adapt.general\" >exec.adapt.general</a> General", "md_docs_implementation_status.html#autotoc_md48", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.adapt.obj\" >exec.adapt.obj</a> Closure objects", "md_docs_implementation_status.html#autotoc_md49", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.write.env\" >exec.write.env</a> <tt>execution::write_env</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/write_env.hpp\" ><tt>write_env.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md50", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.unstoppable\" >exec.unstoppable</a> <tt>execution::unstoppable</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/unstoppable.hpp\" ><tt>unstoppable.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md51", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.starts.on\" >exec.starts.on</a> <tt>execution::starts_on</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/starts_on.hpp\" ><tt>starts_on.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md52", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.continues.on\" >exec.continues.on</a> <tt>execution::continues_on</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/continues_one.hpp\" ><tt>continues_one.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md53", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.schedule.from\" >exec.schedule.from</a> <tt>execution::schedule_from</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/schedule_from.hpp\" ><tt>schedule_from.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md54", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.on\" >exec.on</a> <tt>execution::on</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/on.hpp\" ><tt>on.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md55", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.then\" >exec.then</a> <tt>execution::then</tt>, <tt>execution::upon_error</tt>, <tt>execution::upon_stopped</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/then.hpp\" ><tt>then.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md56", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.let\" >exec.let</a> <tt>execution::let_value</tt>, <tt>execution::let_error</tt>, <tt>execution::let_stopped</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/let.hpp\" ><tt>let.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md57", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.bulk\" >exec.bulk</a> <tt>execution::bulk</tt>, <tt>execution::bulk_chunked</tt>, and <tt>execution::bulk_unchunked</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/bulk.hpp\" ><tt>bulk.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md58", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.when.all\" >exec.when.all</a> <tt>execution::when_all</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/when_all.hpp\" ><tt>when_all.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md59", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.into.variant\" >exec.into.variant</a> <tt>execution::into_variant</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/into_variant.hpp\" ><tt>into_variant.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md60", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.stopped.opt\" >exec.stopped.opt</a> <tt>execution::stopped_as_optional</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/stopped_as_optional.hpp\" ><tt>stopped_as_optional.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md61", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.stopped.err\" >exec.stopped.err</a> <tt>execution::stopped_as_error</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/stopped_as_error.hpp\" ><tt>stopped_as_error.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md62", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.associate\" >exec.associate</a> <tt>execution::associate</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/associate.hpp\" ><tt>associate.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md63", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.stop.when\" >exec.stop.when</a> Exposition-only <code><i>execution::stop_when</i></code>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/stop_when.hpp\" ><tt>stop_when.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md64", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.spawn.future\" >exec.spawn.future</a> <tt>execution::spawn_future</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/spawn_future.hpp\" ><tt>spawn_future.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md65", null ]
        ] ],
        [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.consumers\" >exec.consumers</a> Sender consumers", "md_docs_implementation_status.html#autotoc_md66", [
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.sync.wait\" >exec.sync.wait</a> <tt>this_thread::sync_wait</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sync_wait.hpp\" ><tt>sync_wait.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md67", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.sync.wait.var\" >exec.sync.wait.var</a> <tt>this_thread::sync_with_variant</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sync_wait_with_variant.hpp\" ><tt>sync_wait_with_variant.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md68", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.spawn\" >exec.spawn</a> <tt>execution::spawn</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/spawn.hpp\" ><tt>spawn.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md69", null ]
        ] ],
        [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.cmplsig\" >exec.cmplsig</a> Completion signatures", "md_docs_implementation_status.html#autotoc_md70", null ],
        [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.envs\" >exec.envs</a> Queryable utilities", "md_docs_implementation_status.html#autotoc_md71", [
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.prop\" >exec.prop</a> Class template <tt>prop</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/prop.hpp\" ><tt>prop.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md72", null ],
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.env\" >exec.env</a> Class template <tt>env</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/env.hpp\" ><tt>env.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md73", null ]
        ] ],
        [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.ctx\" >exec.ctx</a> Execution contexts", "md_docs_implementation_status.html#autotoc_md74", [
          [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.run.loop\" >exec.run.loop</a> <tt>execution::run_loop</tt>", "md_docs_implementation_status.html#autotoc_md75", [
            [ "&#x2705;&#x2705;&#x2705; <a href=\"https://wg21.link/exec.run.loop.general\" >exec.run.loop.general</a> General: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/run_loop.hpp\" ><tt>run_loop.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md76", null ],
            [ "&#x2705;&#x2705;&#x274E; <a href=\"https://wg21.link/exec.run.loop.types\" >exec.run.loop.types</a> Associated types", "md_docs_implementation_status.html#autotoc_md77", null ],
            [ "&#x2705;&#x2705;&#x274E; <a href=\"https://wg21.link/exec.run.loop.ctor\" >exec.run.loop.ctor</a> Constructor and destructor", "md_docs_implementation_status.html#autotoc_md78", null ],
            [ "&#x2705;&#x2705;&#x274E; <a href=\"https://wg21.link/exec.run.loop.members\" >exec.run.loop.members</a> Member functions", "md_docs_implementation_status.html#autotoc_md79", null ]
          ] ]
        ] ],
        [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.coro.util\" >exec.coro.util</a> Coroutine utilities", "md_docs_implementation_status.html#autotoc_md80", [
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.as.awaitable\" >exec.as.awaitable</a> <tt>execution::as_awaitable</tt>", "md_docs_implementation_status.html#autotoc_md81", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.with.awaitable.senders\" >exec.with.awaitable.senders</a> <tt>execution::with_awaitable_senders</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/with_awaitable_senders.hpp\" ><tt>with_awaitable_senders.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md82", null ],
          [ "&#x2705;&#x2705;&#2705; <a href=\"https://wg21.link/exec.affine\" >exec.affine</a> <tt>execution::affine</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/affine.hpp\" ><tt>affine.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md83", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.inline.scheduler\" >exec.inline.scheduler</a> <tt>execution::inline_scheduler</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/inline_scheduler.hpp\" ><tt>inline_scheduler.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md84", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.task.scheduler\" >exec.task.scheduler</a> <tt>execution::task_scheduler</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/task_scheduler.hpp\" ><tt>task_scheduler.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md85", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.task\" >exec.task</a> <tt>execution::task</tt>", "md_docs_implementation_status.html#autotoc_md86", [
            [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/task.overview\" >task.overview</a> <tt>task</tt> overview", "md_docs_implementation_status.html#autotoc_md87", null ],
            [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/task.class\" >task.class</a> Class template <tt>task</tt>", "md_docs_implementation_status.html#autotoc_md88", null ],
            [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/task.members\" >task.members</a> <tt>task</tt> members", "md_docs_implementation_status.html#autotoc_md89", null ],
            [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/task.state\" >task.state</a> Class template <tt>task::state</tt>", "md_docs_implementation_status.html#autotoc_md90", null ],
            [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/task.promise\" >task.promise</a> Class template <tt>task::promise_type</tt>", "md_docs_implementation_status.html#autotoc_md91", null ]
          ] ]
        ] ],
        [ "&#x1F534;&#x1F534;&#x1F534; <a href=\"https://wg21.link/exec.scope\" >exec.scope</a> Execution scope utilities", "md_docs_implementation_status.html#autotoc_md92", [
          [ "&#x2705;&#x1F534;&#x1F534; <a href=\"https://wg21.link/exec.scope.concepts\" >exec.scope.concepts</a> Execution scope concepts", "md_docs_implementation_status.html#autotoc_md93", null ],
          [ "&#x1F534;&#x1F534;&#x1F534; <a href=\"https://wg21.link/exec.counting.scopes\" >exec.counting.scopes</a> Counting scopes", "md_docs_implementation_status.html#autotoc_md94", [
            [ "&#x1F534;&#x1F534;&#x1F534; <a href=\"https://wg21.link/exec.counting.scopes.general\" >exec.counting.scopes.general</a> General", "md_docs_implementation_status.html#autotoc_md95", null ],
            [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.scope.simple.counting\" >exec.scope.simple.counting</a> Simple Counting Scope", "md_docs_implementation_status.html#autotoc_md96", null ],
            [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.scope.counting\" >exec.scope.counting</a> Counting Scope", "md_docs_implementation_status.html#autotoc_md101", null ]
          ] ]
        ] ],
        [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.par.scheduler\" >exec.par.scheduler</a> Parallel scheduler: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/parallel_scheduler.hpp\" ><tt>parallel_scheduler.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md102", null ],
        [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.parschedrepl\" >exec.parschedrepl</a> Namespace <tt>parallel_scheduler_replacement</tt>", "md_docs_implementation_status.html#autotoc_md103", [
          [ "&#x274E;&#x274E;&#x274E; <a href=\"https://wg21.link/exec.parschedrepl.general\" >exec.parschedrepl.general</a> General", "md_docs_implementation_status.html#autotoc_md104", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.parschedrepl.recvproxy\" >exec.parschedrepl.recvproxy</a> Receiver proxies: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/parallel_scheduler_replacement.hpp\" ><tt>parallel_scheduler_replacement.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md105", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.parschedrepl.query\" >exec.parschedrepl.query</a> <tt>query_parallel_scheduler_backend</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/parallel_scheduler_replacement.hpp\" ><tt>parallel_scheduler_replacement.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md106", null ],
          [ "&#x2705;&#x2705;&#x1F534; <a href=\"https://wg21.link/exec.parschedrepl.psb\" >exec.parschedrepl.psb</a> Class <tt>parallel_scheduler_backend</tt>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/parallel_scheduler_replacement.hpp\" ><tt>parallel_scheduler_replacement.hpp</tt></a>", "md_docs_implementation_status.html#autotoc_md107", null ]
        ] ]
      ] ]
    ] ],
    [ "intro_examples", "md_docs_intro_examples.html", [
      [ "Introduction by Example", "md_docs_intro_examples.html#autotoc_md108", null ]
    ] ],
    [ "Supporting Modules", "md_docs_modules.html", [
      [ "History of Adding Module Support", "md_docs_modules.html#autotoc_md110", [
        [ "Use <tt>export using</tt>", "md_docs_modules.html#autotoc_md111", null ],
        [ "Generate Module Friendly Code: <tt>mk-module.py</tt>", "md_docs_modules.html#autotoc_md112", null ],
        [ "Retry <tt>export using</tt>", "md_docs_modules.html#autotoc_md113", null ],
        [ "<tt>import std;</tt>", "md_docs_modules.html#autotoc_md114", null ]
      ] ],
      [ "Changes Needed to Support Modules", "md_docs_modules.html#autotoc_md115", null ],
      [ "Scanning and Building", "md_docs_modules.html#autotoc_md116", null ],
      [ "Modules vs. Testing", "md_docs_modules.html#autotoc_md117", null ],
      [ "Conclusion", "md_docs_modules.html#autotoc_md118", null ]
    ] ],
    [ "overview", "md_docs_overview.html", [
      [ "std::execution Overview", "md_docs_overview.html#autotoc_md119", [
        [ "Terms", "md_docs_overview.html#autotoc_md120", null ],
        [ "Concepts", "md_docs_overview.html#autotoc_md121", null ],
        [ "Queries", "md_docs_overview.html#autotoc_md122", null ],
        [ "Customization Point Objects", "md_docs_overview.html#autotoc_md123", null ],
        [ "Senders", "md_docs_overview.html#autotoc_md124", [
          [ "Sender Factories", "md_docs_overview.html#autotoc_md125", null ],
          [ "Sender Adaptors", "md_docs_overview.html#autotoc_md126", null ],
          [ "Sender Consumers", "md_docs_overview.html#autotoc_md127", null ]
        ] ],
        [ "Helpers", "md_docs_overview.html#autotoc_md128", null ],
        [ "Stop Token", "md_docs_overview.html#autotoc_md129", null ],
        [ "Exposition Only", "md_docs_overview.html#autotoc_md130", null ]
      ] ]
    ] ],
    [ "Questions", "md_docs_questions.html", null ],
    [ "resources", "md_docs_resources.html", [
      [ "Resources Related to <tt>beman::execution</tt>", "md_docs_resources.html#autotoc_md132", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ]
      ] ]
    ] ],
    [ "Concepts", "concepts.html", "concepts" ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"affine_8hpp_source.html",
"md_docs_implementation_status.html#autotoc_md39",
"structbeman_1_1execution_1_1is__execution__policy_3_01_1_1beman_1_1execution_1_1sequenced__policy_01_4.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';