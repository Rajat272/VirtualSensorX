#include "vsensor_internal.h"

int vs_open(struct inode *inode, struct file *file)
{
	struct vs_device *dev = container_of(inode->i_cdev, struct vs_device, cdev);
	file->private_data = dev;
	return 0;
}

int vs_release(struct inode *inode, struct file *file)
{
	return 0;
}

ssize_t vs_read(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	struct vs_device *dev = file->private_data;
	struct vs_sample sample;
	size_t records_to_read;
	size_t records_read = 0;
	int ret;

	if (count < sizeof(struct vs_sample))
		return -EINVAL;

	records_to_read = count / sizeof(struct vs_sample);

	while (records_read < records_to_read) {
		if (vs_ringbuf_pop(&dev->ring, &sample)) {
			if (copy_to_user(buf + (records_read * sizeof(struct vs_sample)),
					 &sample, sizeof(struct vs_sample))) {
				return records_read ? (ssize_t)(records_read * sizeof(struct vs_sample)) : -EFAULT;
			}
			records_read++;
			continue;
		}

		/* Ring buffer empty */
		if (records_read > 0)
			break;

		if (file->f_flags & O_NONBLOCK)
			return -EAGAIN;

		ret = wait_event_interruptible(dev->read_wait, dev->ring.count > 0);
		if (ret)
			return ret; /* -ERESTARTSYS */
	}

	return (ssize_t)(records_read * sizeof(struct vs_sample));
}

ssize_t vs_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	struct vs_device *dev = file->private_data;
	struct vs_sample sample;

	mutex_lock(&dev->lock);
	if (dev->mode != VS_MODE_DIAGNOSTIC) {
		mutex_unlock(&dev->lock);
		return -EPERM;
	}
	mutex_unlock(&dev->lock);

	if (count < sizeof(struct vs_sample))
		return -EINVAL;

	if (copy_from_user(&sample, buf, sizeof(struct vs_sample)))
		return -EFAULT;

	vs_ringbuf_push(&dev->ring, &sample);
	wake_up_interruptible(&dev->read_wait);

	return sizeof(struct vs_sample);
}

__poll_t vs_poll(struct file *file, poll_table *wait)
{
	struct vs_device *dev = file->private_data;
	__poll_t mask = 0;
	unsigned long flags;

	poll_wait(file, &dev->read_wait, wait);

	spin_lock_irqsave(&dev->ring.lock, flags);
	if (dev->ring.count > 0)
		mask |= EPOLLIN | EPOLLRDNORM;
	spin_unlock_irqrestore(&dev->ring.lock, flags);

	return mask;
}
