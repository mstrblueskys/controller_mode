#!/bin/sh

HERE="$(dirname "$0")"
LOG="$HERE/controller-mode.log"
exec >"$LOG" 2>&1

notify-send "Controller Mode Launcher Started"

INSTALL_FLAG="/usr/bin/controller-daemon"

# 1. Check if installed
if [ ! -f "$INSTALL_FLAG" ]; then
    notify-send "Installing Controller Mode..."
    sudo chmod +x "$HERE/controller-mode-os/src/install.sh"
    sudo sh "$HERE/controller-mode-os/src/install.sh"
    notify-send "Installation complete."
fi

# 2. Enable controller mode
notify-send "Enabling Controller Mode..."
sudo touch /boot/controller_mode

notify-send "Rebooting now..."
sudo reboot