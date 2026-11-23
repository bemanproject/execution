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
    [ "contributing", "md_docs_2contributing.html", [
      [ "Contributing to <span class=\"tt\">beman::execution</span>", "md_docs_2contributing.html#autotoc_md0", null ]
    ] ],
    [ "implementation-status", "md_docs_2implementation-status.html", [
      [ "Implementation Status", "md_docs_2implementation-status.html#autotoc_md1", null ],
      [ "🔴🔴🔴 <a href=\"https://wg21.link/exec\">exec</a> Execution control library", "md_docs_2implementation-status.html#autotoc_md2", [
        [ "✅✅✅ <a href=\"https://wg21.link/exec.general\">exec.general</a> General", "md_docs_2implementation-status.html#autotoc_md3", null ],
        [ "✅✅✅ <a href=\"https://wg21.link/exec.queryable\">exec.queryable</a> Query and queryables", "md_docs_2implementation-status.html#autotoc_md4", [
          [ "❎❎❎ <a href=\"https://wg21.link/exec.queryable.general\">exec.queryable.general</a> General", "md_docs_2implementation-status.html#autotoc_md5", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.queryable.concept\">exec.queryable.concept</a> Queryable concept", "md_docs_2implementation-status.html#autotoc_md6", null ]
        ] ],
        [ "❎❎❎ <a href=\"https://wg21.link/exec.async.ops\">exec.async.ops</a> Asynchronous operations", "md_docs_2implementation-status.html#autotoc_md7", null ],
        [ "✅✅🔴 <a href=\"https://wg21.link/execution.syn\">execution.syn</a> Header <span class=\"tt\">&lt;execution&gt;</span> synopsis", "md_docs_2implementation-status.html#autotoc_md8", null ],
        [ "✅✅✅ <a href=\"https://wg21.link/exec.queries\">exec.queries</a> Queries", "md_docs_2implementation-status.html#autotoc_md9", [
          [ "✅✅✅ <a href=\"https://wg21.link/exec.queries.expos\">exec.queries.expos</a> Query utilities", "md_docs_2implementation-status.html#autotoc_md10", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.fwd.env\">exec.fwd.env</a> <span class=\"tt\">forwarding_query</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/forwarding_query.hpp\"><span class=\"tt\">forwarding_query.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md11", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.get.allocator\">exec.get.allocator</a> <span class=\"tt\">get_allocator</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_allocator.hpp\"><span class=\"tt\">get_allocator.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md12", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.get.stop.token\">exec.get.stop.token</a> <span class=\"tt\">get_stop_token</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_stop_token.hpp\"><span class=\"tt\">get_stop_token.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md13", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.get.env\">exec.get.env</a> <span class=\"tt\">execution::get_env</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_env.hpp\"><span class=\"tt\">get_env.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md14", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.get.domain\">exec.get.domain</a> <span class=\"tt\">execution::get_domain</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_domain.hpp\"><span class=\"tt\">get_domain.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md15", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.get.scheduler\">exec.get.scheduler</a> <span class=\"tt\">execution::get_scheduler</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_scheduler.hpp\"><span class=\"tt\">get_scheduler.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md16", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.get.start.scheduler\">exec.get.start.scheduler</a> <span class=\"tt\">execution::get_start_scheduler</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_start_scheduler.hpp\"><span class=\"tt\">get_start_scheduler.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md17", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.get.delegation.scheduler\">exec.get.delegation.scheduler</a> <span class=\"tt\">execution::get_delegation_scheduler</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_delegation_scheduler.hpp\"><span class=\"tt\">get_delegation_scheduler.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md18", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.get.fwd.progress\">exec.get.fwd.progress</a> <span class=\"tt\">execution::get_forward_progress_guarantee</span>", "md_docs_2implementation-status.html#autotoc_md19", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.get.compl.sched\">exec.get.compl.sched</a> <span class=\"tt\">execution::get_completion_scheduler</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_completion_scheduler.hpp\"><span class=\"tt\">get_completion_scheduler.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md20", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.get.compl.domain\">exec.get.compl.domain</a> <span class=\"tt\">execution::get_completion_domain</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_completion_domain.hpp\"><span class=\"tt\">get_completion_domain.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md21", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.get.await.adapt\">exec.get.await.adapt</a> <span class=\"tt\">execution::get_await_completion_adaptor</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_await_completion_adaptor.hpp\"><span class=\"tt\">get_await_completion_adaptor.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md22", null ]
        ] ],
        [ "✅✅✅ <a href=\"https://wg21.link/exec.sched\">exec.sched</a> Schedulers", "md_docs_2implementation-status.html#autotoc_md23", null ],
        [ "✅✅✅ <a href=\"https://wg21.link/exec.recv\">exec.recv</a> Receivers", "md_docs_2implementation-status.html#autotoc_md24", [
          [ "✅✅✅ <a href=\"https://wg21.link/exec.recv.concepts\">exec.recv.concepts</a> Receiver concepts", "md_docs_2implementation-status.html#autotoc_md25", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.set.value\">exec.set.value</a> <span class=\"tt\">execution::set_value</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_value.hpp\"><span class=\"tt\">set_value.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md26", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.set.error\">exec.set.error</a> <span class=\"tt\">execution::set_error</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_error.hpp\"><span class=\"tt\">set_error.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md27", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.set.stopped\">exec.set.stopped</a> <span class=\"tt\">execution::set_stopped</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/set_stopped.hpp\"><span class=\"tt\">set_stopped.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md28", null ]
        ] ],
        [ "✅✅✅ <a href=\"https://wg21.link/exec.opstate\">exec.opstate</a> Operation states", "md_docs_2implementation-status.html#autotoc_md29", [
          [ "✅✅✅ <a href=\"https://wg21.link/exec.opstate.general\">exec.opstate.general</a> General: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/operation_state.hpp\"><span class=\"tt\">operation_state.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md30", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.opstate.start\">exec.opstate.start</a> <span class=\"tt\">execution::start</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/start.hpp\"><span class=\"tt\">start.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md31", null ]
        ] ],
        [ "🔴🔴🔴 <a href=\"https://wg21.link/exec.snd\">exec.snd</a> Senders", "md_docs_2implementation-status.html#autotoc_md32", [
          [ "❎❎❎ <a href=\"https://wg21.link/exec.snd.general\">exec.snd.general</a> General", "md_docs_2implementation-status.html#autotoc_md33", null ],
          [ "🔴🔴🔴 <a href=\"https://wg21.link/exec.snd.expos\">exec.snd.expos</a> Exposition-only entities", "md_docs_2implementation-status.html#autotoc_md34", null ],
          [ "🔴🔴🔴 <a href=\"https://wg21.link/exec.snd.concepts\">exec.snd.concepts</a> Sender concepts", "md_docs_2implementation-status.html#autotoc_md35", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.awaitable\">exec.awaitable</a> Awaitable helpers", "md_docs_2implementation-status.html#autotoc_md36", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.domain.indeterminate\">exec.domain.indeterminate</a> <span class=\"tt\">execution::indeterminate_domain</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/indeterminate_domain.hpp\"><span class=\"tt\">indeterminate_domain.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md37", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.domain.default\">exec.domain.default</a> <span class=\"tt\">execution::default_domain</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/default_domain.hpp\"><span class=\"tt\">default_domain.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md38", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.snd.transform\">exec.snd.transform</a> <span class=\"tt\">exuection::transform_sender</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/transform_sender.hpp\"><span class=\"tt\">transform_sender.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md39", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.snd.apply\">exec.snd.apply</a> <span class=\"tt\">execution::apply_sender</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/apply_sender.hpp\"><span class=\"tt\">apply_sender.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md40", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.getcomplsigs\">exec.getcomplsigs</a> <span class=\"tt\">execution::get_completion_signatures</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/get_completion_signatures.hpp\"><span class=\"tt\">get_completion_signatures.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md41", null ],
          [ "🔴🔴🔴 <a href=\"https://wg21.link/exec.connect\">exec.connect</a> <span class=\"tt\">execution::connect</span>", "md_docs_2implementation-status.html#autotoc_md42", null ]
        ] ],
        [ "✅✅✅ <a href=\"https://wg21.link/exec.factories\">exec.factories</a> Sender factories", "md_docs_2implementation-status.html#autotoc_md43", [
          [ "✅✅✅ <a href=\"https://wg21.link/exec.schedule\">exec.schedule</a> <span class=\"tt\">execution::schedule</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/schedule.hpp\"><span class=\"tt\">schedule.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md44", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.just\">exec.just</a> <span class=\"tt\">execution::just</span>, <span class=\"tt\">exuection::just_error</span>, <span class=\"tt\">execution::just_stopped</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/just.hpp\"><span class=\"tt\">just.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md45", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.read.env\">exec.read.env</a> <span class=\"tt\">execution::read_env</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/read_env.hpp\"><span class=\"tt\">read_env.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md46", null ]
        ] ],
        [ "✅🔴🔴 <a href=\"https://wg21.link/exec.adapt\">exec.adapt</a> Sender adaptors", "md_docs_2implementation-status.html#autotoc_md47", [
          [ "✅🔴🔴 <a href=\"https://wg21.link/exec.adapt.general\">exec.adapt.general</a> General", "md_docs_2implementation-status.html#autotoc_md48", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.adapt.obj\">exec.adapt.obj</a> Closure objects", "md_docs_2implementation-status.html#autotoc_md49", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.write.env\">exec.write.env</a> <span class=\"tt\">execution::write_env</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/write_env.hpp\"><span class=\"tt\">write_env.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md50", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.unstoppable\">exec.unstoppable</a> <span class=\"tt\">execution::unstoppable</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/unstoppable.hpp\"><span class=\"tt\">unstoppable.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md51", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.starts.on\">exec.starts.on</a> <span class=\"tt\">execution::starts_on</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/starts_on.hpp\"><span class=\"tt\">starts_on.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md52", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.continues.on\">exec.continues.on</a> <span class=\"tt\">execution::continues_on</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/continues_one.hpp\"><span class=\"tt\">continues_one.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md53", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.schedule.from\">exec.schedule.from</a> <span class=\"tt\">execution::schedule_from</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/schedule_from.hpp\"><span class=\"tt\">schedule_from.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md54", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.on\">exec.on</a> <span class=\"tt\">execution::on</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/on.hpp\"><span class=\"tt\">on.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md55", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.then\">exec.then</a> <span class=\"tt\">execution::then</span>, <span class=\"tt\">execution::upon_error</span>, <span class=\"tt\">execution::upon_stopped</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/then.hpp\"><span class=\"tt\">then.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md56", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.let\">exec.let</a> <span class=\"tt\">execution::let_value</span>, <span class=\"tt\">execution::let_error</span>, <span class=\"tt\">execution::let_stopped</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/let.hpp\"><span class=\"tt\">let.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md57", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.bulk\">exec.bulk</a> <span class=\"tt\">execution::bulk</span>, <span class=\"tt\">execution::bulk_chunked</span>, and <span class=\"tt\">execution::bulk_unchunked</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/bulk.hpp\"><span class=\"tt\">bulk.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md58", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.when.all\">exec.when.all</a> <span class=\"tt\">execution::when_all</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/when_all.hpp\"><span class=\"tt\">when_all.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md59", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.into.variant\">exec.into.variant</a> <span class=\"tt\">execution::into_variant</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/into_variant.hpp\"><span class=\"tt\">into_variant.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md60", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.stopped.opt\">exec.stopped.opt</a> <span class=\"tt\">execution::stopped_as_optional</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/stopped_as_optional.hpp\"><span class=\"tt\">stopped_as_optional.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md61", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.stopped.err\">exec.stopped.err</a> <span class=\"tt\">execution::stopped_as_error</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/stopped_as_error.hpp\"><span class=\"tt\">stopped_as_error.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md62", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.associate\">exec.associate</a> <span class=\"tt\">execution::associate</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/associate.hpp\"><span class=\"tt\">associate.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md63", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.stop.when\">exec.stop.when</a> Exposition-only <code><em>execution::stop_when</em></code>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/stop_when.hpp\"><span class=\"tt\">stop_when.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md64", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.spawn.future\">exec.spawn.future</a> <span class=\"tt\">execution::spawn_future</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/spawn_future.hpp\"><span class=\"tt\">spawn_future.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md65", null ]
        ] ],
        [ "✅✅🔴 <a href=\"https://wg21.link/exec.consumers\">exec.consumers</a> Sender consumers", "md_docs_2implementation-status.html#autotoc_md66", [
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.sync.wait\">exec.sync.wait</a> <span class=\"tt\">this_thread::sync_wait</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sync_wait.hpp\"><span class=\"tt\">sync_wait.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md67", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.sync.wait.var\">exec.sync.wait.var</a> <span class=\"tt\">this_thread::sync_with_variant</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/sync_wait_with_variant.hpp\"><span class=\"tt\">sync_wait_with_variant.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md68", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.spawn\">exec.spawn</a> <span class=\"tt\">execution::spawn</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/spawn.hpp\"><span class=\"tt\">spawn.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md69", null ]
        ] ],
        [ "✅✅🔴 <a href=\"https://wg21.link/exec.cmplsig\">exec.cmplsig</a> Completion signatures", "md_docs_2implementation-status.html#autotoc_md70", null ],
        [ "✅✅✅ <a href=\"https://wg21.link/exec.envs\">exec.envs</a> Queryable utilities", "md_docs_2implementation-status.html#autotoc_md71", [
          [ "✅✅✅ <a href=\"https://wg21.link/exec.prop\">exec.prop</a> Class template <span class=\"tt\">prop</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/prop.hpp\"><span class=\"tt\">prop.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md72", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.env\">exec.env</a> Class template <span class=\"tt\">env</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/env.hpp\"><span class=\"tt\">env.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md73", null ]
        ] ],
        [ "✅✅✅ <a href=\"https://wg21.link/exec.ctx\">exec.ctx</a> Execution contexts", "md_docs_2implementation-status.html#autotoc_md74", [
          [ "✅✅✅ <a href=\"https://wg21.link/exec.run.loop\">exec.run.loop</a> <span class=\"tt\">execution::run_loop</span>", "md_docs_2implementation-status.html#autotoc_md75", [
            [ "✅✅✅ <a href=\"https://wg21.link/exec.run.loop.general\">exec.run.loop.general</a> General: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/run_loop.hpp\"><span class=\"tt\">run_loop.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md76", null ],
            [ "✅✅❎ <a href=\"https://wg21.link/exec.run.loop.types\">exec.run.loop.types</a> Associated types", "md_docs_2implementation-status.html#autotoc_md77", null ],
            [ "✅✅❎ <a href=\"https://wg21.link/exec.run.loop.ctor\">exec.run.loop.ctor</a> Constructor and destructor", "md_docs_2implementation-status.html#autotoc_md78", null ],
            [ "✅✅❎ <a href=\"https://wg21.link/exec.run.loop.members\">exec.run.loop.members</a> Member functions", "md_docs_2implementation-status.html#autotoc_md79", null ]
          ] ]
        ] ],
        [ "✅✅🔴 <a href=\"https://wg21.link/exec.coro.util\">exec.coro.util</a> Coroutine utilities", "md_docs_2implementation-status.html#autotoc_md80", [
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.as.awaitable\">exec.as.awaitable</a> <span class=\"tt\">execution::as_awaitable</span>", "md_docs_2implementation-status.html#autotoc_md81", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.with.awaitable.senders\">exec.with.awaitable.senders</a> <span class=\"tt\">execution::with_awaitable_senders</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/with_awaitable_senders.hpp\"><span class=\"tt\">with_awaitable_senders.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md82", null ],
          [ "✅✅✅ <a href=\"https://wg21.link/exec.affine\">exec.affine</a> <span class=\"tt\">execution::affine</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/affine.hpp\"><span class=\"tt\">affine.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md83", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.inline.scheduler\">exec.inline.scheduler</a> <span class=\"tt\">execution::inline_scheduler</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/inline_scheduler.hpp\"><span class=\"tt\">inline_scheduler.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md84", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.task.scheduler\">exec.task.scheduler</a> <span class=\"tt\">execution::task_scheduler</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/task_scheduler.hpp\"><span class=\"tt\">task_scheduler.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md85", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.task\">exec.task</a> <span class=\"tt\">execution::task</span>", "md_docs_2implementation-status.html#autotoc_md86", [
            [ "✅✅🔴 <a href=\"https://wg21.link/task.overview\">task.overview</a> <span class=\"tt\">task</span> overview", "md_docs_2implementation-status.html#autotoc_md87", null ],
            [ "✅✅🔴 <a href=\"https://wg21.link/task.class\">task.class</a> Class template <span class=\"tt\">task</span>", "md_docs_2implementation-status.html#autotoc_md88", null ],
            [ "✅✅🔴 <a href=\"https://wg21.link/task.members\">task.members</a> <span class=\"tt\">task</span> members", "md_docs_2implementation-status.html#autotoc_md89", null ],
            [ "✅✅🔴 <a href=\"https://wg21.link/task.state\">task.state</a> Class template <span class=\"tt\">task::state</span>", "md_docs_2implementation-status.html#autotoc_md90", null ],
            [ "✅✅🔴 <a href=\"https://wg21.link/task.promise\">task.promise</a> Class template <span class=\"tt\">task::promise_type</span>", "md_docs_2implementation-status.html#autotoc_md91", null ]
          ] ]
        ] ],
        [ "🔴🔴🔴 <a href=\"https://wg21.link/exec.scope\">exec.scope</a> Execution scope utilities", "md_docs_2implementation-status.html#autotoc_md92", [
          [ "✅🔴🔴 <a href=\"https://wg21.link/exec.scope.concepts\">exec.scope.concepts</a> Execution scope concepts", "md_docs_2implementation-status.html#autotoc_md93", null ],
          [ "🔴🔴🔴 <a href=\"https://wg21.link/exec.counting.scopes\">exec.counting.scopes</a> Counting scopes", "md_docs_2implementation-status.html#autotoc_md94", [
            [ "🔴🔴🔴 <a href=\"https://wg21.link/exec.counting.scopes.general\">exec.counting.scopes.general</a> General", "md_docs_2implementation-status.html#autotoc_md95", null ],
            [ "✅✅🔴 <a href=\"https://wg21.link/exec.scope.simple.counting\">exec.scope.simple.counting</a> Simple Counting Scope", "md_docs_2implementation-status.html#autotoc_md96", [
              [ "✅✅🔴 <a href=\"https://wg21.link/exec.scope.simple.counting.general\">exec.scope.simple.counting.general</a> <span class=\"tt\">simple_counting_scope</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/simple_counting_scope.hpp\"><span class=\"tt\">simple_counting_scope.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md97", null ],
              [ "✅✅🔴 <a href=\"https://wg21.link/exec.simple.counting.ctor\">exec.simple.counting.ctor</a> Constructor and Destructor", "md_docs_2implementation-status.html#autotoc_md98", null ],
              [ "✅✅🔴 <a href=\"https://wg21.link/exec.simple.counting.mem\">exec.simple.counting.mem</a> Members", "md_docs_2implementation-status.html#autotoc_md99", null ],
              [ "✅✅🔴 <a href=\"https://wg21.link/exec.simple.counting.token\">exec.simple.counting.token</a> Token", "md_docs_2implementation-status.html#autotoc_md100", null ]
            ] ],
            [ "✅✅🔴 <a href=\"https://wg21.link/exec.scope.counting\">exec.scope.counting</a> Counting Scope", "md_docs_2implementation-status.html#autotoc_md101", null ]
          ] ]
        ] ],
        [ "✅✅🔴 <a href=\"https://wg21.link/exec.par.scheduler\">exec.par.scheduler</a> Parallel scheduler: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/parallel_scheduler.hpp\"><span class=\"tt\">parallel_scheduler.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md102", null ],
        [ "✅✅🔴 <a href=\"https://wg21.link/exec.parschedrepl\">exec.parschedrepl</a> Namespace <span class=\"tt\">parallel_scheduler_replacement</span>", "md_docs_2implementation-status.html#autotoc_md103", [
          [ "❎❎❎ <a href=\"https://wg21.link/exec.parschedrepl.general\">exec.parschedrepl.general</a> General", "md_docs_2implementation-status.html#autotoc_md104", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.parschedrepl.recvproxy\">exec.parschedrepl.recvproxy</a> Receiver proxies: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/parallel_scheduler_replacement.hpp\"><span class=\"tt\">parallel_scheduler_replacement.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md105", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.parschedrepl.query\">exec.parschedrepl.query</a> <span class=\"tt\">query_parallel_scheduler_backend</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/parallel_scheduler_replacement.hpp\"><span class=\"tt\">parallel_scheduler_replacement.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md106", null ],
          [ "✅✅🔴 <a href=\"https://wg21.link/exec.parschedrepl.psb\">exec.parschedrepl.psb</a> Class <span class=\"tt\">parallel_scheduler_backend</span>: <a href=\"https://github.com/bemanproject/execution/blob/main/include/beman/execution/detail/parallel_scheduler_replacement.hpp\"><span class=\"tt\">parallel_scheduler_replacement.hpp</span></a>", "md_docs_2implementation-status.html#autotoc_md107", null ]
        ] ]
      ] ]
    ] ],
    [ "intro_examples", "md_docs_2intro__examples.html", [
      [ "Introduction by Example", "md_docs_2intro__examples.html#autotoc_md108", null ]
    ] ],
    [ "Supporting Modules", "md_docs_2modules.html", [
      [ "History of Adding Module Support", "md_docs_2modules.html#autotoc_md110", [
        [ "Use <span class=\"tt\">export using</span>", "md_docs_2modules.html#autotoc_md111", null ],
        [ "Generate Module Friendly Code: <span class=\"tt\">mk-module.py</span>", "md_docs_2modules.html#autotoc_md112", null ],
        [ "Retry <span class=\"tt\">export using</span>", "md_docs_2modules.html#autotoc_md113", null ],
        [ "<span class=\"tt\">import std;</span>", "md_docs_2modules.html#autotoc_md114", null ]
      ] ],
      [ "Changes Needed to Support Modules", "md_docs_2modules.html#autotoc_md115", null ],
      [ "Scanning and Building", "md_docs_2modules.html#autotoc_md116", null ],
      [ "Modules vs. Testing", "md_docs_2modules.html#autotoc_md117", null ],
      [ "Conclusion", "md_docs_2modules.html#autotoc_md118", null ]
    ] ],
    [ "overview", "md_docs_2overview.html", [
      [ "std::execution Overview", "md_docs_2overview.html#autotoc_md119", [
        [ "Terms", "md_docs_2overview.html#autotoc_md120", null ],
        [ "Concepts", "md_docs_2overview.html#autotoc_md121", null ],
        [ "Queries", "md_docs_2overview.html#autotoc_md122", null ],
        [ "Customization Point Objects", "md_docs_2overview.html#autotoc_md123", null ],
        [ "Senders", "md_docs_2overview.html#autotoc_md124", [
          [ "Sender Factories", "md_docs_2overview.html#autotoc_md125", null ],
          [ "Sender Adaptors", "md_docs_2overview.html#autotoc_md126", null ],
          [ "Sender Consumers", "md_docs_2overview.html#autotoc_md127", null ]
        ] ],
        [ "Helpers", "md_docs_2overview.html#autotoc_md128", null ],
        [ "Stop Token", "md_docs_2overview.html#autotoc_md129", null ],
        [ "Exposition Only", "md_docs_2overview.html#autotoc_md130", null ]
      ] ]
    ] ],
    [ "Questions", "md_docs_2questions.html", null ],
    [ "resources", "md_docs_2resources.html", [
      [ "Resources Related to <span class=\"tt\">beman::execution</span>", "md_docs_2resources.html#autotoc_md132", null ]
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
"md_docs_2implementation-status.html#autotoc_md26",
"structbeman_1_1execution_1_1as__awaitable__t.html"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';