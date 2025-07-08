# beman-submodule

<!-- SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception -->

## What is this script?

`beman-submodule` provides some of the features of `git submodule`, adding child git
repositories to a parent git repository, but unlike with `git submodule`, the entire child
repo is directly checked in, so only maintainers, not users, need to run this script. The
command line interface mimics `git submodule`'s.

## How do I add a beman submodule to my repository?

The first beman submodule you should add is this repository, `infra/`, which you can
bootstrap by running:

<!-- markdownlint-disable MD013 -->
```sh
curl -s https://raw.githubusercontent.com/bemanproject/infra/refs/heads/main/tools/beman-submodule/beman-submodule | python3 - add https://github.com/bemanproject/infra.git
```

Once that's added, you can run the script from `infra/tools/beman-submodule/beman-submodule`.

## How do I update a beman submodule to the latest trunk?

You can run `beman-submodule update --remote` to update all beman submodule to latest
trunk, or e.g. `beman-submodule update --remote infra` to update only a specific one.

## How does it work under the hood?

Along with the files from the child repository, it creates a dotfile called
`.beman_submodule`, which looks like this:

```ini
[beman_submodule]
remote=https://github.com/bemanproject/infra.git
commit_hash=9b88395a86c4290794e503e94d8213b6c442ae77
```

## How do I update a beman submodule to a specific commit or change the remote URL?

You can edit the corresponding lines in the `.beman_submodule` file and run
`beman-submodule update` to update the state of the beman submodule to the new
`.beman_submodule` settings.

## How can I make CI ensure that my beman submodules are in a valid state?

Add this job to your CI workflow:

```yaml
  beman-submodule-test:
    runs-on: ubuntu-latest
    name: "Check beman submodules for consistency"
    steps:
      - name: Checkout
        uses: actions/checkout@v4
      - name: beman submodule consistency check
        run: |
          (set -o pipefail; ./infra/tools/beman-submodule/beman-submodule status | grep -qvF '+')
```

This will fail if the contents of any beman submodule don't match what's specified in the
`.beman_submodule` file.
