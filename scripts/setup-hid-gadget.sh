#!/bin/sh

# Path to the gadget
G=/sys/kernel/config/usb_gadget/gamepad

# 1. Create gadget directory
mkdir -p $G

cd $G

# 2. Basic USB info
echo 0x1d6b > idVendor      # Linux Foundation
echo 0x0104 > idProduct     # Multifunction Composite Gadget
echo 0x0100 > bcdDevice     # v1.0.0
echo 0x0200 > bcdUSB        # USB2

# 3. Strings (optional but nice)
mkdir -p strings/0x409
echo "mstrblueskys" > strings/0x409/manufacturer
echo "RetroController" > strings/0x409/product
echo "69420" > strings/0x409/serialnumber

# 4. Create configuration
mkdir -p configs/c.1
echo 120 > configs/c.1/MaxPower

# 5. HID function (gamepad)
mkdir -p functions/hid.usb0
echo 1 > functions/hid.usb0/protocol
echo 1 > functions/hid.usb0/subclass
echo 64 > functions/hid.usb0/report_length

# Load your HID descriptor
cat /usr/share/controller-mode/hid/gamepad-descriptor.bin > functions/hid.usb0/report_desc

# 6. Link HID function to configuration
ln -s functions/hid.usb0 configs/c.1/

# 7. Bind to UDC (USB Device Controller)
UDC=$(ls /sys/class/udc | head -n 1)
echo $UDC > UDC