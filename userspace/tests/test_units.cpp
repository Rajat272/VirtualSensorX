#include "vsensor/units.hpp"
#include <iostream>
#include <cassert>
#include <cmath>

void testUnits() {
    std::cout << "[TEST] Units conversion... " << std::flush;

    assert(std::abs(vsensor::milliCToCelsius(31600) - 31.6) < 0.001);
    assert(std::abs(vsensor::milliGToG(420) - 0.42) < 0.001);
    assert(std::abs(vsensor::paToHPa(100800) - 1008.0) < 0.001);

    auto fmt = vsensor::convertSample(31600, 420, 100800);
    assert(fmt.temp_str == "31.6 °C");
    assert(fmt.vib_str == "0.42 g");
    assert(fmt.press_str == "1008 hPa");

    std::cout << "PASS\n";
}

int main() {
    testUnits();
    std::cout << "All unit tests passed successfully!\n";
    return 0;
}
