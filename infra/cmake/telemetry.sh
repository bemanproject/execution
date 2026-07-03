# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#!/usr/bin/env bash

set -o nounset
set -o errexit
trap 'echo "Aborting due to errexit on line $LINENO. Exit code: $?" >&2' ERR
set -o errtrace
set -o pipefail
IFS=$'\n\t'

###############################################################################
# Environment
###############################################################################

# $_ME
#
# This program's basename.
_ME="$(basename "${0}")"

###############################################################################
# Help
###############################################################################

# _print_help()
#
# Usage:
#   _print_help
#
# Print the program help information.
_print_help() {
    cat <<HEREDOC

Callback script to process CMake Instrumentation data
https://cmake.org/cmake/help/latest/command/cmake_instrumentation.html

Usage:
  ${_ME} [<arguments>]
  ${_ME} -h | --help

Options:
  -h --help  Show this screen.

Environment:
  Setting DEBUG_TELEMETRY in the environment will enable DEBUG logging
HEREDOC
}

###############################################################################
# Program Functions
###############################################################################
_debug_print() {
    if [[ -n "${DEBUG_TELEMETRY:-}" ]]; then
        printf "[DEBUG] $(date +'%H:%M:%S'): %s \n" "$1" >&2
    fi
}

_check_file_exists() {
    local file="$1"
    if [[ ! -f "${file}" ]]; then
        echo "Error: File not found: ${file}" >&2
        exit 1  # Exit the entire script with a non-zero status
    fi
}

_process_index() {
    indexFile=${1:-}
    _check_file_exists "${indexFile}"
    _debug_print "$(cat "${indexFile}")"

    local buildDir
    buildDir=$(jq -r '.buildDir' "${1:-}")
    _debug_print "$(printf "buildDir is |%q|" "${buildDir}")"

    local dataDir
    dataDir=$(jq -r '.dataDir' "${1:-}")
    _debug_print "$(printf "dataDir is |%q|" "${dataDir}")"

    local hook
    hook=$(jq -r '.hook' "${1:-}")
    _debug_print "$(printf "hook is |%q|" "${hook}")"

    local trace
    trace=$(jq -r '.trace' "${1:-}")
    _debug_print "$(printf "trace is |%q|" "${trace}")"

    local outputDir
    outputDir="${buildDir}/.trace"
    _debug_print "$(printf "Copy trace to |%q|" "${outputDir}")"
    mkdir -p "${outputDir}"

    local traceDestFile
    traceDestFile="${outputDir}/${hook}-$(basename "${trace}")"
    _debug_print "$(printf "traceDestFile: |%q|" "${traceDestFile}")"
    cp "${dataDir}/${trace}" "${outputDir}/${hook}-$(basename "${trace}")"
}

###############################################################################
# Main
###############################################################################

# _main()
#
# Usage:
#   _main [<options>] [<arguments>]
#
# Description:
#   Entry point for the program, handling basic option parsing and dispatching.
_main() {
    # Avoid complex option parsing when only one program option is expected.
    if [[ "${1:-}" =~ ^-h|--help$  ]]
    then
        _print_help
    else
        _process_index "$@"
    fi
}

# Call `_main` after everything has been defined.
_main "$@"
