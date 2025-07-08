#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import pytest
from pathlib import Path

from tests.utils.path_runners import (
    run_check_for_each_path,
    run_fix_inplace_for_each_file_path,
)

# Actual tested checks.
from beman_tidy.lib.checks.beman_standard.readme import (
    ReadmeTitleCheck,
    ReadmeBadgesCheck,
    ReadmeLibraryStatusCheck,
)

test_data_prefix = "tests/lib/checks/beman_standard/readme/data"
valid_prefix = f"{test_data_prefix}/valid"
invalid_prefix = f"{test_data_prefix}/invalid"


def test__README_TITLE__valid(repo_info, beman_standard_check_config):
    """
    Test that a valid README.md title passes the check.
    """
    valid_readme_paths = [
        # Title: # beman.exemplar: A Beman Library Exemplar
        Path(f"{valid_prefix}/README-v1.md"),
        # Title: # beman.exemplar: Another Beman Library
        Path(f"{valid_prefix}/README-v2.md"),
        # Title: # beman.exemplar: Awesome Beman Library
        Path(f"{valid_prefix}/README-v3.md"),
        # Title: # beman.exemplar: The Most Awesome Beman Library
        Path(f"{valid_prefix}/README-v4.md"),
    ]

    run_check_for_each_path(
        True,
        valid_readme_paths,
        ReadmeTitleCheck,
        repo_info,
        beman_standard_check_config,
    )


def test__README_TITLE__invalid(repo_info, beman_standard_check_config):
    """
    Test that an invalid README.md title fails the check.
    """
    invalid_readme_paths = [
        Path(f"{invalid_prefix}/invalid.md"),
        Path(f"{invalid_prefix}/invalid-title-v1.md"),
        Path(f"{invalid_prefix}/invalid-title-v2.md"),
        Path(f"{invalid_prefix}/invalid-title-v3.md"),
        Path(f"{invalid_prefix}/invalid-title-v4.md"),
    ]

    run_check_for_each_path(
        False,
        invalid_readme_paths,
        ReadmeTitleCheck,
        repo_info,
        beman_standard_check_config,
    )


def test__README_TITLE__fix_inplace(repo_info, beman_standard_check_config):
    """
    Test that the fix method corrects an invalid README.md title.
    """
    invalid_readme_paths = [
        Path(f"{invalid_prefix}/invalid-title-v1.md"),
        Path(f"{invalid_prefix}/invalid-title-v2.md"),
        Path(f"{invalid_prefix}/invalid-title-v3.md"),
        Path(f"{invalid_prefix}/invalid-title-v4.md"),
    ]

    run_fix_inplace_for_each_file_path(
        invalid_readme_paths, ReadmeTitleCheck, repo_info, beman_standard_check_config
    )


def test__README_BADGES__valid(repo_info, beman_standard_check_config):
    """
    Test that a valid README.md badges passes the check.
    """
    valid_readme_paths = [
        Path(f"{valid_prefix}/README-v1.md"),
        Path(f"{valid_prefix}/README-v2.md"),
        Path(f"{valid_prefix}/README-v3.md"),
        Path(f"{valid_prefix}/README-v4.md"),
    ]

    run_check_for_each_path(
        True,
        valid_readme_paths,
        ReadmeBadgesCheck,
        repo_info,
        beman_standard_check_config,
    )


def test__README_BADGES__invalid(repo_info, beman_standard_check_config):
    """
    Test that an invalid README.md badges fails the check.
    """
    invalid_readme_paths = [
        Path(f"{invalid_prefix}/invalid.md"),
        Path(f"{invalid_prefix}/invalid-badge-v1.md"),
        Path(f"{invalid_prefix}/invalid-badge-v2.md"),
        Path(f"{invalid_prefix}/invalid-badge-v3.md"),
    ]

    run_check_for_each_path(
        False,
        invalid_readme_paths,
        ReadmeBadgesCheck,
        repo_info,
        beman_standard_check_config,
    )


@pytest.mark.skip(reason="NOT implemented")
def test__README_BADGES__fix_inplace(repo_info, beman_standard_check_config):
    """
    Test that the fix method corrects an invalid README.md badges.
    """
    pass


def test__README_LIBRARY_STATUS__valid(repo_info, beman_standard_check_config):
    """
    Test that a valid README.md library status passes the check.
    """
    valid_readme_paths = [
        Path(f"{valid_prefix}/README-v1.md"),
        Path(f"{valid_prefix}/README-v2.md"),
        Path(f"{valid_prefix}/README-v3.md"),
        Path(f"{valid_prefix}/README-v4.md"),
    ]

    run_check_for_each_path(
        True,
        valid_readme_paths,
        ReadmeLibraryStatusCheck,
        repo_info,
        beman_standard_check_config,
    )


def test__README_LIBRARY_STATUS__invalid(repo_info, beman_standard_check_config):
    """
    Test that an invalid README.md library status fails the check.
    """
    invalid_readme_paths = [
        Path(f"{invalid_prefix}/invalid.md"),
        Path(f"{invalid_prefix}/invalid-status-line-v1.md"),
        Path(f"{invalid_prefix}/invalid-status-line-v2.md"),
        Path(f"{invalid_prefix}/invalid-status-line-v3.md"),
    ]

    run_check_for_each_path(
        False,
        invalid_readme_paths,
        ReadmeLibraryStatusCheck,
        repo_info,
        beman_standard_check_config,
    )


@pytest.mark.skip(reason="NOT implemented")
def test__README_LIBRARY_STATUS__fix_inplace(repo_info, beman_standard_check_config):
    """
    Test that the fix method corrects an invalid README.md library status.
    """
    pass
