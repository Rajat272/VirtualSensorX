# 02 - Architecture Document

## Overview Architectural Diagram

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

## Layer Descriptions
- **UAPI**: Header `include/uapi/virtualsensor_uapi.h` defines binary memory representation shared by kernel and user space.
- **Kernel Module**: `virtualsensor.ko` handles sys call boundary, timer callbacks, queueing, and locking.
- **C++ Library**: `libvsensor` provides RAII wrapper (`vsensor::Device`), exception translation, and fixed-point unit conversion.
- **CLI**: `vsctl` provides command-line interaction and self-test verification.
