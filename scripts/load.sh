#!/bin/bash
set -e

DEVICE_NODE="/dev/virtualsensor"
MODULE_NAME="virtualsensor"

echo "Loading $MODULE_NAME module..."

if [ -f "driver/virtualsensor.ko" ]; then
    MODULE_PATH="driver/virtualsensor.ko"
elif [ -f "/tmp/vs_build/driver/virtualsensor.ko" ]; then
    MODULE_PATH="/tmp/vs_build/driver/virtualsensor.ko"
else
    echo "Error: virtualsensor.ko not found!"
    exit 1
fi

insmod "$MODULE_PATH"

if [ -f "scripts/udev/99-virtualsensor.rules" ]; then
    cp scripts/udev/99-virtualsensor.rules /etc/udev/rules.d/ 2>/dev/null || true
    udevadm control --reload-rules 2>/dev/null || true
    udevadm trigger 2>/dev/null || true
fi

# Fallback explicit node permission setup
if [ -c "$DEVICE_NODE" ]; then
    chmod 0666 "$DEVICE_NODE"
    echo "Device node $DEVICE_NODE permissions set to 0666"
fi

echo "$MODULE_NAME loaded successfully."
