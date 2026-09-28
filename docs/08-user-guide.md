# 08 - User Guide

## Quick Start Guide

### 1. Build Driver & Userspace
```bash
make -C driver W=1
cmake -S userspace -B userspace/build
cmake --build userspace/build -j
```

### 2. Load Driver
```bash
sudo ./scripts/load.sh
```

### 3. Use `vsctl` CLI
```bash
# Check status
./userspace/build/vsctl status

# Start sensor
./userspace/build/vsctl start

# Read sample
./userspace/build/vsctl read

# Watch continuous samples
./userspace/build/vsctl read --watch

# Configure high frequency mode
./userspace/build/vsctl configure --mode HIGH_FREQUENCY --rate 200

# Run self-check
./userspace/build/vsctl test
```

### 4. Unload Driver
```bash
sudo ./scripts/unload.sh
```
