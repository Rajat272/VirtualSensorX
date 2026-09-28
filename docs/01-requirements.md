# 01 - Requirements Document

## Overview
VirtualSensorX emulates an industrial sensor (temperature, vibration, pressure) as a character device `/dev/virtualsensor`.

## Functional Requirements
1. **Device File**: Expose `/dev/virtualsensor` character device node.
2. **Fixed-Point Arithmetic**: Internal kernel measurements stored as fixed-point integers (`milli_degC`, `milli_g`, `Pa`).
3. **Data Generation**: High-resolution timer (`hrtimer`) triggers sample creation at configured rates (1 - 1000 Hz).
4. **Ring Buffer**: Ring buffer of size 64. Overruns drop oldest records and increment overrun counters.
5. **IOCTL Interface**: Start/stop timer, configure mode/rate, query status/sample, reset state, and inject fault flags.
6. **Operating Modes**: `NORMAL`, `HIGH_FREQUENCY`, `LOW_POWER`, `DIAGNOSTIC`, `ERROR`.
7. **Write Access**: `write()` allowed only in `DIAGNOSTIC` mode (-EPERM otherwise).
