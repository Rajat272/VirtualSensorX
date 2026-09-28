#ifndef VSENSOR_SAMPLE_HPP
#define VSENSOR_SAMPLE_HPP

#include <cstdint>
#include <string>
#include "uapi/virtualsensor_uapi.h"

namespace vsensor {

using Sample = struct vs_sample;
using Status = struct vs_status;
using Mode   = enum vs_mode;

} // namespace vsensor

#endif // VSENSOR_SAMPLE_HPP
