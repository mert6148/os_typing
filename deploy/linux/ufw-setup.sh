#!/usr/bin/env bash
set -euo pipefail

# Minimal UFW setup for os_typing
# Usage: sudo ./deploy/linux/ufw-setup.sh [--ssh-port 22] [--allow-port 12345]

SSH_PORT=22
ALLOW_PORT=12345

while [[ $# -gt 0 ]]; do
  case $1 in
    --ssh-port) SSH_PORT="$2"; shift 2;;
    --allow-port) ALLOW_PORT="$2"; shift 2;;
    *) echo "Unknown arg: $1"; exit 1;;
  esac
done

if ! command -v ufw >/dev/null 2>&1; then
  echo "ufw not found; install it (Ubuntu: apt install ufw)" >&2
  exit 1
fi

echo "Setting default deny incoming, allow outgoing"
sudo ufw default deny incoming
sudo ufw default allow outgoing

echo "Allowing SSH port: $SSH_PORT"
sudo ufw allow "$SSH_PORT"/tcp

echo "Allowing app port: $ALLOW_PORT"
sudo ufw allow "$ALLOW_PORT"/tcp

echo "Enabling UFW (may prompt)"
sudo ufw --force enable
sudo ufw status verbose
