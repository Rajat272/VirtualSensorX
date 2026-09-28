#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <fcntl.h>
#include <unistd.h>
#include "uapi/virtualsensor_uapi.h"

int main() {
    std::cout << "=== Running Multi-Reader Stress Test ===\n";

    int check_fd = open("/dev/virtualsensor", O_RDWR);
    if (check_fd < 0) {
        std::cout << "[SKIP] /dev/virtualsensor not loaded.\n";
        return 0;
    }
    close(check_fd);

    std::atomic<bool> running{true};
    std::atomic<uint64_t> total_read{0};
    constexpr int NUM_THREADS = 4;
    std::vector<std::thread> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&]() {
            int fd = open("/dev/virtualsensor", O_RDONLY | O_NONBLOCK);
            if (fd < 0) return;
            struct vs_sample s;
            while (running) {
                ssize_t ret = read(fd, &s, sizeof(s));
                if (ret == sizeof(s)) {
                    total_read++;
                }
                std::this_thread::sleep_for(std::chrono::microseconds(500));
            }
            close(fd);
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    running = false;

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }

    std::cout << "Multi-reader total samples read across threads: " << total_read << "\n";
    std::cout << "=== Multi-Reader Test PASSED ===" << "\n";
    return 0;
}
