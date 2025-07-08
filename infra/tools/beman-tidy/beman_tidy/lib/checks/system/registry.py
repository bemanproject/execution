#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from typing import Dict, Type, List

# Registry to store all The Beman Standard check classes.
_beman_standard_check_registry: Dict[str, Type] = {}


def register_beman_standard_check(check: str):
    """
    Decorator to register a check class with a specific ID.

    Usage:
        @register_beman_standard_check("README.TITLE")
        class ReadmeTitleCheck(ReadmeBaseCheck):
            ...

    Notes: Only register most derived check classes, which are actually part from
    The Beman Standard - e.g., README.TITLE, README.BADGES, etc.
    """

    def decorator(check_class: Type) -> Type:
        _beman_standard_check_registry[check] = check_class
        return check_class

    return decorator


def get_registered_beman_standard_checks() -> Dict[str, Type]:
    """Get all registered check classes"""
    return _beman_standard_check_registry.copy()


def get_beman_standard_check_by_name(check_name: str) -> Type:
    """Get a specific check class by its name"""
    return _beman_standard_check_registry.get(check_name)


def get_all_beman_standard_check_names() -> List[str]:
    """Get all registered check names"""
    return list(_beman_standard_check_registry.keys())


def get_beman_standard_check_name_by_class(target_check_class: Type) -> str:
    """Get the name of a check class"""
    for check_name, check_class in _beman_standard_check_registry.items():
        if check_class == target_check_class:
            return check_name
    return None
