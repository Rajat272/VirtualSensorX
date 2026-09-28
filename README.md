# VirtualSensorX - Industrial Sensor Kernel Emulator & C++17 Library

VirtualSensorX is a Linux kernel module (`virtualsensor.ko`) that emulates an industrial sensor (temperature, vibration, pressure) as a character device at `/dev/virtualsensor`, paired with a C++17 library (`libvsensor`) and CLI (`vsctl`).

## Architecture
```
            USER SPACE
┌───────────────────────────────────────┐
│  vsctl (CLI)                          │
│  libvsensor (C++17, RAII Device class)│
└──────────────────┬────────────────────┘
        open/read/write/ioctl/poll
═══════════ system call boundary ═══════
                KERNEL SPACE
┌──────────────────▼────────────────────┐
│  virtualsensor.ko                     │
│  ├─ file operations layer             │
│  ├─ ioctl command handler             │
│  ├─ device state machine (modes)      │
│  ├─ ring buffer (samples)             │
│  ├─ sample generator (hrtimer + work) │
│  └─ wait queue (blocking / poll)      │
└───────────────────────────────────────┘
         Virtual Hardware (simulated)
```

## Prerequisites
- Linux kernel headers (`linux-headers-$(uname -r)`)
- GCC / G++ with C++17 support
- CMake 3.12+ and Make

## Build Steps
```bash
# Build kernel module
make -C driver W=1

# Build C++ library, CLI, and unit tests
cmake -S userspace -B userspace/build
cmake --build userspace/build -j
```

## Load & Unload Driver
```bash
# Load module
sudo ./scripts/load.sh

# Unload module
sudo ./scripts/unload.sh
```

## Usage Examples (`vsctl`)
```bash
# Check sensor status
./userspace/build/vsctl status

# Start periodic sampling
./userspace/build/vsctl start

# Read single formatted sample
./userspace/build/vsctl read

# Run continuous streaming read
./userspace/build/vsctl read --watch

# Configure operating mode and sample rate
./userspace/build/vsctl configure --mode HIGH_FREQUENCY --rate 200

# Run built-in self-test suite (works in mock mode without driver load!)
./userspace/build/vsctl --mock test
```

## Running Test Suites
```bash
# Run all userspace C++ unit and mock integration tests
ctest --test-dir userspace/build --output-on-failure

# Run complete script test suite
./scripts/run_all_tests.sh
```

## Troubleshooting
- **Permission Denied opening `/dev/virtualsensor`**: Ensure udev rules are loaded (`scripts/load.sh`) or run `sudo chmod 0666 /dev/virtualsensor`.
- **`write()` fails with Permission Denied**: Write sample injection is only permitted when device mode is `DIAGNOSTIC`.
- **WSL / Container execution**: `insmod` requires root privileges in a Linux kernel with module loading enabled. Use `vsctl --mock` for userspace verification in restricted environments.
