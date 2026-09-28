#include "vsensor/device.hpp"
#include "vsensor/units.hpp"
#include "vsensor/errors.hpp"
#include <iostream>
#include <string>

int handleConfig(int argc, char** argv) {
    vsensor::Device dev;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--mode" && i + 1 < argc) {
            std::string mode_str = argv[++i];
            vsensor::Mode mode = VS_MODE_NORMAL;
            if (mode_str == "NORMAL") mode = VS_MODE_NORMAL;
            else if (mode_str == "HIGH_FREQUENCY") mode = VS_MODE_HIGH_FREQUENCY;
            else if (mode_str == "LOW_POWER") mode = VS_MODE_LOW_POWER;
            else if (mode_str == "DIAGNOSTIC") mode = VS_MODE_DIAGNOSTIC;
            else if (mode_str == "ERROR") mode = VS_MODE_ERROR;
            else {
                std::cerr << "Invalid mode: " << mode_str << "\n";
                return 1;
            }
            dev.setMode(mode);
            std::cout << "Mode set to " << mode_str << "\n";
        } else if (arg == "--rate" && i + 1 < argc) {
            uint32_t rate = std::stoul(argv[++i]);
            dev.setRate(rate);
            std::cout << "Sample rate set to " << rate << " Hz\n";
        }
    }
    return 0;
}
