#!/bin/sh

HERE="$(dirname "$0")"
LOG="$HERE/controller-mode-perm.log"
exec >"$LOG" 2>&1

echo "=== Fixing permissions ==="

chmod -R +x "$HERE/"
chmod +x "$HERE/launch-controller-mode.sh"
chmod +x "$HERE/exit-controller-mode.sh"
chmod +x "$HERE/controller-mode.sh"
chmod +x "$HERE/install.sh"

echo "Permissions fixed."