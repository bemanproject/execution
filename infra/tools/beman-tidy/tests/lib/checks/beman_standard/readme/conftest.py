#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import pytest

from tests.utils.conftest import mock_repo_info, mock_beman_standard_check_config  # noqa: F401


@pytest.fixture(autouse=True)
def repo_info(mock_repo_info):  # noqa: F811
    return mock_repo_info


@pytest.fixture
def beman_standard_check_config(mock_beman_standard_check_config):  # noqa: F811
    return mock_beman_standard_check_config
