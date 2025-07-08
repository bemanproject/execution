#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import os
from pathlib import Path


def run_check_for_each_path(
    expected_result, paths, check_class, repo_info, beman_standard_check_config
):
    """
    Run path-based check (check_class) for each given path: evaluate check_class(paths[i]).

    Example:
        expected_result = True / False
        paths = [
            "tests/lib/checks/beman_standard/readme/data/valid/README-v1.md",
            "tests/lib/checks/beman_standard/readme/data/valid/README-v2.md",
        ]
        check_class = ReadmeTitleCheck or DirectorySourcesCheck
        repo_info = "beman.exemplar"
        beman_standard_check_config = "/path/to/.beman-standard.yml"
    """
    for path in paths:
        check_instance = check_class(repo_info, beman_standard_check_config)
        check_instance.path = Path(path)
        check_instance.log_level = True

        assert check_instance.pre_check() is True, (
            f"[{check_instance.__class__.__name__}] pre_check() failed for {path}"
        )
        assert check_instance.check() is expected_result, (
            f"[{check_instance.__class__.__name__}] check() failed for {path}"
        )


def run_fix_inplace_for_each_file_path(
    invalid_file_paths, check_class, repo_info, beman_standard_check_config
):
    """
    Run multiple testcases for a file-based check, for each file starting with a file that is invalid,
    and then fixing it.

    Example:
        invalid_file_paths = [
            "tests/lib/checks/beman_standard/readme/data/invalid/README-v1.md",
            "tests/lib/checks/beman_standard/readme/data/invalid/README-v2.md",
        ]
        check_class = ReadmeTitleCheck
        repo_info = "beman.exemplar"
        beman_standard_check_config = "beman_tidy/.beman-standard.yml"
    """
    for invalid_path in invalid_file_paths:
        check_instance = check_class(repo_info, beman_standard_check_config)
        check_instance.path = Path(f"{invalid_path}.delete_me")
        check_instance.write(invalid_path.read_text())

        assert check_instance.pre_check() is True
        assert check_instance.check() is False

        assert check_instance.fix() is True

        assert check_instance.pre_check() is True
        assert check_instance.check() is True

        # Delete the temporary file
        os.remove(f"{invalid_path}.delete_me")


def run_fix_inplace_for_each_directory_path(
    invalid_directory_paths, check_class, repo_info, beman_standard_check_config
):
    # TODO: We may not provide a fix_inplace method for directory-based checks.
    pass
