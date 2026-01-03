# Questions

This page lists some questions which came up while implementing
[P2300](http://wg21.link/p2300). The current implementation made a
choice to avoid getting blocked on the respective question. Different
implementations may make different choices and the differences are
likely observable.

- [exec.set.value], [exec.set.error], and [exec.set.stopped] don't
    specify whether the operator should be `noexcept`. Choice: make
    the respective operation `noexcept`.
- [exec.general] use "Mandates: err != exception_ptr() is true."
    However, the condition can only be determined at run-time =>
    The "Mandates" should be "Precondition".
- [exec.fwd.env] states "Mandates: ... is a core constant expression if
    `q` is a core constant expression": I don't know how to possibly
    implement that.
- done: [exec.snd.expos] uses query-or-default; it seems only query-with-default
    is defined.
- [exec.snd.concepts] "The type tag_of_t<Sndr> is +defined+ as follows:"
- [exec.sched] uses `auto(get_completion_scheduler<set_value_t>(...))`
    which is OK for clang but doesn't seem to compile for g++ os MSVC.
- [exec.just] p2.1: `movable-value<Ts>` doesn't seems right: `movable-value<decay_t<Ts>>`
- [exec.just] Otherwise after p2.3 is missing <ts...>
- [exec.run.loop.types] p9.1: "refers remains" -> "refers to remains"
- [exec.run.loop.types] p9.2: "get_stop_token(REC(o))": REC is a receiver, any
     environment would be attached to get_env(REC(o)).
- [exec.into.variant] p2: the sender argument to into_variant really needs to
    have completion_signatures defined - should the constraint be sender_in
    instead of sender? oh, actually, only the result of transform_sender needs
    to be a sender_in!
- [exec.sync.wait] p4.2: what is "e"? probably apply_sender(...)
- [exec.sync.wait] p9 seems to be wrong: the sender needs to be scheduled
    somehow. Consider using sync_wait(execution::just()). The operation
    immediately completes, calling into sync_wait_receiver::set_value which
    in turn calls loop.finish(). When the loop then is run() it immediately
    std::terminate()s because the run_loop's state is finished not starting.
    I can see two potential fixes:
    1. connect(on(sender), `sync-wait-receiver<Sndr>{&state})`
    2. have run_loop::run() only terminate when the state is running; if
        the state is already finishing, leave it at that
- [exec.sync_wait] are sync_wait(just_error(17)) and sync_wait(just_stopped())
    supposed to work? I don't see a reason why a value completion is necessarily
    required. As is, they are not because type_idenity_t ends up being used without
    argument.
- [exec.then] p5 seems to miss a space between sendr and out_sndr
- [exec.let] p9: Env is not defined; it is probably meant to be env_t
- [exec.schedule.from] p6: the Rcvr in state-type shouldn't be a reference
- [exec.when.all] uses on-stop-request without saying what it actually does.
    most likely on-stop-request is supposed to call stop_src.request_stop
- [exec.when.all] p11.1 uses a non-existing conversion for tuple to implement its "tie"
    actually, that one may be new in C++23!
