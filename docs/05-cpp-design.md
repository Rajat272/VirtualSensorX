# 05 - C++ User-Space Design

## Class Architecture (`vsensor::Device`)

The `vsensor::Device` class encapsulates access to `/dev/virtualsensor` using RAII principles:
- Non-copyable (`Device(const Device&) = delete`).
- Movable (`Device(Device&&)`).
- Destructor automatically closes the open file descriptor (`close(fd_)`).

## Fixed-Point Unit Conversion (`units.hpp`)
- `milliCToCelsius(int32_t milliC)` -> converts milli-degrees to double °C.
- `milliGToG(uint32_t milliG)` -> converts milli-g to double g.
- `paToHPa(uint32_t pa)` -> converts Pascals to double hPa.
