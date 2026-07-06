#!/bin/sh
set -e

echo "Installing Controller Mode..."

# Determine where this script lives
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

# Directories on the device
BIN_DIR="/usr/bin"
SYSTEMD_DIR="/etc/systemd/system"
SHARE_DIR="/usr/share/controller-mode/hid"

HERE="$(dirname "$0")"
LOG="$HERE/installattempt.log"
exec >"$LOG" 2>&1

set -x

# Ensure HID directory exists
mkdir -p "$SHARE_DIR"

echo "Compiling controller daemon..."
gcc -O2 -Wall -Wextra -o "$BIN_DIR/controller-daemon" "$SCRIPT_DIR/controller-daemon.c"
chmod +x "$BIN_DIR/controller-daemon"

echo "Copying scripts..."
cp "$SCRIPT_DIR/../scripts/controller-mode.sh" "$BIN_DIR/"
cp "$SCRIPT_DIR/../scripts/setup-hid-gadget.sh" "$BIN_DIR/"
cp "$SCRIPT_DIR/../scripts/clear-flag.sh" "$BIN_DIR/"
chmod +x "$BIN_DIR/controller-mode.sh"
chmod +x "$BIN_DIR/setup-hid-gadget.sh"
chmod +x "$BIN_DIR/clear-flag.sh"

echo "Copying HID descriptor..."
cp "$SCRIPT_DIR/../hid/gamepad-descriptor.bin" "$SHARE_DIR/"

echo "Installing systemd files..."
cp "$SCRIPT_DIR/../systemd/controller-daemon.service" "$SYSTEMD_DIR/"
cp "$SCRIPT_DIR/../systemd/controller-mode.service" "$SYSTEMD_DIR/"
cp "$SCRIPT_DIR/../systemd/controller-mode.target" "$SYSTEMD_DIR/"

echo "Reloading systemd..."
systemctl daemon-reload

echo "Enabling controller-mode.target..."
systemctl enable controller-mode.target

echo "Controller Mode installation complete."
echo "To activate controller mode on next boot, create:"
echo "    /boot/controller_mode"