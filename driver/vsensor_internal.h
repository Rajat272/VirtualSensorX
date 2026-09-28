/* private driver header */
#ifndef _VSENSOR_INTERNAL_H_
#define _VSENSOR_INTERNAL_H_

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/wait.h>
#include <linux/hrtimer.h>
#include <linux/workqueue.h>
#include <linux/uaccess.h>
#include <linux/poll.h>

#include "../include/uapi/virtualsensor_uapi.h"

struct vs_ringbuf {
	struct vs_sample samples[VS_RING_CAPACITY];
	u32 head;
	u32 tail;
	u32 count;
	u64 overruns;
	spinlock_t lock; /* Protects ring buffer operations across timer/wq and process context */
};

struct vs_device {
	dev_t dev_num;
	struct cdev cdev;
	struct class *class;
	struct device *device;

	struct mutex lock; /* Protects configuration, state transitions, and open count */
	wait_queue_head_t read_wait;

	enum vs_mode mode;
	u32 rate_hz;
	bool running;
	u64 samples_generated;
	u32 error_flags;
	u32 sequence_counter;

	struct vs_ringbuf ring;
	struct hrtimer timer;
	struct work_struct work;
};

extern struct vs_device *g_vs_dev;

void vs_ringbuf_init(struct vs_ringbuf *rb);
bool vs_ringbuf_push(struct vs_ringbuf *rb, const struct vs_sample *s);
bool vs_ringbuf_pop(struct vs_ringbuf *rb, struct vs_sample *s);
bool vs_ringbuf_peek_latest(struct vs_ringbuf *rb, struct vs_sample *s);
void vs_ringbuf_clear(struct vs_ringbuf *rb);

void vs_gen_init(struct vs_device *dev);
void vs_gen_cleanup(struct vs_device *dev);
void vs_gen_start(struct vs_device *dev);
void vs_gen_stop(struct vs_device *dev);

void vs_state_init(struct vs_device *dev);
int vs_state_set_mode(struct vs_device *dev, enum vs_mode mode);
int vs_state_set_rate(struct vs_device *dev, u32 rate_hz);
void vs_state_generate_sample(struct vs_device *dev, struct vs_sample *s);

long vs_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
int vs_open(struct inode *inode, struct file *file);
int vs_release(struct inode *inode, struct file *file);
ssize_t vs_read(struct file *file, char __user *buf, size_t count, loff_t *ppos);
ssize_t vs_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos);
__poll_t vs_poll(struct file *file, poll_table *wait);

#endif /* _VSENSOR_INTERNAL_H_ */
