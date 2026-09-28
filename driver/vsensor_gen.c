#include "vsensor_internal.h"

static void vs_work_func(struct work_struct *work)
{
	struct vs_device *dev = container_of(work, struct vs_device, work);
	struct vs_sample sample;

	vs_state_generate_sample(dev, &sample);
	vs_ringbuf_push(&dev->ring, &sample);

	wake_up_interruptible(&dev->read_wait);
}

static enum hrtimer_restart vs_timer_func(struct hrtimer *timer)
{
	struct vs_device *dev = container_of(timer, struct vs_device, timer);
	u64 interval_ns;
	u32 rate;

	mutex_lock(&dev->lock);
	rate = dev->rate_hz;
	if (!dev->running) {
		mutex_unlock(&dev->lock);
		return HRTIMER_NORESTART;
	}
	mutex_unlock(&dev->lock);

	schedule_work(&dev->work);

	interval_ns = 1000000000ULL / (rate ? rate : 1);
	hrtimer_forward_now(timer, ns_to_ktime(interval_ns));
	return HRTIMER_RESTART;
}

void vs_gen_init(struct vs_device *dev)
{
	INIT_WORK(&dev->work, vs_work_func);
	hrtimer_setup(&dev->timer, vs_timer_func, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
}

void vs_gen_start(struct vs_device *dev)
{
	u64 interval_ns;
	u32 rate;

	mutex_lock(&dev->lock);
	if (!dev->running) {
		dev->running = true;
		rate = dev->rate_hz;
		interval_ns = 1000000000ULL / (rate ? rate : 1);
		hrtimer_start(&dev->timer, ns_to_ktime(interval_ns), HRTIMER_MODE_REL);
	}
	mutex_unlock(&dev->lock);
}

void vs_gen_stop(struct vs_device *dev)
{
	mutex_lock(&dev->lock);
	dev->running = false;
	mutex_unlock(&dev->lock);

	hrtimer_cancel(&dev->timer);
	cancel_work_sync(&dev->work);
}

void vs_gen_cleanup(struct vs_device *dev)
{
	vs_gen_stop(dev);
}
