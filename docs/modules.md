# Supporting Modules

Author: Dietmar Kühl dietmar.kuehl@me.com

Originally [`beman.execution`](https://github.com/bemanproject/execution)
was implemented with out `module` support. Eventually, `module`
support in all compilers and in [`cmake`](https://cmake.org/) made
it reaonable to add `module` support. This document describes some
of the experiences of the journey adding `module` support for
[`beman.execution`](https://github.com/bemanproject/execution). It is
likely that my attempts on how to support `module`s were misguided and
I'm happy to learn how things can be done properly or better.

## History of Adding Module Support

First, let's start describing what I did. The starting point for
that was a working
[`beman.execution`](https://github.com/bemanproject/execution)
implementation which had no `module` support. It had tests and some
examples and the `CMakeLists.txt` built these. The default build
procedure run from the `Makefile` was already set up to use `ninja`.

Claus Klein had started to land `cmake` support fo `module`s. In
particular there were `cmake` rules to detect whether there is a
dependency scanner and whether `import std;` is supported. The
statement from Claus was that using `import std;` would be the
second step - the first step is to actually create a `module` for
[`beman.execution`](https://github.com/bemanproject/execution)
including the standard library headers.

### Use `export using`

I had tried to create a `module` right after the Hagenberg meeting
at the end of 2025. Since I was clueless how to go about that I had
asked peolple who worked on the `module` specification and implemented
the corresponding support in compilers. The recommendation was to
create a `module` file, include all the headers, start the `module`,
and have `export using <name>;` declarations. That is, something
like this:

```c++
module;

#include <beman/execution/execution.hpp>

export module beman.execution;

namespace beman::execution {
    export using ::beman::execution::forwarding_query_t;
    // more of these - one for each name to be exported
}
```

Creating a file like that was fairly mechanical work and I just did
it.  I tried it. There were a bunch of errors which I could work
out easily but eventually I got stuck with unscruitible errors.
Since I never planned to write about the experience I don't recall
what the errors were.  When I asked about it the message was that
neither `cmake` nor the compilers are quite there. So I abondoned
this first attempt.

Time passed and in the beginning of 2026 some `cmake` support for
`module`s was added via a PR to the repository. The code wasn't
ready to use `module` but it seems reasonable to retry. A first
feeble try to get things built using the `export using` approach
again resulted in errors and there were voice stating that this
won't work but `module` support should actually work. The error
messages seemed to imply that I should `export` the declarations
immediately. So I abandoned that particular approach, again.

### Generate Module Friendly Code: `mk-module.py`

The attempts up to this point had indicated a few things which
were somewhat misaligned with the way the code in 
[`beman.execution`](https://github.com/bemanproject/execution)
is laid out. The code is structured into lots of small _components_
(loosely based on John Lakos's idea of components in his 1996 version
of "Large-Scale C++"). Each component consists of

<ul>
  <li>
  a header declaring an entity like a class/class template, a
  function/function template, or a concept as well as everything
  needed to do so;
  </li>
  <li>
  a test file verifying that everything promised is, indeed, defined
  and confirm using tests that it works;
  </li>
  <li>
  an optional source file with the definitions of what is declared
  in the header; in case of templates the header will actually
  contain these definitions
  </li>
</ul>

Each component's file includes all necessary headers and a component
`A` is said to _directly depend on a component_ `D` if any of `A`'s
files includes `D`'s header. Creating a graph with the components
as nodes and directed edges from each component to all components
it directly depends on results in dependency graph which does not
contain any cycles. Building the code without `module`s just works
fine.

When trying directly `export` the declarations of names when they
are first declared, it quickly transpired that this doesn't work
due to the structure of `module` files required by contemporary
compilers:

<ol>
  <li>
  The first 7 charactesr of a `module` file shall be `module;`
  without anything preceding them.  [`g++`](https://gcc.gnu.org/)
  and [`clang++`](https://llvm.org/) are somewhat relaxed about
  this requirement but some compiler is rather strict. This, however,
  is at least easily achieved.
  </li>
  <li>
  Any `export` of a name has to follow the `module`'s name declaration,
  in this case after `export module beman.execution;`. This makes
  sense: the compiler needs to know what `module` the names belong to.
  </li>
  <li>
  The problem comes with any standard library header included after
  this name declaration: that entirely confuses the compilers. That
  is, all headers really need to be included before the name
  declaration. That is pretty much _not_ how the components in
  [`beman.execution`](https://github.com/bemanproject/execution) are
  organized.
  </li>
</ol>

To still achieve the objective of `export`ing a name when it first
gets declared, the structure needs to be change. However, the componets
are organized in a consistent structure. So the idea is to use this structure
to reorganize the files for `module` builds:

<ol>
  <li>
  Add an `export` keyword in front of any declaration which should be
  `export`ed (well, really a name which can be defined so the headers can
  function both when building a `module` and when just using headers; the
  implementation uses `BEMAN_EXECUTION_EXPORT`).
  </li>
  <li>
  Use a script (named
  [`bin/mk-module.py`](https://github.com/bemanproject/execution/blob/main/bin/mk-module.py))
  to create a `module` definition:
    <ol>
      <li>
      Start the file with `module;` (and some header stating the
      file is generated).
      </li>
      <li>
      Create a list of all used standard library headers and put
      these right below the files head.
      </li>
      <li>
      Add the `module` name declaration.
      </li>
      <li>
      Copy the declaration from all the components in correct
      dependency order, i.e., each component's declarations is
      preceded by the declarations it directly depends on.
      </li>
    </ol>
  </li>
  <li>Profit!</li>
</ol>

Creating the script to write the file was reasonably straight forward
although I spend way too much time making it fancy and include
suitable `#line` directives to find the actual source. Compiling
the resulting still didn't quite work, of course. There was a bunch
of silly errors in the component headers which could be quickly
resolved, though. That wasn't quite as true for the test files,
though (more [ruminations on tests](#modules-vs-testing) below):

- Many tests didn't include all standard library headers they
  dependent on. Since the corresponding header were actually included
  by a component header things still worked. So, the corresponding
  headers needed to be added.
- Instead of `#include <beman/execution/execution.hpp>` the tests
  now use `import beman.execution;` (well, the test really use
  conditional compilation to either use a header or an `import`
  statement). Including any standard library header _after_ the
  `import` statement again confuses the compiler, i.e., some
  reordering in the files was needed: the test files deliberately
  included the component's header first (to make sure all needed
  headers are included by the component header) but this include
  statement is now replaced by the `import` statement.
- The tests actually use some of the implementation-defined entities
  which were not meant to be `export`ed. To still have these tests
  I ended up `export`ing the necessary implementation-defined names.
  That needs to be corrected eventually (assuming that is actually
  possible which isn't quite as clear).
- Of course, the tests actually used the various names and it turned
  out that quite a few names, e.g., the `operator|`, were missing.

That worked OK with one compiler. Then I tried a different compiler
and lots of issues emerged:

- More names needed to be `export`ed for the tests.
- Some things just didn't compile at all and needed to be changed
  (I managed to avoid the problems but I haven't quite understood why).
- Symbols were undefined.

I ended up spending quite a bit of time reshuffling where headers
go, fixing some actual bugs, and working around what looks like
compiler problems. Most of that was, however, fairly mechanical
and eventually I got a `module` declaration working with all major
C++ compilers (using recent versions of each).

-dk:TODO `export using`
-dk:TODO `import std;`

## Changes Needed to Support Modules

-dk:TODO order of headers vs. import
-dk:TODO fixes for join_env
-dk:TODO requires declaration of variant, optional, etc.

## Scanning

## Modules vs. Testing

