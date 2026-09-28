#!/bin/bash

MODULE_NAME="virtualsensor"

echo "Unloading $MODULE_NAME module..."

if lsmod | grep -q "$MODULE_NAME"; then
    rmmod "$MODULE_NAME"
    echo "$MODULE_NAME unloaded successfully."
else
    echo "Module $MODULE_NAME is not currently loaded."
fi
