#ifndef VSENSOR_ERRORS_HPP
#define VSENSOR_ERRORS_HPP

#include <stdexcept>
#include <string>
#include <cerrno>
#include <cstring>

namespace vsensor {

class SensorException : public std::runtime_error {
public:
    explicit SensorException(const std::string& msg, int err_code = 0)
        : std::runtime_error(msg + (err_code ? ": " + std::string(strerror(err_code)) : "")),
          errno_(err_code) {}

    int getErrno() const noexcept { return errno_; }

private:
    int errno_;
};

} // namespace vsensor

#endif // VSENSOR_ERRORS_HPP
