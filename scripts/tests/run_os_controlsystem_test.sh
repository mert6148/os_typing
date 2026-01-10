#!/usr/bin/env bash
set -euo pipefail

# Non-interactive test wrapper for os_controlsystem
# - Exits with os_controlsystem's exit code so CI can fail on issues
# - If binary is missing, attempt to compile; if compiler missing, exit 2

TOPDIR="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$TOPDIR"

# Default config path (relative to repo root)
CONFIG_PATH="tests/hardening_config.json"

# parse optional --config <path>
while [[ $# -gt 0 ]]; do
  case "$1" in
    --config)
      CONFIG_PATH="$2"; shift 2;;
    *) echo "Unknown arg: $1"; exit 1;;
  esac
done

BIN=./os_controlsystem
SRC=scripts/c-c++/os_controlsystem.cpp

if [ ! -x "$BIN" ]; then
  echo "os_controlsystem binary not found, attempting to compile..."
  if command -v g++ >/dev/null 2>&1; then
    g++ -std=c++17 -O2 -o "$BIN" "$SRC"
  elif command -v clang++ >/dev/null 2>&1; then
    clang++ -std=c++17 -O2 -o "$BIN" "$SRC"
  else
    echo "No C++ compiler found (g++ or clang++). Cannot build os_controlsystem." >&2
    exit 2
  fi
fi

# Run checks non-interactively and preserve exit code
echo "Running os_controlsystem --checks all --config $CONFIG_PATH (non-interactive)"
./os_controlsystem --checks all --config "$CONFIG_PATH"
rc=$?
if [ $rc -eq 0 ]; then
  echo "os_controlsystem: OK"
else
  echo "os_controlsystem: FAILED (exit code $rc)"
fi
exit $rc
