#include "vsensor_internal.h"

void vs_ringbuf_init(struct vs_ringbuf *rb)
{
	rb->head = 0;
	rb->tail = 0;
	rb->count = 0;
	rb->overruns = 0;
	spin_lock_init(&rb->lock);
}

bool vs_ringbuf_push(struct vs_ringbuf *rb, const struct vs_sample *s)
{
	unsigned long flags;
	bool sample_dropped = false;

	spin_lock_irqsave(&rb->lock, flags);

	if (rb->count == VS_RING_CAPACITY) {
		/* Ring buffer full: drop oldest sample (advance tail) and count overrun */
		rb->tail = (rb->tail + 1) % VS_RING_CAPACITY;
		rb->count--;
		rb->overruns++;
		sample_dropped = true;
	}

	rb->samples[rb->head] = *s;
	rb->head = (rb->head + 1) % VS_RING_CAPACITY;
	rb->count++;

	spin_unlock_irqrestore(&rb->lock, flags);
	return sample_dropped;
}

bool vs_ringbuf_pop(struct vs_ringbuf *rb, struct vs_sample *s)
{
	unsigned long flags;
	bool success = false;

	spin_lock_irqsave(&rb->lock, flags);

	if (rb->count > 0) {
		*s = rb->samples[rb->tail];
		rb->tail = (rb->tail + 1) % VS_RING_CAPACITY;
		rb->count--;
		success = true;
	}

	spin_unlock_irqrestore(&rb->lock, flags);
	return success;
}

bool vs_ringbuf_peek_latest(struct vs_ringbuf *rb, struct vs_sample *s)
{
	unsigned long flags;
	bool success = false;
	u32 idx;

	spin_lock_irqsave(&rb->lock, flags);

	if (rb->count > 0) {
		idx = (rb->head + VS_RING_CAPACITY - 1) % VS_RING_CAPACITY;
		*s = rb->samples[idx];
		success = true;
	}

	spin_unlock_irqrestore(&rb->lock, flags);
	return success;
}

void vs_ringbuf_clear(struct vs_ringbuf *rb)
{
	unsigned long flags;

	spin_lock_irqsave(&rb->lock, flags);
	rb->head = 0;
	rb->tail = 0;
	rb->count = 0;
	spin_unlock_irqrestore(&rb->lock, flags);
}
