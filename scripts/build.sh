#!/usr/bin/env bash
set -euo pipefail

# Build script for Unix-like systems (Linux, macOS)
# Usage: ./scripts/build.sh

TOPDIR="$(cd "$(dirname "$0")/.." && pwd)"
cd "$TOPDIR"

if command -v gcc >/dev/null 2>&1; then
    CC=gcc
elif command -v clang >/dev/null 2>&1; then
    CC=clang
else
    echo "No supported compiler found (gcc or clang). Install build-essential or clang." >&2
    exit 1
fi

echo "Using compiler: $CC"

$CC -DOS_CHECKING_TEST -Isrc -o os_checking src/os_checking.c
$CC -Isrc -o terminal_test src/terminal_test.c src/terminal.c
$CC -Iscripts/c-c++ -o os_controlsystem scripts/c-c++/os_controlsystem.cpp

echo "Running tests..."
./os_checking
printf "help\nexit\n" | ./terminal_test

# Run os_controlsystem for non-fatal environment checks; do not fail the build on non-zero
echo "Running os_controlsystem (non-fatal checks)..."
./os_controlsystem --checks all || echo "os_controlsystem reported issues (non-fatal)"

echo "All tests passed (except non-fatal control checks may have reported issues)."
