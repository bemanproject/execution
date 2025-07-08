#!/usr/bin/env python3
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import sys

from .checks.system.registry import get_registered_beman_standard_checks
from .checks.system.git import DisallowFixInplaceAndUnstagedChangesCheck

# import all the implemented checks.
# TODO: Consider removing F403 from ignored lint checks
from .checks.beman_standard.cmake import *  # noqa: F401, F403
from .checks.beman_standard.cpp import *  # noqa: F401, F403
from .checks.beman_standard.directory import *  # noqa: F401, F403
from .checks.beman_standard.file import *  # noqa: F401, F403
from .checks.beman_standard.general import *  # noqa: F401, F403
from .checks.beman_standard.license import *  # noqa: F401, F403
from .checks.beman_standard.readme import *  # noqa: F401, F403
from .checks.beman_standard.release import *  # noqa: F401, F403
from .checks.beman_standard.toplevel import *  # noqa: F401, F403

red_color = "\033[91m"
green_color = "\033[92m"
yellow_color = "\033[93m"
gray_color = "\033[90m"
no_color = "\033[0m"


def run_checks_pipeline(checks_to_run, args, beman_standard_check_config):
    """
    Run the checks pipeline for The Beman Standard.
    Read-only checks if args.fix_inplace is False, otherwise try to fix the issues in-place.
    Verbosity is controlled by args.verbose.

    @return: The number of failed checks.
    """

    """
    Helper function to log messages.
    """

    def log(msg):
        if args.verbose:
            print(msg)

    """
    Helper function to run a check.
    @param check_class: The check class type to run.
    @param log_enabled: Whether to log the check result.
    @return: True if the check passed, False otherwise.
    """

    def run_check(check_class, log_enabled=args.verbose):
        check_instance = check_class(args.repo_info, beman_standard_check_config)
        check_instance.log_enabled = log_enabled
        check_type = check_instance.type

        log(f"Running check [{check_instance.type}][{check_instance.name}] ... ")

        if (check_instance.pre_check() and check_instance.check()) or (
            args.fix_inplace and check_instance.fix()
        ):
            log(
                f"\tcheck [{check_instance.type}][{check_instance.name}] ... {green_color}PASSED{no_color}\n"
            )
            return check_type, True
        else:
            log(
                f"\tcheck [{check_instance.type}][{check_instance.name}] ... {red_color}FAILED{no_color}\n"
            )
            return check_type, False

    """
    Main pipeline.
    """

    def run_pipeline_helper():
        # Internal checks
        if args.fix_inplace:
            run_check(DisallowFixInplaceAndUnstagedChangesCheck, log_enabled=False)

        implemented_checks = get_registered_beman_standard_checks()
        all_checks = beman_standard_check_config

        cnt_passed = {
            "REQUIREMENT": 0,
            "RECOMMENDATION": 0,
        }
        cnt_failed = {
            "REQUIREMENT": 0,
            "RECOMMENDATION": 0,
        }
        for check_name in checks_to_run:
            if check_name not in implemented_checks:
                continue

            check_type, passed = run_check(implemented_checks[check_name])
            if passed:
                cnt_passed[check_type] += 1
            else:
                cnt_failed[check_type] += 1

        cnt_skipped = {
            "REQUIREMENT": 0,
            "RECOMMENDATION": 0,
        }
        cnt_all_beman_standard_checks = {
            "REQUIREMENT": 0,
            "RECOMMENDATION": 0,
        }
        cnt_implemented_checks = {
            "REQUIREMENT": 0,
            "RECOMMENDATION": 0,
        }
        for check_name in all_checks:
            check_type = all_checks[check_name]["type"]
            cnt_all_beman_standard_checks[check_type] += 1

            if check_name not in implemented_checks:
                cnt_skipped[check_type] += 1
            else:
                cnt_implemented_checks[check_type] += 1

        return (
            cnt_passed,
            cnt_failed,
            cnt_skipped,
            cnt_implemented_checks,
            cnt_all_beman_standard_checks,
        )

    log("beman-tidy pipeline started ...\n")
    (
        cnt_passed,
        cnt_failed,
        cnt_skipped,
        cnt_implemented_checks,
        cnt_all_beman_standard_checks,
    ) = run_pipeline_helper()
    log("\nbeman-tidy pipeline finished.\n")

    # Always print the summary.
    print(
        f"Summary    REQUIREMENT: {green_color} {cnt_passed['REQUIREMENT']} checks PASSED{no_color}, {red_color}{cnt_failed['REQUIREMENT']} checks FAILED{no_color}, {gray_color}{cnt_skipped['REQUIREMENT']} skipped (NOT implemented).{no_color}"
    )
    print(
        f"Summary RECOMMENDATION: {green_color} {cnt_passed['RECOMMENDATION']} checks PASSED{no_color}, {red_color}{cnt_failed['RECOMMENDATION']} checks FAILED{no_color}, {gray_color}{cnt_skipped['RECOMMENDATION']} skipped (NOT implemented).{no_color}"
    )

    # Always print the coverage.
    coverage_requirement = round(
        cnt_passed["REQUIREMENT"] / cnt_implemented_checks["REQUIREMENT"] * 100, 2
    )
    coverage_recommendation = round(
        cnt_passed["RECOMMENDATION"] / cnt_implemented_checks["RECOMMENDATION"] * 100, 2
    )
    total_passed = cnt_passed["REQUIREMENT"] + cnt_passed["RECOMMENDATION"]
    total_implemented = (
        cnt_implemented_checks["REQUIREMENT"] + cnt_implemented_checks["RECOMMENDATION"]
    )
    total_coverage = round((total_passed) / (total_implemented) * 100, 2)
    print(
        f"\n{__calculate_coverage_color(coverage_requirement)}Coverage    REQUIREMENT: {coverage_requirement:{6}.2f}% ({cnt_passed['REQUIREMENT']}/{cnt_implemented_checks['REQUIREMENT']} checks passed).{no_color}"
    )
    if args.require_all:
        print(
            f"{__calculate_coverage_color(coverage_recommendation)}Coverage RECOMMENDATION: {coverage_recommendation:{6}.2f}% ({cnt_passed['RECOMMENDATION']}/{cnt_implemented_checks['RECOMMENDATION']} checks passed).{no_color}"
        )
        print(
            f"{__calculate_coverage_color(total_coverage)}Coverage          TOTAL: {total_coverage:{6}.2f}% ({total_passed}/{total_implemented} checks passed).{no_color}"
        )
    else:
        print("Note: RECOMMENDATIONs are not included (--require-all NOT set).")
    total_cnt_failed = cnt_failed["REQUIREMENT"] + (
        cnt_failed["RECOMMENDATION"] if args.require_all else 0
    )

    sys.stdout.flush()
    return total_cnt_failed


def __calculate_coverage_color(cov):
    """
    Returns the colour for the coverage print based on severity
    Green for 100%
    Red for 0%
    Yellow for anything else
    """
    if cov == 100:
        return green_color
    elif cov == 0:
        return red_color
    else:
        return yellow_color
