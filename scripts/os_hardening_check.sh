#!/usr/bin/env bash
# Basic checks for Linux hardening items we added
set -euo pipefail

echo "Checking sysctl values..."
SYSCTL_FILE=/etc/sysctl.d/99-os_typing.conf
if [ -f "$SYSCTL_FILE" ]; then
  echo "Found $SYSCTL_FILE"
else
  echo "Warning: $SYSCTL_FILE not found; consider placing deploy/linux/sysctl-os_typing.conf into /etc/sysctl.d/99-os_typing.conf"
fi

if command -v ufw >/dev/null 2>&1; then
  echo "UFW status:"
  sudo ufw status verbose || true
else
  echo "UFW not installed"
fi

echo "Check systemd unit (if installed):"
systemctl status os_typing.service --no-pager || echo "Service not found or not running"

echo "Checks done - review warnings above."
