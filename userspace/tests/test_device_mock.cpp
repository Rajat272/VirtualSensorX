#include "vsensor/device.hpp"
#include "vsensor/units.hpp"
#include "vsensor/errors.hpp"
#include <iostream>
#include <cassert>

void testMockDevice() {
    std::cout << "[TEST] Device Mock layer... " << std::flush;

    vsensor::Device::setMockMode(true);
    assert(vsensor::Device::isMockMode());

    vsensor::Device dev;
    dev.reset();
    auto st = dev.getStatus();
    assert(st.running == 0);
    assert(st.samples_generated == 0);

    dev.start();
    st = dev.getStatus();
    assert(st.running == 1);

    dev.setMode(VS_MODE_HIGH_FREQUENCY);
    st = dev.getStatus();
    assert(st.mode == VS_MODE_HIGH_FREQUENCY);

    auto sample = dev.readSample();
    assert(sample.sequence_id > 0);

    dev.stop();
    st = dev.getStatus();
    assert(st.running == 0);

    std::cout << "PASS\n";
}

int main() {
    testMockDevice();
    std::cout << "All device mock tests passed successfully!\n";
    return 0;
}
