# beman-tidy: The Codebase Bemanification Tool

<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

## Description

`beman-tidy` is a tool used to check and apply
[The Beman Standard](https://github.com/bemanproject/beman/blob/main/docs/BEMAN_STANDARD.md).

Purpose: The tool is used to `check` (`--dry-run`) and `apply` (`--fix-inplace`) the Beman Standard to a repository.
Note: `2025-06-07`: In order to make the best and quickly use of the tool in the entire organization, most of the
checks will not support the `--fix-inplace` flag in the first iteration.

## Installation

- The current recommended workflow relies on [Astral's uv](https://docs.astral.sh/uv/)
- However, we provide a [PEP 751](https://peps.python.org/pep-0751/) `pylock.toml`, so don't feel forced to use uv
- You can use beman-tidy as a pre-commit hook or install it on your system using `pipx`

```shell
uv build
pipx install path/to/wheel
```

<details>
<summary>beman-tidy: Full example - build and install</summary>

```shell
$ uv build
Building source distribution...
Building wheel from source distribution...
Successfully built dist/beman_tidy-0.1.0.tar.gz
Successfully built dist/beman_tidy-0.1.0-py3-none-any.whl

$ pipx install dist/beman_tidy-0.1.0-py3-none-any.whl
Installing to existing venv 'beman-tidy'
  installed package beman-tidy 0.1.0, installed using Python 3.13.4
  These apps are now globally available
    - beman-tidy
...
You will need to open a new terminal or re-login for the PATH changes to take effect. Alternatively, you can source your shell's config file with e.g. 'source ~/.bashrc'.

$ beman-tidy --help
usage: beman-tidy [-h] [--fix-inplace | --no-fix-inplace] [--verbose | --no-verbose] [--checks CHECKS] repo_path
...
```

</details>

## Usage

- Display help:

```shell
$ uv run beman-tidy --help
usage: beman-tidy [-h] [--fix-inplace | --no-fix-inplace] [--verbose | --no-verbose] [--require-all | --no-require-all] [--checks CHECKS] repo_path

positional arguments:
  repo_path             path to the repository to check

options:
  -h, --help            show this help message and exit
  --fix-inplace, --no-fix-inplace
                        Try to automatically fix found issues
  --verbose, --no-verbose
                        print verbose output for each check
  --require-all, --no-require-all
                        all checks are required regardless of the check type (e.g., RECOMMENDATION becomes REQUIREMENT)
  --checks CHECKS       array of checks to run
```

- Run beman-tidy on the exemplar repository **(default: dry-run mode)**

```shell
# dry-run, require-all, non-verbose
$ uv run beman-tidy /path/to/exemplar --require-all
Summary    REQUIREMENT:  1 checks PASSED, 0 checks FAILED, 4 skipped (NOT implemented).
Summary RECOMMENDATION:  2 checks PASSED, 1 checks FAILED, 35 skipped (NOT implemented).

Coverage    REQUIREMENT: 100.0% (1/1 checks passed).
Coverage RECOMMENDATION: 66.67% (2/3 checks passed).

# dry-run, non-require-all, non-verbose
$ uv run beman-tidy /path/to/exemplar
Summary    REQUIREMENT:  1 checks PASSED, 0 checks FAILED, 4 skipped (NOT implemented).
Summary RECOMMENDATION:  2 checks PASSED, 1 checks FAILED, 35 skipped (NOT implemented).

Coverage    REQUIREMENT: 100.0% (1/1 checks passed).
Note: RECOMMENDATIONs are not included (--require-all NOT set).

```

or verbose mode:

```shell
# dry-run, require-all, verbose mode - no errors
$ uv run beman-tidy /path/to/exemplar --require-all --verbose
beman-tidy pipeline started ...

Running check [RECOMMENDATION][README.TITLE] ...
    check [RECOMMENDATION][README.TITLE] ... PASSED

Running check [REQUIREMENT][README.BADGES] ...
    check [REQUIREMENT][README.BADGES] ... PASSED

Running check [RECOMMENDATION][README.LIBRARY_STATUS] ...
    check [RECOMMENDATION][README.LIBRARY_STATUS] ... PASSED

Running check [RECOMMENDATION][DIRECTORY.SOURCES] ...
[WARNING        ][DIRECTORY.SOURCES        ]: The directory '/Users/dariusn/dev/dn/git/Beman/exemplar/src/beman/exemplar' does not exist.
    check [RECOMMENDATION][DIRECTORY.SOURCES] ... FAILED


beman-tidy pipeline finished.

Summary    REQUIREMENT:  1 checks PASSED, 0 checks FAILED, 4 skipped (NOT implemented).
Summary RECOMMENDATION:  2 checks PASSED, 1 checks FAILED, 35 skipped (NOT implemented).

Coverage    REQUIREMENT: 100.0% (1/1 checks passed).
Coverage RECOMMENDATION: 66.67% (2/3 checks passed).
```

```shell
# dry-run, require-all, verbose mode - no errors
$ uv run beman-tidy /path/to/exemplar --require-all --verbose
beman-tidy pipeline started ...

Running check [RECOMMENDATION][README.TITLE] ...
    check [RECOMMENDATION][README.TITLE] ... PASSED

Running check [REQUIREMENT][README.BADGES] ...
    check [REQUIREMENT][README.BADGES] ... PASSED

Running check [RECOMMENDATION][README.LIBRARY_STATUS] ...
    check [RECOMMENDATION][README.LIBRARY_STATUS] ... PASSED

Running check [RECOMMENDATION][DIRECTORY.SOURCES] ...
    check [RECOMMENDATION][DIRECTORY.SOURCES] ... PASSED


beman-tidy pipeline finished.

Summary    REQUIREMENT:  1 checks PASSED, 0 checks FAILED, 4 skipped (NOT implemented).
Summary RECOMMENDATION:  3 checks PASSED, 0 checks FAILED, 35 skipped (NOT implemented).

Coverage    REQUIREMENT: 100.0% (1/1 checks passed).
Coverage RECOMMENDATION: 100.0% (3/3 checks passed).
```

- Run beman-tidy on the exemplar repository (fix issues in-place):

```shell
uv run beman-tidy path/to/exemplar --fix-inplace --verbose
```

## beman-tidy Development

Please refer to the [Beman Tidy Development Guide](./docs/dev-guide.md) for more details.
