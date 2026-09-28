cmake_minimum_required(VERSION 3.12)
project(VirtualSensorX_Userspace CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Werror)

include_directories(
    lib/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Shared / Static Library
add_library(vsensor STATIC
    lib/src/device.cpp
    lib/src/units.cpp
)

# CLI Binary
add_executable(vsctl
    cli/main.cpp
    cli/commands/read_cmd.cpp
    cli/commands/config_cmd.cpp
    cli/commands/status_cmd.cpp
    cli/commands/selftest_cmd.cpp
)
target_link_libraries(vsctl PRIVATE vsensor)

# Enable Testing
enable_testing()

add_executable(test_units tests/test_units.cpp)
target_link_libraries(test_units PRIVATE vsensor)
add_test(NAME test_units COMMAND test_units)

add_executable(test_device_mock tests/test_device_mock.cpp)
target_link_libraries(test_device_mock PRIVATE vsensor)
add_test(NAME test_device_mock COMMAND test_device_mock)

add_executable(test_device_integration tests/test_device_integration.cpp)
target_link_libraries(test_device_integration PRIVATE vsensor)
add_test(NAME test_device_integration COMMAND test_device_integration)
