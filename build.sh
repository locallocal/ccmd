#!/usr/bin/env bash

set -euo pipefail

readonly BUILD_DIR="build"
readonly COVERAGE_DIR="cov"

print_help() {
    printf '%s\n' \
        "Usage: ./build.sh [option]" \
        "" \
        "Options:" \
        "  --release  Configure and build an optimized binary (default)." \
        "  --debug    Configure and build with debug symbols." \
        "  --test     Configure, build, and run the test suite." \
        "  --cov      Run tests and generate an HTML coverage report." \
        "  --clean    Remove generated build and coverage directories." \
        "  --help     Show this help message."
}

require_command() {
    if ! command -v "$1" >/dev/null 2>&1; then
        printf 'error: required command "%s" was not found.\n' "$1" >&2
        exit 1
    fi
}

configure_and_build() {
    local build_type="$1"
    shift
    cmake -S . -B "${BUILD_DIR}" -DCMAKE_BUILD_TYPE="${build_type}" "$@"
    cmake --build "${BUILD_DIR}" --parallel
}

run_tests() {
    configure_and_build Debug -DCCMD_BUILD_TESTS=ON "$@"
    ctest --test-dir "${BUILD_DIR}" --output-on-failure
}

main() {
    local action="${1:---release}"

    if [[ $# -gt 1 ]]; then
        print_help >&2
        exit 1
    fi

    case "${action}" in
        --release)
            require_command cmake
            configure_and_build Release -DCCMD_BUILD_TESTS=OFF -DCCMD_ENABLE_COVERAGE=OFF
            ;;
        --debug)
            require_command cmake
            configure_and_build Debug -DCCMD_BUILD_TESTS=OFF -DCCMD_ENABLE_COVERAGE=OFF
            ;;
        --test)
            require_command cmake
            run_tests -DCCMD_ENABLE_COVERAGE=OFF
            ;;
        --cov)
            require_command cmake
            require_command lcov
            require_command genhtml
            run_tests -DCCMD_ENABLE_COVERAGE=ON
            lcov --capture --directory "${BUILD_DIR}" --output-file "${BUILD_DIR}/ccmd.info"
            lcov --remove "${BUILD_DIR}/ccmd.info" '/usr/*' '*/third/*' '*/tests/*' \
                --output-file "${BUILD_DIR}/ccmd.filtered.info"
            genhtml "${BUILD_DIR}/ccmd.filtered.info" --output-directory "${COVERAGE_DIR}"
            printf 'Coverage report: %s/index.html\n' "${COVERAGE_DIR}"
            ;;
        --clean)
            require_command cmake
            cmake -E remove_directory "${BUILD_DIR}"
            cmake -E remove_directory "${COVERAGE_DIR}"
            ;;
        --help)
            print_help
            ;;
        *)
            printf 'error: unknown option "%s".\n\n' "${action}" >&2
            print_help >&2
            exit 1
            ;;
    esac
}

main "$@"
