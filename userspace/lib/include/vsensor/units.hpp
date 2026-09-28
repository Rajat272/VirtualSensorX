#ifndef VSENSOR_UNITS_HPP
#define VSENSOR_UNITS_HPP

#include <string>
#include <cstdint>

namespace vsensor {

struct FormattedSample {
    double temp_C;
    double vib_g;
    double press_hPa;
    std::string temp_str;
    std::string vib_str;
    std::string press_str;
};

double milliCToCelsius(int32_t milliC);
double milliGToG(uint32_t milliG);
double paToHPa(uint32_t pa);

FormattedSample convertSample(int32_t milliC, uint32_t milliG, uint32_t pa);
std::string modeToString(uint32_t mode);

} // namespace vsensor

#endif // VSENSOR_UNITS_HPP
