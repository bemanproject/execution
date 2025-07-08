#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import pytest
from beman_tidy.lib.utils.git import load_beman_standard_config


@pytest.fixture
def mock_repo_info():
    """Return repository information for beman.exemplar library"""
    return {
        "top_level": ".",
        "name": "exemplar",
        "remote_url": "https://github.com/bemanproject/exemplar",
        "current_branch": "main",
        "commit_hash": 0,
        "status": "",
        "unstaged_changes": "",
    }


@pytest.fixture
def mock_beman_standard_check_config():
    """Parse the Beman Standard YAML file and return a dictionary of check configurations"""

    return load_beman_standard_config()
