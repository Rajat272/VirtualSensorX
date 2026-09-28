#include "vsensor/device.hpp"
#include "vsensor/units.hpp"
#include "vsensor/errors.hpp"
#include <iostream>

int handleStatus(int argc, char** argv) {
    (void)argc; (void)argv;
    vsensor::Device dev;
    auto st = dev.getStatus();

    std::cout << "VirtualSensor Status\n"
              << "---------------------\n"
              << "Running           : " << (st.running ? "YES" : "NO") << "\n"
              << "Mode              : " << vsensor::modeToString(st.mode) << "\n"
              << "Buffer Fill       : " << st.buffer_fill << " / " << st.capacity << "\n"
              << "Overruns          : " << st.overruns << "\n"
              << "Samples Generated : " << st.samples_generated << "\n"
              << "Error Flags       : 0x" << std::hex << st.error_flags << std::dec << "\n";
    return 0;
}
