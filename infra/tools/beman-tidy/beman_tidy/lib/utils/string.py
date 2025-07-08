#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import re


def is_snake_case(name):
    return re.match("(^[a-z0-9]+$)|(^[a-z0-9][a-z0-9_.]+[a-z0-9]$)", name)


def is_beman_snake_case(name):
    """
    Has prefix "beman." and continues with snake_case.
    It must NOT end with a C++ target standard version - e.g. 17, 20, 23, 26, 32, etc.
    """

    return (
        name[:6] == "beman."
        and is_snake_case(name[6:])
        and not re.match(".*[0-9]+$", name[6:])
    )


def match_badges(string):
    """
    e.g., ![Library Status](https://raw.githubusercontent.com/bemanproject/beman/refs/heads/main/images/badges/beman_badge-beman_library_under_development.svg) ![Continuous Integration Tests](https://github.com/bemanproject/exemplar/actions/workflows/ci_tests.yml/badge.svg) ![Lint Check (pre-commit)](https://github.com/bemanproject/exemplar/actions/workflows/pre-commit.yml/badge.svg)
    """
    if string is None:
        return None

    badges_str = re.findall(r"!\[[^\]]+\]\([^)]+\)", string)
    return [
        re.match(r"!\[([^\]]+)\]\(([^)]+)\)", badge).groups() for badge in badges_str
    ]


def skip_lines(lines, n):
    return lines[n:] if lines is not None else None


def skip_empty_lines(lines):
    if lines is None:
        return None

    while len(lines) > 0 and len(lines[0].strip()) == 0:
        lines = lines[1:]
    return lines
