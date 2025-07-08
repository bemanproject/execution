# Beman Project Reusable Github Actions Repository

<!-- SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception -->

This repository contains [reusable GitHub Actions
workflows](https://docs.github.com/en/actions/how-tos/sharing-automations/reusing-workflows)
workflow files, intended to help unify the GitHub Actions machinery used across Beman
repositories for CI. It contains the following reusable workflows:

## `reusable-beman-build-and-test.yml`

This is the main workflow file used for CI. It takes in a JSON build configuration like
the following example:

```json
{
  "gcc": [
    { "versions": ["15"],
      "tests": [
        { "cxxversions": ["c++26"],
          "tests": [
            { "stdlibs": ["libstdc++"],
              "tests": [
                "Debug.Default", "Release.Default", "Debug.TSan", "Debug.MaxSan",
                "Debug.Werror", "Debug.Dynamic"
              ]
            }
          ]
        },
        { "cxxversions": ["c++23", "c++20", "c++17"],
          "tests": [{ "stdlibs": ["libstdc++"], "tests": ["Release.Default"]}]
        }
      ]
    },
  "clang-p2996": [
    { "versions": ["trunk"],
      "tests": [
        { "cxxversions": ["c++26"],
          "tests": [{"stdlibs": ["libc++"], "tests": ["Release.-DCMAKE_CXX_FLAGS='-freflection-latest'"]}]
        }
      ]
    }
  ]
}
```

It then runs jobs corresponding to the specified set of configurations.

## `reusable-beman-create-issue-when-fault.yml`

This workflow is intended to help with projects that invoke CI on a scheduled basis when
those jobs fail. It creates a GitHub issue describing the CI failure.

## `reusable-beman-preset-test.yml`

This workflow is intended to ensure that the CMake presets provided by beman/infra are
valid and working for the given repository. It takes in a JSON build configuration like
the following:

```json
[
  {"preset": "gcc-debug", "image": "ghcr.io/bemanproject/infra-containers-gcc:latest"},
  {"preset": "gcc-release", "image": "ghcr.io/bemanproject/infra-containers-gcc:latest"},
  {"preset": "llvm-debug", "image": "ghcr.io/bemanproject/infra-containers-clang:latest"},
  {"preset": "llvm-release", "image": "ghcr.io/bemanproject/infra-containers-clang:latest"},
  {"preset": "appleclang-debug", "runner": "macos-latest"},
  {"preset": "appleclang-release", "runner": "macos-latest"},
  {"preset": "msvc-debug", "runner": "windows-latest"},
  {"preset": "msvc-release", "runner": "windows-latest"}
]
```

It then runs jobs corresponding to the specified set of presets.

## `reusable-beman-pre-commit.yml`

This provides a workflow for running the
[pre-commit](https://github.com/pre-commit/pre-commit) checks Beman libraries use, on pull
requests and on push.

## `reusable-beman-submodule-check.yml`

This provides a workflow for checking consistency of
[`beman-submodule`](https://github.com/bemanproject/infra/blob/main/tools/beman-submodule/README.md)
directories used by Beman repositories to deduplicate infrastructure.
