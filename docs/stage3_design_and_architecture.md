# Stage 3 – System Design & Architecture

## 1. Overall System Architecture
```
                                USER SPACE
+-----------------------------------------------------------------------+
|  vsctl (CLI Application)                                              |
|  - Parses command line parameters                                     |
|  - Calls libvsensor C++ API                                           |
|  - Displays formatted sensor metrics & handles self-tests             |
+-----------------------------------++----------------------------------+
                                    ||
+-----------------------------------\/----------------------------------+
|  libvsensor (C++17 RAII Shared Library)                               |
|  - vsensor::Device (open, read, write, ioctl, poll)                   |
|  - MockDevice (Hardwareless emulation backend for unit tests)          |
|  - Exception hierarchy & unit formatting (Celsius, g, Pa)             |
+-----------------------------------++----------------------------------+
                                    || System Call Boundary
====================================||===================================
                                    || (open / read / write / ioctl / poll)
                                KERNEL SPACE
+-----------------------------------\/----------------------------------+
|  virtualsensor.ko (Linux Character Device Driver)                     |
|  +------------------------------------------------------------------+ |
|  | VFS File Operations Handler (virtualsensor_fops.c)               | |
|  +------------------------------------------------------------------+ |
|  | IOCTL Command Processor (virtualsensor_ioctl.c)                  | |
|  +------------------------------------------------------------------+ |
|  | State Machine & Mode Manager (virtualsensor_main.c)              | |
|  +------------------------------------------------------------------+ |
|  | Ring Buffer Manager (virtualsensor_ringbuf.c) - Capacity 64      | |
|  +------------------------------------------------------------------+ |
|  | Timer & Workqueue Telemetry Generator (virtualsensor_timer.c)    | |
|  +------------------------------------------------------------------+ |
+-----------------------------------------------------------------------+
```

## 2. Major Components & Responsibilities
1. **UAPI (`virtualsensor_uapi.h`)**: Binary representation shared between kernel module and userspace applications. Defines `struct virtualsensor_sample`, `struct virtualsensor_status`, `struct virtualsensor_config`, and IOCTL magic macros (`VSENSOR_IOC_START`, `VSENSOR_IOC_STOP`, `VSENSOR_IOC_SET_CONFIG`, etc.).
2. **Kernel Driver Core (`driver/`)**:
   - `virtualsensor_main.c`: Device initialization (`module_init`), character device registration (`alloc_chrdev_region`, `cdev_add`), class device creation (`class_create`, `device_create`).
   - `virtualsensor_fops.c`: File operations implementation (`read`, `write`, `poll`). Restricts `write` to `DIAGNOSTIC` mode.
   - `virtualsensor_ioctl.c`: Safe handling of IOCTL commands and user-kernel buffer copying (`copy_to_user`, `copy_from_user`).
   - `virtualsensor_ringbuf.c`: Fixed-capacity ring buffer implementation protected by spinlocks (`spinlock_t`).
   - `virtualsensor_timer.c`: `hrtimer` high-resolution timer callback scheduling kernel work items (`work_struct`) to produce synthetic sensor data without blocking atomic interrupt contexts.
3. **Userspace Library (`userspace/libvsensor`)**:
   - `Device.cpp`: C++ RAII class managing device file descriptors, executing ioctls, wrapping errors into `SensorException`.
   - `MockDevice.cpp`: Standalone mock device backend providing identical API contracts for environments without loaded kernel modules.
4. **CLI Utility (`userspace/vsctl`)**:
   - `main.cpp`: Entry point supporting options (`status`, `start`, `stop`, `read`, `configure`, `test`).

## 3. Data Structures
```cpp
// Telemetry sample layout (24 bytes)
struct virtualsensor_sample {
    __u64 timestamp_ns;    // Nanoseconds since boot (ktime_get_real_ns)
    __s32 temp_milli_c;    // Temperature in milli-degrees Celsius (e.g., 25000 = 25.0 C)
    __s32 accel_milli_g;   // Vibration / Acceleration in milli-g (e.g., 1000 = 1.0 g)
    __u32 press_pa;        // Pressure in Pascals (e.g., 101325 Pa)
    __u32 flags;           // Fault flags (VSENSOR_FLAG_OVERHEAT, etc.)
};
```

## 4. UML Diagrams
### Class Diagram (Userspace C++ Library)
```
+-----------------------------------------+
|             vsensor::Device             |
+-----------------------------------------+
| - fd_: int                              |
| - path_: std::string                    |
+-----------------------------------------+
| + Device(const std::string& path)       |
| + ~Device()                             |
| + start(): void                         |
| + stop(): void                          |
| + readSample(): virtualsensor_sample    |
| + writeSample(sample): void             |
| + getStatus(): virtualsensor_status     |
| + setConfig(config): void               |
+-----------------------------------------+
```

### State Machine Diagram (Kernel Driver State)
```
     [STOPPED] -------(VSENSOR_IOC_START)-------> [RUNNING]
         ^                                           |
         |                                    (Fault Detected /
   (VSENSOR_IOC_STOP)                         VSENSOR_IOC_INJECT_FAULT)
         |                                           v
      [PAUSED] <------(VSENSOR_IOC_PAUSE)-------- [FAULT]
```

## 5. Development Environment & Tooling
- **Build System**: `Make` (Kernel driver) + `CMake 3.12+` (Userspace C++17 library and tests).
- **Compiler**: GCC / G++ with `-Wall -Wextra -Werror` support.
- **Version Control**: Git repository with branch strategy (`main` for release baseline, feature topic branches).

## 6. Stage 3 Progress Evidence & Verification
- Design specifications documented and validated against source code architecture.
- Baseline test suite re-verified: PASS.

## 7. Roadmap to Stage 4
- Verify core module implementation and prototype files.
- Demonstrate initial kernel and mock functionality.
- Document prototype implementation details.
