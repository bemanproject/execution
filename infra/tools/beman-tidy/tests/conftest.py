#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import pytest
import os
from pathlib import Path


def pytest_configure(config):
    """
    Add custom markers to pytest.
    """
    config.addinivalue_line(
        "markers", "use_test_repo: mark test to use test repository instead of exemplar"
    )


@pytest.fixture(autouse=True)
def _setup_test_environment():
    """
    Setup test environment variables and paths.
    This runs automatically for all tests.
    """
    # Get the root directory of the project
    root_dir = Path(__file__).parent.parent

    # Add the project root to PYTHONPATH if not already there
    if str(root_dir) not in os.environ.get("PYTHONPATH", ""):
        os.environ["PYTHONPATH"] = f"{root_dir}:{os.environ.get('PYTHONPATH', '')}"

    yield
