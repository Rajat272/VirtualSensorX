#include "vsensor_internal.h"

long vs_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct vs_device *dev = file->private_data;
	int ret = 0;
	u32 val32;
	struct vs_status st;
	struct vs_sample sample;
	unsigned long flags;

	if (_IOC_TYPE(cmd) != VS_IOC_MAGIC)
		return -ENOTTY;

	switch (cmd) {
	case VS_IOC_START:
		vs_gen_start(dev);
		break;

	case VS_IOC_STOP:
		vs_gen_stop(dev);
		break;

	case VS_IOC_SET_MODE:
		if (copy_from_user(&val32, (void __user *)arg, sizeof(val32)))
			return -EFAULT;
		ret = vs_state_set_mode(dev, (enum vs_mode)val32);
		break;

	case VS_IOC_SET_RATE:
		if (copy_from_user(&val32, (void __user *)arg, sizeof(val32)))
			return -EFAULT;
		ret = vs_state_set_rate(dev, val32);
		break;

	case VS_IOC_GET_STATUS:
		mutex_lock(&dev->lock);
		st.mode = (u32)dev->mode;
		st.running = dev->running ? 1 : 0;
		st.capacity = VS_RING_CAPACITY;
		st.samples_generated = dev->samples_generated;
		st.error_flags = dev->error_flags;
		mutex_unlock(&dev->lock);

		spin_lock_irqsave(&dev->ring.lock, flags);
		st.buffer_fill = dev->ring.count;
		st.overruns = dev->ring.overruns;
		spin_unlock_irqrestore(&dev->ring.lock, flags);

		if (copy_to_user((void __user *)arg, &st, sizeof(st)))
			return -EFAULT;
		break;

	case VS_IOC_GET_SAMPLE:
		if (!vs_ringbuf_peek_latest(&dev->ring, &sample))
			return -EAGAIN;
		if (copy_to_user((void __user *)arg, &sample, sizeof(sample)))
			return -EFAULT;
		break;

	case VS_IOC_RESET:
		mutex_lock(&dev->lock);
		dev->samples_generated = 0;
		dev->sequence_counter = 0;
		dev->error_flags = VS_STATUS_NORMAL;
		dev->mode = VS_MODE_NORMAL;
		dev->rate_hz = VS_DEFAULT_RATE_HZ;
		mutex_unlock(&dev->lock);

		vs_ringbuf_clear(&dev->ring);
		break;

	case VS_IOC_INJECT_FAULT:
		if (copy_from_user(&val32, (void __user *)arg, sizeof(val32)))
			return -EFAULT;
		mutex_lock(&dev->lock);
		dev->error_flags |= val32;
		dev->mode = VS_MODE_ERROR;
		mutex_unlock(&dev->lock);
		break;

	default:
		ret = -ENOTTY;
		break;
	}

	return ret;
}
