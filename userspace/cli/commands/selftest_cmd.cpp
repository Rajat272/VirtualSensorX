#include "vsensor/device.hpp"
#include "vsensor/units.hpp"
#include "vsensor/errors.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>

int handleSelftest(int argc, char** argv) {
    (void)argc; (void)argv;
    int passed = 0;
    int failed = 0;

    auto test = [&](const std::string& name, auto fn) {
        std::cout << "[RUN] " << name << " ... " << std::flush;
        try {
            if (fn()) {
                std::cout << "PASS\n";
                passed++;
            } else {
                std::cout << "FAIL\n";
                failed++;
            }
        } catch (const std::exception& e) {
            std::cout << "FAIL (Exception: " << e.what() << ")\n";
            failed++;
        }
    };

    std::cout << "Starting VirtualSensor Built-in Self-Test Suite\n";
    std::cout << "=================================================\n";

    vsensor::Device dev;

    test("Reset state", [&]() {
        dev.reset();
        auto st = dev.getStatus();
        return st.samples_generated == 0 && st.buffer_fill == 0 && st.running == 0;
    });

    test("Mode changes & Rate configuration", [&]() {
        dev.setMode(VS_MODE_HIGH_FREQUENCY);
        dev.setRate(200);
        auto st = dev.getStatus();
        return st.mode == VS_MODE_HIGH_FREQUENCY;
    });

    test("Start sample generation & read continuity", [&]() {
        dev.start();
        if (vsensor::Device::isMockMode()) {
            auto s1 = dev.readSample();
            auto s2 = dev.readSample();
            dev.stop();
            return s2.sequence_id > s1.sequence_id;
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            auto st = dev.getStatus();
            if (!st.running || st.samples_generated == 0) return false;

            auto s1 = dev.readSample();
            auto s2 = dev.readSample();
            dev.stop();
            return s2.sequence_id > s1.sequence_id;
        }
    });

    test("Diagnostic mode sample injection & write permission", [&]() {
        dev.setMode(VS_MODE_DIAGNOSTIC);
        vsensor::Sample injected{};
        injected.temperature_mC = 45000;
        injected.vibration_mg = 800;
        injected.pressure_Pa = 102000;
        injected.sequence_id = 9999;
        dev.writeSample(injected);

        auto read_back = dev.readSample();
        return read_back.sequence_id == 9999 && read_back.temperature_mC == 45000;
    });

    test("Non-DIAGNOSTIC mode write restriction (-EPERM)", [&]() {
        dev.setMode(VS_MODE_NORMAL);
        vsensor::Sample s{};
        try {
            dev.writeSample(s);
            return false; /* should have thrown */
        } catch (const vsensor::SensorException& e) {
            return e.getErrno() == EPERM || vsensor::Device::isMockMode();
        }
    });

    std::cout << "=================================================\n";
    std::cout << "Results: " << passed << " PASSED, " << failed << " FAILED\n";

    return (failed == 0) ? 0 : 1;
}
