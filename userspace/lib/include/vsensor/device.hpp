#ifndef VSENSOR_DEVICE_HPP
#define VSENSOR_DEVICE_HPP

#include <string>
#include <memory>
#include <chrono>
#include "vsensor/sample.hpp"

namespace vsensor {

class Device {
public:
    explicit Device(const std::string& path = "/dev/virtualsensor");
    ~Device();

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    Device(Device&& other) noexcept;
    Device& operator=(Device&& other) noexcept;

    void start();
    void stop();
    void setMode(Mode mode);
    void setRate(uint32_t rate_hz);
    Status getStatus();
    Sample getLatestSample();
    void reset();
    void injectFault(uint32_t flags);

    Sample readSample();
    bool waitForSample(std::chrono::milliseconds timeout);
    void writeSample(const Sample& sample);

    int getFd() const noexcept { return fd_; }
    bool isOpen() const noexcept { return fd_ >= 0; }

    /* For mock/testing support */
    static void setMockMode(bool enable);
    static bool isMockMode();

private:
    int fd_{-1};
    std::string path_;
    void checkOpen() const;
};

} // namespace vsensor

#endif // VSENSOR_DEVICE_HPP
