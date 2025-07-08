# Beman Tidy Development Guide

## Tree structure

* `README.md`: The public documentation for the `beman-tidy` tool.
* `docs/`: The internal documentation.
* `beman_tidy/`: The package/production code for the tool.
  * `beman_tidy/cli.py`: The CLI / entry point for the tool.
  * `beman_tidy/lib/`: The library for the tool.
    * `beman_tidy/lib/checks/`: The checks for the tool.
    * `beman_tidy/lib/pipeline.py`: The checks pipeline for the `beman-tidy` tool.
  * `beman_tidy/.beman-standard.yml`: Stable (offline) version of the standard.
* `tests/`: Unit tests for the tool.
  * Structure is similar to the `beman_tidy/` directory.
  * `pytest` is used for testing.

## Adding a new check

Find an unimplemented check in the [BEMAN_STANDARD.md](https://github.com/bemanproject/beman/blob/main/docs/BEMAN_STANDARD.md) file and check that is not already assigned in [Planning for beman-tidy: The Codebase Bemanification Tool](https://github.com/orgs/bemanproject/projects/8/views/1).


Check this PR example: [beman-tidy: add check - README.LIBRARY_STATUS](https://github.com/bemanproject/infra/pull/35).

<details>
<summary>Step by step tutorial: add a new check</summary>

* `[mandatory]` Make sure `beman_tidy/.beman-standard.yml` reflects your check metadata (latest status from [BEMAN_STANDARD.md](https://github.com/bemanproject/beman/blob/main/docs/BEMAN_STANDARD.md)).
  * `[optional]` New syntax / keys from yml config can be added in
    [infra/tools/beman-tidy/beman_tidy/lib/utils_git.py:load_beman_standard_config()](https://github.com/bemanproject/infra/blob/main/tools/beman-tidy/beman_tidy/lib/utils/git.py)
    if not already implemented. Checks for TODOs in `load_beman_standard_config()`.
* `[mandatory]` Add the check to the `beman_tidy/lib/checks/beman_standard/` directory.
  * `[mandatory]` e.g., `README.*` checks will most likely go to a path similar to `beman_tidy/lib/checks/beman_standard/readme.py`.
  * `[mandatory]` Use an appropriate base class - e.g., defaults like `FileBaseCheck` / `DirectoryBaseCheck` or create
    specializations for reusing code - e.g.,  `ReadmeBaseCheck(FileBaseCheck)` / `CmakeBaseCheck(FileBaseCheck)` /
    `CppBaseCheck(FileBaseCheck)` etc.
  * `[mandatory]` Register the new check via `@register_beman_standard_check` decorator - e.g.,

    ```python
    @register_beman_standard_check("README.TITLE")
    class ReadmeTitleCheck(ReadmeBaseCheck):
    ```

* `[mandatory]` Add tests for the check to the `tests/beman_standard/` directory. More in [Writing Tests](#writing-tests).
* `[optional]` Updates docs if needed in `README.md` and `docs/dev-guide.md` files.
* `[optional]` Update the `beman_tidy/cli.py` file if the public API has changed.

</details>


## Linting

Run the linter on the beman-tidy's codebase:

```shell
uv run ruff check --diff
uv run ruff check --fix
```

## Testing

### Running Tests

Run the tests:

```shell
$ uv run pytest
================================================================================================================ test session starts ================================================================================================================
platform darwin -- Python 3.14.0b2, pytest-8.4.0, pluggy-1.6.0 -- /Users/dariusn/dev/dn/git/Beman/infra/tools/beman-tidy/.venv/bin/python
cachedir: .pytest_cache
rootdir: /Users/dariusn/dev/dn/git/Beman/infra/tools/beman-tidy
configfile: pyproject.toml
collected 6 items

tests/beman_standard/readme/test_readme.py::test__README_TITLE__valid PASSED                                                                                                                                                                  [ 16%]
tests/beman_standard/readme/test_readme.py::test__README_TITLE__invalid PASSED                                                                                                                                                                [ 33%]
tests/beman_standard/readme/test_readme.py::test__README_TITLE__fix_inplace PASSED                                                                                                                                                            [ 50%]
tests/beman_standard/readme/test_readme.py::test__README_BADGES__valid PASSED                                                                                                                                                                 [ 66%]
tests/beman_standard/readme/test_readme.py::test__README_BADGES__invalid PASSED                                                                                                                                                               [ 83%]
tests/beman_standard/readme/test_readme.py::test__README_BADGES__fix_inplace SKIPPED (NOT implemented)                                                                                                                                        [100%]

=========================================================================================================== 5 passed, 1 skipped in 0.07s ============================================================================================================
```

### Writing Tests

* `tests/lib/checks/beman_standard/<check_category>/test_<check_category>.py`: The test file for the `<check_category>`
  check.
  * e.g., for `check_category = "readme"` the test file is `tests/lib/checks/beman_standard/readme/test_readme.py`.
* `test__<check_category>__<test_case_name>()` function inside the test file.
  * e.g., for `check_category = "readme"` and `test_case_name = "valid"` the function is `test__README_TITLE__valid()`.
  * e.g., for `check_category = "readme"` and `test_case_name = "invalid"` the function is
    `test__README_TITLE__invalid()`.
* `tests/beman_standard/<check_category>/data/`: The data for the tests (e.g., files, directories, etc.).
  * e.g., for `check_category = "readme"` and `test_case_name = "valid"` the data is in
    `tests/lib/checks/beman_standard/readme/data/valid/`.
  * e.g., for `check_category = "readme"` and `test_case_name = "invalid"` the data is in
    `tests/lib/checks/beman_standard/readme/data/invalid/`.
  * e.g., for `check_category = "readme"` and `test_case_name = "fix_inplace"` the data may use both `valid` and
    `invalid` files. It is recommended to not change these files and use temporary copies having suffix `.delete_me`
    (which are not tracked by git).
* Default setup / mocks:
  * `repo_info`: The repository information (e.g., path, name, etc.). Mocked with hardcoded values of `beman.exemplar`.
  * `beman_standard_check_config`: The Beman Standard configuration file. Actual load of the `.beman-standard.yml`
    file.
* Always add at least 3 test cases for each check.
  * `valid`: The test case for the valid case.
  * `invalid`: The test case for the invalid case.
  * `fix_inplace`: The test case for the fix invalid case. If the fix is not (yet) implementable, add a
    `@pytest.mark.skip(reason="NOT implemented")` decorator to track the progress.

## Changing dependencies

* Add / update the dependency to the `pyproject.toml` file.
* Run `uv clean` to make sure the dependencies are updated.
* Run `uv sync` to update the uv lockfile
* Run `uv export -o pylock.toml` to update `pylock.toml`
* Run `uv build` to build the wheel.
* Run `uv run beman-tidy --help` to check if the new dependency is available.
* Commit the changes from `pyproject.toml`, `pylock.toml` and `uv.lock`.

## Development Notes

Requirements:

* `beman-tidy` must be able to run on Windows, Linux, and macOS, thus it's 100% Python.
* `beman-tidy` must NOT use internet access.  A local snapshot of the standard is used (check `.beman-standard.yml`).
* `beman-tidy` must have `verbose` and `non-verbose` modes. Default is `non-verbose`.
* `beman-tidy` must have `dry-run` and `fix-inplace` modes. Default is `dry-run`.
* `beman-tidy` must detect types of checks: failed, passed, skipped (not implemented) and print the summary/coverage.

Limitations:

* `2025-06-07`: `beman-tidy` will not support the `--fix-inplace` flag in the first iteration for most of the checks.
* `2025-06-07`: `beman-tidy` may generate small changes to the standard (e.g., for automated fixes), while the standard
  is not stable. Thus, the tool itself may be unstable.
