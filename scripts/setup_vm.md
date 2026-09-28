# VirtualSensorX VM & Development Environment Setup Guide

## Requirements
- Linux VM (Ubuntu 22.04 LTS / 24.04 LTS / Debian 12) or bare-metal Linux
- Kernel headers (`linux-headers-$(uname -r)`)
- GCC / G++ 11+ with C++17 support
- CMake 3.12+ and Make

## Installation Commands
```bash
sudo apt update
sudo apt install -y build-essential cmake linux-headers-$(uname -r) udev
```

## Build & Test Workflow

1. **Build Kernel Module**:
   ```bash
   make -C driver W=1
   ```

2. **Build Userspace & CLI**:
   ```bash
   cmake -S userspace -B userspace/build
   cmake --build userspace/build -j
   ```

3. **Load Module**:
   ```bash
   sudo ./scripts/load.sh
   ```

4. **Verify Device Creation**:
   ```bash
   lsmod | grep virtualsensor
   ls -l /dev/virtualsensor
   dmesg | tail -20
   ```

5. **Run Tests**:
   ```bash
   # CLI Self-Test
   ./userspace/build/vsctl test

   # All C++ Unit & Integration Tests
   ctest --test-dir userspace/build --output-on-failure

   # Negative & Stress Tests
   ./tests/negative/bad_ioctl_test.cpp
   ./tests/stress/overrun_test.sh
   ```

6. **Unload Module**:
   ```bash
   sudo ./scripts/unload.sh
   ```
