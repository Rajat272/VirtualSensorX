/*
 * VirtualSensorX - Shared User-Kernel ABI Header
 * include/uapi/virtualsensor_uapi.h
 */

#ifndef _VIRTUALSENSOR_UAPI_H_
#define _VIRTUALSENSOR_UAPI_H_

#ifdef __KERNEL__
#include <linux/types.h>
#include <linux/ioctl.h>
#else
#include <linux/types.h>
#include <sys/ioctl.h>
#endif

#define VS_RING_CAPACITY 64
#define VS_MIN_RATE_HZ   1
#define VS_MAX_RATE_HZ   1000
#define VS_DEFAULT_RATE_HZ 10

/* Device Operating Modes */
enum vs_mode {
    VS_MODE_NORMAL         = 0,
    VS_MODE_HIGH_FREQUENCY = 1,
    VS_MODE_LOW_POWER      = 2,
    VS_MODE_DIAGNOSTIC     = 3,
    VS_MODE_ERROR          = 4,
    VS_MODE_MAX
};

/* Status Flags (bitmask) */
#define VS_STATUS_NORMAL   (0x00)
#define VS_STATUS_WARN     (1 << 0)
#define VS_STATUS_FAULT    (1 << 1)
#define VS_STATUS_OVERRUN  (1 << 2)

/* Single Sample Structure */
struct vs_sample {
    __u64 timestamp_ns;
    __u32 sequence_id;
    __s32 temperature_mC;   /* milli-degrees C */
    __u32 vibration_mg;     /* milli-g */
    __u32 pressure_Pa;      /* Pascals */
    __u32 status_flags;     /* NORMAL / WARN / FAULT bits */
};

/* Status Information Structure */
struct vs_status {
    __u32 mode;
    __u32 running;
    __u32 buffer_fill;
    __u32 capacity;
    __u64 overruns;
    __u64 samples_generated;
    __u32 error_flags;
};

/* IOCTL Magic & Commands */
#define VS_IOC_MAGIC 'v'

#define VS_IOC_START        _IO(VS_IOC_MAGIC, 1)
#define VS_IOC_STOP         _IO(VS_IOC_MAGIC, 2)
#define VS_IOC_SET_MODE     _IOW(VS_IOC_MAGIC, 3, __u32)
#define VS_IOC_SET_RATE     _IOW(VS_IOC_MAGIC, 4, __u32)
#define VS_IOC_GET_STATUS   _IOR(VS_IOC_MAGIC, 5, struct vs_status)
#define VS_IOC_GET_SAMPLE   _IOR(VS_IOC_MAGIC, 6, struct vs_sample)
#define VS_IOC_RESET        _IO(VS_IOC_MAGIC, 7)
#define VS_IOC_INJECT_FAULT _IOW(VS_IOC_MAGIC, 8, __u32)

#endif /* _VIRTUALSENSOR_UAPI_H_ */
