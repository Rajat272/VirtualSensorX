#!/bin/bash
set -e

echo "=== Running All VirtualSensorX Test Suites ==="

# 1. Userspace unit and mock tests
echo "--> Running C++ Userspace Tests..."
cd userspace
cmake -B build
cmake --build build -j
ctest --test-dir build --output-on-failure
cd ..

# 2. Mock CLI test
echo "--> Running vsctl CLI Mock Self-Test..."
userspace/build/vsctl --mock test

# 3. Kernel module test (if device present)
if [ -c "/dev/virtualsensor" ]; then
    echo "--> Real kernel device node detected at /dev/virtualsensor. Running live integration tests..."
    userspace/build/vsctl test
else
    echo "--> /dev/virtualsensor not present. (Skipping live kernel driver tests)."
fi

echo "=== All Available Tests Completed Successfully ==="
