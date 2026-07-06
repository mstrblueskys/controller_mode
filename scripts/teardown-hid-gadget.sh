#!/bin/sh

# Universal HID Gadget Teardown Script
# Safely unbinds and removes the USB HID gadget

G=/sys/kernel/config/usb_gadget/gamepad

echo "=== Tearing down HID gadget ==="

# If gadget doesn't exist, exit cleanly
if [ ! -d "$G" ]; then
    echo "No HID gadget found. Nothing to tear down."
    exit 0
fi

# Unbind from UDC
if [ -f "$G/UDC" ]; then
    echo "" > "$G/UDC" 2>/dev/null || echo "UDC unbind failed (may already be unbound)"
fi

# Remove HID function link
if [ -L "$G/configs/c.1/hid.usb0" ]; then
    rm "$G/configs/c.1/hid.usb0"
fi

# Remove HID function directory
if [ -d "$G/functions/hid.usb0" ]; then
    rmdir "$G/functions/hid.usb0" 2>/dev/null || echo "Could not remove HID function directory"
fi

# Remove configuration directory
if [ -d "$G/configs/c.1" ]; then
    rmdir "$G/configs/c.1" 2>/dev/null || echo "Could not remove config directory"
fi

# Remove strings directory
if [ -d "$G/strings/0x409" ]; then
    rmdir "$G/strings/0x409" 2>/dev/null || echo "Could not remove strings directory"
fi

# Remove gadget directory
rmdir "$G" 2>/dev/null || echo "Could not remove gadget directory (may not be empty)"

echo "=== HID gadget teardown complete ==="
exit 0
