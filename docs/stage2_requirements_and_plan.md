# Stage 2 – Project Requirements & Development Plan

## 1. Functional Requirements
1. **Device File Interface**:
   - Expose `/dev/virtualsensor` character device node (major dynamically allocated, minor 0).
   - Standard file operations: `open()`, `close()`, `read()`, `write()`, `ioctl()`, `poll()`.
2. **Telemetry Data Generation**:
   - High-resolution kernel timer (`hrtimer`) driving periodic sample generation.
   - Configurable sampling frequencies from 1 Hz to 1000 Hz.
   - Fixed-point arithmetic for internal metrics (`milli_degC`, `milli_g`, `Pa`).
3. **Ring Buffer Management**:
   - Capacity of 64 telemetry samples.
   - Lock-protected ring buffer dropping oldest records on overflow and updating overrun statistics.
4. **State Machine & Operating Modes**:
   - State transition rules: `STOPPED` <-> `RUNNING` <-> `PAUSED` <-> `FAULT`.
   - Modes: `NORMAL`, `HIGH_FREQUENCY`, `LOW_POWER`, `DIAGNOSTIC`, `ERROR`.
5. **Security & Write Permission Control**:
   - `write()` system call restricted exclusively to `DIAGNOSTIC` mode; attempts in other modes return `-EPERM`.
6. **IOCTL Command Interface**:
   - Dedicated IOCTL commands for starting/stopping sampling, changing modes/rates, reading status, injecting synthetic samples, and resetting driver state.
7. **Userspace Library & CLI**:
   - C++17 RAII wrapper `vsensor::Device` with exception handling.
   - Command-line interface `vsctl` supporting monitoring, control, streaming, and mock testing.

## 2. Non-Functional Requirements
- **Performance**: Low CPU overhead during high-frequency sampling (1000 Hz) using deferrable kernel workqueues.
- **Reliability & Concurrency**: Thread-safe operations in kernel and userspace using spinlocks/mutexes.
- **Portability**: C++17 compliance for userspace library; Linux kernel module compatibility (kernel versions 5.x/6.x).
- **Maintainability**: Clean modular design separating UAPI definitions, kernel driver layers, C++ library abstractions, and CLI UI.

## 3. Project Modules & Deliverables
| Module | Location | Description |
|---|---|---|
| **UAPI Header** | `include/uapi/virtualsensor_uapi.h` | Shared struct schemas, IOCTL command IDs, enum definitions |
| **Kernel Driver** | `driver/` | Character device driver implementation (`virtualsensor.ko`) |
| **C++ Library** | `userspace/libvsensor/` | C++17 `libvsensor.a` RAII wrapper |
| **CLI Tool** | `userspace/vsctl/` | `vsctl` executable command-line application |
| **Tests** | `tests/` | Unit, integration, mock, and stress test suites |
| **Scripts** | `scripts/` | Driver loading (`load.sh`), unloading (`unload.sh`), and test execution |

## 4. Development Timeline & Plan
```
Stage 1: Project Introduction & Setup ────────────────► [COMPLETED]
Stage 2: Requirements & PRD ──────────────────────────► [COMPLETED]
Stage 3: System Design & Architecture ───────────────► [NEXT STAGE]
Stage 4: Core Implementation & Prototype ──────────────► [IN PLACE]
Stage 5: Testing, Integration & Optimization ──────────► [IN PLACE]
Stage 6: Final Presentation & Documentation ──────────► [PENDING]
```

## 5. Stage 2 Progress Evidence & Verification
- Functional and non-functional requirements audited and formalized in PRD.
- Baseline test suite re-verified: PASS.

## 6. Roadmap to Stage 3
- Formulate high-level and detailed architecture diagrams.
- Detail component responsibilities and data structure schemas.
- Provide UML Class, Sequence, and State Machine diagrams.
- Document Git repository structure and branching strategy.
