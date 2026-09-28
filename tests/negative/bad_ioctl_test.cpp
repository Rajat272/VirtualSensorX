#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <cerrno>
#include <cassert>
#include "uapi/virtualsensor_uapi.h"

int main() {
    std::cout << "=== Running Negative IOCTL Tests ===\n";

    int fd = open("/dev/virtualsensor", O_RDWR);
    if (fd < 0) {
        std::cout << "[SKIP] Cannot open /dev/virtualsensor (device not loaded)\n";
        return 0;
    }

    /* 1. Invalid IOCTL Magic / Command */
    int ret = ioctl(fd, _IO('x', 99));
    assert(ret < 0 && errno == ENOTTY);
    std::cout << "[PASS] Invalid IOCTL magic returns -ENOTTY\n";

    /* 2. Out-of-range sample rate */
    uint32_t bad_rate = 5000;
    ret = ioctl(fd, VS_IOC_SET_RATE, &bad_rate);
    assert(ret < 0 && errno == EINVAL);
    std::cout << "[PASS] Out-of-range rate (5000 Hz) returns -EINVAL\n";

    /* 3. Out-of-range mode */
    uint32_t bad_mode = 99;
    ret = ioctl(fd, VS_IOC_SET_MODE, &bad_mode);
    assert(ret < 0 && errno == EINVAL);
    std::cout << "[PASS] Out-of-range mode (99) returns -EINVAL\n";

    /* 4. Write in NORMAL mode */
    uint32_t mode = VS_MODE_NORMAL;
    ioctl(fd, VS_IOC_SET_MODE, &mode);
    struct vs_sample dummy{};
    ssize_t wret = write(fd, &dummy, sizeof(dummy));
    assert(wret < 0 && errno == EPERM);
    std::cout << "[PASS] Write in NORMAL mode returns -EPERM\n";

    close(fd);
    std::cout << "=== All Negative IOCTL Tests PASSED ===\n";
    return 0;
}
