#include "vsensor_internal.h"
#include <linux/ktime.h>

void vs_state_init(struct vs_device *dev)
{
	mutex_lock(&dev->lock);
	dev->mode = VS_MODE_NORMAL;
	dev->rate_hz = VS_DEFAULT_RATE_HZ;
	dev->running = false;
	dev->samples_generated = 0;
	dev->error_flags = VS_STATUS_NORMAL;
	dev->sequence_counter = 0;
	mutex_unlock(&dev->lock);
}

int vs_state_set_mode(struct vs_device *dev, enum vs_mode mode)
{
	if (mode >= VS_MODE_MAX)
		return -EINVAL;

	mutex_lock(&dev->lock);
	dev->mode = mode;
	if (mode == VS_MODE_ERROR)
		dev->error_flags |= VS_STATUS_FAULT;
	else
		dev->error_flags &= ~VS_STATUS_FAULT;

	/* Auto-adjust sample rate based on mode preset defaults if requested */
	if (mode == VS_MODE_HIGH_FREQUENCY)
		dev->rate_hz = 100;
	else if (mode == VS_MODE_LOW_POWER)
		dev->rate_hz = 1;
	else if (mode == VS_MODE_NORMAL)
		dev->rate_hz = VS_DEFAULT_RATE_HZ;

	mutex_unlock(&dev->lock);
	return 0;
}

int vs_state_set_rate(struct vs_device *dev, u32 rate_hz)
{
	if (rate_hz < VS_MIN_RATE_HZ || rate_hz > VS_MAX_RATE_HZ)
		return -EINVAL;

	mutex_lock(&dev->lock);
	dev->rate_hz = rate_hz;
	mutex_unlock(&dev->lock);
	return 0;
}

void vs_state_generate_sample(struct vs_device *dev, struct vs_sample *s)
{
	u64 ts = ktime_get_ns();
	u32 seq;
	enum vs_mode current_mode;

	mutex_lock(&dev->lock);
	dev->sequence_counter++;
	seq = dev->sequence_counter;
	dev->samples_generated++;
	current_mode = dev->mode;
	s->status_flags = dev->error_flags;
	mutex_unlock(&dev->lock);

	s->timestamp_ns = ts;
	s->sequence_id = seq;

	switch (current_mode) {
	case VS_MODE_DIAGNOSTIC:
		s->temperature_mC = 25000; /* 25.0 °C */
		s->vibration_mg   = 100;   /* 0.10 g */
		s->pressure_Pa    = 100000;/* 1000.0 hPa */
		break;
	case VS_MODE_HIGH_FREQUENCY:
		s->temperature_mC = 30000 + (seq % 1000);
		s->vibration_mg   = 500 + (seq % 200);
		s->pressure_Pa    = 101325 + (seq % 50);
		break;
	case VS_MODE_LOW_POWER:
		s->temperature_mC = 20000;
		s->vibration_mg   = 10;
		s->pressure_Pa    = 101000;
		break;
	case VS_MODE_ERROR:
		s->temperature_mC = -999000; /* Out of range error value */
		s->vibration_mg   = 999999;
		s->pressure_Pa    = 0;
		s->status_flags   |= VS_STATUS_FAULT;
		break;
	case VS_MODE_NORMAL:
	default:
		s->temperature_mC = 25000 + (seq % 500);  /* ~25.0 - 25.5 °C */
		s->vibration_mg   = 200 + (seq % 50);    /* ~0.20 - 0.25 g */
		s->pressure_Pa    = 101300 + (seq % 30); /* ~1013 hPa */
		break;
	}
}
