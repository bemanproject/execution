#!/usr/bin/python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import re
import subprocess

deps = {}

done_re = re.compile("^(?P<state>.) (?P<name>.*)")
with open("deps") as file:
    for line in file.readlines():
        match = done_re.match(line)
        if match:
            deps[match.group("name")] = match.group("state")

result = subprocess.run(
    ["/usr/bin/tsort", "docs/dependency.txt"], capture_output=True, encoding="utf-8"
)
for line in result.stdout.split("\n"):
    if line != "":
        print(f'{deps[line] if line in deps else "-"} {line}')
