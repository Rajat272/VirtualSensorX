# 07 - Test Plan

## Test Levels
1. **Unit Tests** (`test_units.cpp`): Verifies fixed-point conversions and formatted strings.
2. **Mock Device Tests** (`test_device_mock.cpp`): Validates `vsensor::Device` RAII and logic without requiring kernel privileges.
3. **Integration Tests** (`test_device_integration.cpp`): Tests `libvsensor` against `/dev/virtualsensor`.
4. **CLI Self-Test** (`vsctl test`): Complete diagnostic check.
5. **Negative Tests** (`bad_ioctl_test.cpp`): Invalid magic, invalid bounds, permission checks.
6. **Stress Tests** (`overrun_test.sh`, `multi_reader_test.cpp`): Ring buffer overrun verification and multi-threaded reader stability.
