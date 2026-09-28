#!/bin/bash
set -e

echo "=== Running Ring Buffer Overrun Stress Test ==="

if [ ! -c "/dev/virtualsensor" ]; then
    echo "[SKIP] /dev/virtualsensor device node not available."
    exit 0
fi

# Set HIGH_FREQUENCY mode to generate samples quickly
./userspace/build/vsctl configure --mode HIGH_FREQUENCY --rate 500
./userspace/build/vsctl start

echo "Sleeping 1 second to accumulate overruns..."
sleep 1

./userspace/build/vsctl status
./userspace/build/vsctl stop

echo "=== Overrun Stress Test Complete ==="
