#!/bin/sh

# Universal Controller Mode Launcher

echo "=== Activating Controller Mode ==="

if command -v systemctl >/dev/null 2>&1; then
    systemctl start controller-mode.target
else
    echo "systemctl not available!"
fi

echo "=== Controller Mode Activated ==="
exit 0
