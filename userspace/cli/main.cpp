#include "vsensor/device.hpp"
#include "vsensor/units.hpp"
#include "vsensor/errors.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

int handleRead(int argc, char** argv);
int handleConfig(int argc, char** argv);
int handleStatus(int argc, char** argv);
int handleSelftest(int argc, char** argv);

void printUsage() {
    std::cout << "vsctl - VirtualSensorX Control CLI\n"
              << "Usage: vsctl <command> [options]\n\n"
              << "Commands:\n"
              << "  start                  Start sensor sample generation\n"
              << "  stop                   Stop sensor sample generation\n"
              << "  read [--watch]         Read single sample or continuously (--watch)\n"
              << "  configure [--mode M] [--rate R]\n"
              << "                         Configure mode (NORMAL/HIGH_FREQUENCY/LOW_POWER/DIAGNOSTIC/ERROR) and rate (1-1000 Hz)\n"
              << "  status                 Display current sensor status\n"
              << "  reset                  Reset sensor state and clear buffers\n"
              << "  test                   Run built-in self-test suite\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string cmd = argv[1];

    try {
        if (cmd == "--mock" || cmd == "-m") {
            vsensor::Device::setMockMode(true);
            if (argc < 3) {
                printUsage();
                return 1;
            }
            cmd = argv[2];
            /* shift arguments */
            argc--;
            argv++;
        }

        if (cmd == "start") {
            vsensor::Device dev;
            dev.start();
            std::cout << "VirtualSensor started successfully.\n";
            return 0;
        } else if (cmd == "stop") {
            vsensor::Device dev;
            dev.stop();
            std::cout << "VirtualSensor stopped successfully.\n";
            return 0;
        } else if (cmd == "reset") {
            vsensor::Device dev;
            dev.reset();
            std::cout << "VirtualSensor state reset successfully.\n";
            return 0;
        } else if (cmd == "read") {
            return handleRead(argc - 1, argv + 1);
        } else if (cmd == "configure") {
            return handleConfig(argc - 1, argv + 1);
        } else if (cmd == "status") {
            return handleStatus(argc - 1, argv + 1);
        } else if (cmd == "test") {
            return handleSelftest(argc - 1, argv + 1);
        } else {
            std::cerr << "Unknown command: " << cmd << "\n";
            printUsage();
            return 1;
        }
    } catch (const vsensor::SensorException& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error: " << e.what() << "\n";
        return 1;
    }
}
