#include "vsensor/device.hpp"
#include "vsensor/errors.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <utility>
#include <vector>
#include <chrono>

namespace vsensor {

static bool s_mock_mode = false;

struct MockState {
    bool running{false};
    uint32_t mode{VS_MODE_NORMAL};
    uint32_t rate{VS_DEFAULT_RATE_HZ};
    uint64_t overruns{0};
    uint64_t generated{0};
    uint32_t error_flags{VS_STATUS_NORMAL};
    uint32_t seq{0};
    std::vector<Sample> buffer;
};

static MockState s_mock;

void Device::setMockMode(bool enable) {
    s_mock_mode = enable;
}

bool Device::isMockMode() {
    return s_mock_mode;
}

Device::Device(const std::string& path) : path_(path) {
    if (s_mock_mode) {
        fd_ = 999; /* Dummy mock FD */
        return;
    }
    fd_ = ::open(path.c_str(), O_RDWR);
    if (fd_ < 0) {
        throw SensorException("Failed to open device " + path, errno);
    }
}

Device::~Device() {
    if (fd_ >= 0 && !s_mock_mode) {
        ::close(fd_);
    }
    fd_ = -1;
}

Device::Device(Device&& other) noexcept : fd_(other.fd_), path_(std::move(other.path_)) {
    other.fd_ = -1;
}

Device& Device::operator=(Device&& other) noexcept {
    if (this != &other) {
        if (fd_ >= 0 && !s_mock_mode) {
            ::close(fd_);
        }
        fd_ = other.fd_;
        path_ = std::move(other.path_);
        other.fd_ = -1;
    }
    return *this;
}

void Device::checkOpen() const {
    if (fd_ < 0) {
        throw SensorException("Device file descriptor is closed");
    }
}

void Device::start() {
    checkOpen();
    if (s_mock_mode) {
        s_mock.running = true;
        return;
    }
    if (::ioctl(fd_, VS_IOC_START) < 0) {
        throw SensorException("VS_IOC_START failed", errno);
    }
}

void Device::stop() {
    checkOpen();
    if (s_mock_mode) {
        s_mock.running = false;
        return;
    }
    if (::ioctl(fd_, VS_IOC_STOP) < 0) {
        throw SensorException("VS_IOC_STOP failed", errno);
    }
}

void Device::setMode(Mode mode) {
    checkOpen();
    uint32_t m = static_cast<uint32_t>(mode);
    if (s_mock_mode) {
        if (m >= VS_MODE_MAX) throw SensorException("Invalid mode", EINVAL);
        s_mock.mode = m;
        return;
    }
    if (::ioctl(fd_, VS_IOC_SET_MODE, &m) < 0) {
        throw SensorException("VS_IOC_SET_MODE failed", errno);
    }
}

void Device::setRate(uint32_t rate_hz) {
    checkOpen();
    if (s_mock_mode) {
        if (rate_hz < VS_MIN_RATE_HZ || rate_hz > VS_MAX_RATE_HZ)
            throw SensorException("Invalid rate", EINVAL);
        s_mock.rate = rate_hz;
        return;
    }
    if (::ioctl(fd_, VS_IOC_SET_RATE, &rate_hz) < 0) {
        throw SensorException("VS_IOC_SET_RATE failed", errno);
    }
}

Status Device::getStatus() {
    checkOpen();
    Status st{};
    if (s_mock_mode) {
        st.mode = s_mock.mode;
        st.running = s_mock.running ? 1 : 0;
        st.buffer_fill = static_cast<uint32_t>(s_mock.buffer.size());
        st.capacity = VS_RING_CAPACITY;
        st.overruns = s_mock.overruns;
        st.samples_generated = s_mock.generated;
        st.error_flags = s_mock.error_flags;
        return st;
    }
    if (::ioctl(fd_, VS_IOC_GET_STATUS, &st) < 0) {
        throw SensorException("VS_IOC_GET_STATUS failed", errno);
    }
    return st;
}

Sample Device::getLatestSample() {
    checkOpen();
    Sample s{};
    if (s_mock_mode) {
        if (s_mock.buffer.empty()) throw SensorException("Buffer empty", EAGAIN);
        return s_mock.buffer.back();
    }
    if (::ioctl(fd_, VS_IOC_GET_SAMPLE, &s) < 0) {
        throw SensorException("VS_IOC_GET_SAMPLE failed", errno);
    }
    return s;
}

void Device::reset() {
    checkOpen();
    if (s_mock_mode) {
        s_mock.buffer.clear();
        s_mock.generated = 0;
        s_mock.seq = 0;
        s_mock.overruns = 0;
        s_mock.error_flags = VS_STATUS_NORMAL;
        s_mock.mode = VS_MODE_NORMAL;
        s_mock.rate = VS_DEFAULT_RATE_HZ;
        return;
    }
    if (::ioctl(fd_, VS_IOC_RESET) < 0) {
        throw SensorException("VS_IOC_RESET failed", errno);
    }
}

void Device::injectFault(uint32_t flags) {
    checkOpen();
    if (s_mock_mode) {
        s_mock.error_flags |= flags;
        s_mock.mode = VS_MODE_ERROR;
        return;
    }
    if (::ioctl(fd_, VS_IOC_INJECT_FAULT, &flags) < 0) {
        throw SensorException("VS_IOC_INJECT_FAULT failed", errno);
    }
}

Sample Device::readSample() {
    checkOpen();
    Sample s{};
    if (s_mock_mode) {
        if (s_mock.buffer.empty()) {
            /* Generate a mock sample if empty */
            s_mock.seq++;
            s_mock.generated++;
            s.timestamp_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            s.sequence_id = s_mock.seq;
            s.temperature_mC = 31600;
            s.vibration_mg = 420;
            s.pressure_Pa = 100800;
            s.status_flags = s_mock.error_flags;
            return s;
        } else {
            s = s_mock.buffer.front();
            s_mock.buffer.erase(s_mock.buffer.begin());
            return s;
        }
    }
    ssize_t ret = ::read(fd_, &s, sizeof(s));
    if (ret < 0) {
        throw SensorException("read failed", errno);
    }
    if (static_cast<size_t>(ret) < sizeof(s)) {
        throw SensorException("read partial sample record", EINVAL);
    }
    return s;
}

bool Device::waitForSample(std::chrono::milliseconds timeout) {
    checkOpen();
    if (s_mock_mode) return true;

    struct pollfd pfd{};
    pfd.fd = fd_;
    pfd.events = POLLIN;

    int ret = ::poll(&pfd, 1, static_cast<int>(timeout.count()));
    if (ret < 0) {
        throw SensorException("poll failed", errno);
    }
    return (ret > 0 && (pfd.revents & POLLIN));
}

void Device::writeSample(const Sample& sample) {
    checkOpen();
    if (s_mock_mode) {
        if (s_mock.mode != VS_MODE_DIAGNOSTIC) throw SensorException("Write permitted only in DIAGNOSTIC mode", EPERM);
        s_mock.buffer.push_back(sample);
        return;
    }
    ssize_t ret = ::write(fd_, &sample, sizeof(sample));
    if (ret < 0) {
        throw SensorException("write failed", errno);
    }
}

} // namespace vsensor
