#include "vsensor/device.hpp"
#include "vsensor/units.hpp"
#include "vsensor/errors.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <chrono>

void printFormattedSample(const vsensor::Sample& s) {
    auto fmt = vsensor::convertSample(s.temperature_mC, s.vibration_mg, s.pressure_Pa);
    std::string status_str = "NORMAL";
    if (s.status_flags & VS_STATUS_FAULT) status_str = "FAULT";
    else if (s.status_flags & VS_STATUS_WARN) status_str = "WARN";

    std::cout << "Virtual Sensor\n"
              << "-----------------------\n"
              << "Temperature : " << fmt.temp_str << "\n"
              << "Vibration   : " << fmt.vib_str << "\n"
              << "Pressure    : " << fmt.press_str << "\n"
              << "Status      : " << status_str << "\n"
              << "Sequence ID : " << s.sequence_id << "\n";
}

int handleRead(int argc, char** argv) {
    bool watch = false;
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--watch") {
            watch = true;
        }
    }

    vsensor::Device dev;

    if (!watch) {
        if (!dev.waitForSample(std::chrono::milliseconds(1000))) {
            std::cout << "No sample available (timeout).\n";
            return 0;
        }
        auto sample = dev.readSample();
        printFormattedSample(sample);
    } else {
        std::cout << "Streaming samples (Press Ctrl+C to stop)...\n\n";
        while (true) {
            if (dev.waitForSample(std::chrono::milliseconds(500))) {
                auto sample = dev.readSample();
                printFormattedSample(sample);
                std::cout << "\n";
            }
        }
    }
    return 0;
}
