<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->
# beman.execution: Asynchronous Programming Foundation

![Library Status](https://raw.githubusercontent.com/bemanproject/beman/refs/heads/main/images/badges/beman_badge-beman_library_under_development.svg) ![Continuous Integration Tests](https://github.com/bemanproject/execution/actions/workflows/ci_tests.yml/badge.svg) ![Lint Check (pre-commit)](https://github.com/bemanproject/execution/actions/workflows/pre-commit.yml/badge.svg)
[![Compiler Explorer Example](https://img.shields.io/badge/Try%20it%20on%20Compiler%20Explorer-grey?logo=compilerexplorer&logoColor=67c52a)](https://godbolt.org/z/1narY8cra)

`beman.execution` provides vocabulary and algorithms for asynchronous programs.

**Implements:** [`std::execution` (P2300R10)](http://wg21.link/P2300R10).

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/BEMAN_LIBRARY_MATURITY_MODEL.md#under-development-and-not-yet-ready-for-production-use)

## Description

`beman.execution` provides the basic vocabulary for asynchronous
programming as well as important algorithms implemented in terms
of this vocabulary. The key entities of the vocabulary are:

- `scheduler` used to control where operations are executed.
    - The key operation is `schedule(scheduler) -> sender`.
- `sender` used to represent work.
    - The key operation is `connect(sender, receiver) -> operation-state`.
- `receiver` used to receive completion notifications.
    - There are multiple operations on receivers:
        - `set_value(receiver, args...)` for successful completions.
        - `set_error(receiver, error)` to report errors.
        - `set_stopped(receiver)` to report cancellation.
- `operation-state` to represent a ready to run work graph .
    - The key operation is `start(state)`.

Using these operations some fundamental algorithms are implemented,
e.g.:

- `just(args...)` as the starting point of a work graph without a
    specified scheduler`.
- `let_value(sender, fun)` to produce a sender based on `sender`'s
    results.
- `on(scheduler, sender)` to execute `sender` on `scheduler`.
- `transfer(sender, scheduler)` to complete with with `sender`'s
    results on `scheduler`.
- `when_all(sender ...)` to complete when all `sender`s have
    completed.
- `bulk(...)` to executed execute work, potentially concurrently.

## Help Welcome!

There are plenty of things which need to be done. See the
[contributions page](https://github.com/bemanproject/execution/blob/main/docs/contributing.md)
for some ideas how to contribute. The [resources page](https://github.com/bemanproject/execution/blob/main/docs/resources.md)
contains some links for general information about the sender/receivers and `std::execution`.

## Build

| Library | Linux | MacOS | Windows |
| ------- | ----- | ----- | ------- |
| build | ![Linux build status](https://github.com/bemanproject/execution/actions/workflows/linux.yml/badge.svg) | ![MacOS build status](https://github.com/bemanproject/execution/actions/workflows/macos.yml/badge.svg) | ![Window build status](https://github.com/bemanproject/execution/actions/workflows/windows.yml/badge.svg) |

The following instructions build the library and the examples:

    cmake --workflow --list-presets
    Available workflow presets:

      "debug"
      "release"

    cmake --workflow --preset release

The implementation compiles and passes tests using [clang](https://clang.llvm.org/),
[gcc](http://gcc.gnu.org), and [MSVC++](https://visualstudio.microsoft.com/vs/features/cplusplus/).

## Examples

- `<stop_token>` example: [Compiler Explorer](https://godbolt.org/z/4r4x9q1r7)
