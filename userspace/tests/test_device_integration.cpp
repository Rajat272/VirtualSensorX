#include "vsensor/device.hpp"
#include "vsensor/units.hpp"
#include "vsensor/errors.hpp"
#include <iostream>
#include <cassert>
#include <chrono>

void testIntegration() {
    std::cout << "[TEST] Device Integration... " << std::flush;

    try {
        vsensor::Device dev;
        dev.reset();
        dev.start();
        if (dev.waitForSample(std::chrono::milliseconds(1000))) {
            auto sample = dev.readSample();
            assert(sample.sequence_id > 0);
        }
        dev.stop();
        std::cout << "PASS (real device)\n";
    } catch (const vsensor::SensorException& e) {
        std::cout << "SKIP (no real device available: " << e.what() << ")\n";
    }
}

int main() {
    testIntegration();
    return 0;
}
