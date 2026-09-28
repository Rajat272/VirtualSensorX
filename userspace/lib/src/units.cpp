#include "vsensor/units.hpp"
#include "uapi/virtualsensor_uapi.h"
#include <sstream>
#include <iomanip>

namespace vsensor {

double milliCToCelsius(int32_t milliC) {
    return static_cast<double>(milliC) / 1000.0;
}

double milliGToG(uint32_t milliG) {
    return static_cast<double>(milliG) / 1000.0;
}

double paToHPa(uint32_t pa) {
    return static_cast<double>(pa) / 100.0;
}

FormattedSample convertSample(int32_t milliC, uint32_t milliG, uint32_t pa) {
    FormattedSample f;
    f.temp_C = milliCToCelsius(milliC);
    f.vib_g = milliGToG(milliG);
    f.press_hPa = paToHPa(pa);

    std::ostringstream ss_t, ss_v, ss_p;
    ss_t << std::fixed << std::setprecision(1) << f.temp_C << " °C";
    ss_v << std::fixed << std::setprecision(2) << f.vib_g << " g";
    ss_p << std::fixed << std::setprecision(0) << f.press_hPa << " hPa";

    f.temp_str = ss_t.str();
    f.vib_str = ss_v.str();
    f.press_str = ss_p.str();

    return f;
}

std::string modeToString(uint32_t mode) {
    switch (mode) {
        case VS_MODE_NORMAL:         return "NORMAL";
        case VS_MODE_HIGH_FREQUENCY: return "HIGH_FREQUENCY";
        case VS_MODE_LOW_POWER:      return "LOW_POWER";
        case VS_MODE_DIAGNOSTIC:     return "DIAGNOSTIC";
        case VS_MODE_ERROR:          return "ERROR";
        default:                     return "UNKNOWN";
    }
}

} // namespace vsensor
