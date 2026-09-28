# 03 - Driver Design & Concurrency

## Concurrency & Locking Strategy

1. **`spinlock_t lock` (Ring Buffer)**:
   - **Context**: Access from interrupt context / workqueue thread (`vs_gen`) and user process syscall context (`vs_read`, `vs_poll`, `vs_ioctl`).
   - **Protection**: `spin_lock_irqsave(&rb->lock, flags)` guarantees safe head/tail pointers and overrun tracking without deadlock.

2. **`struct mutex lock` (Device State & Config)**:
   - **Context**: Process context only (`vs_ioctl`, `vs_state_set_mode`, `vs_state_set_rate`).
   - **Protection**: Can sleep while holding mutex. Prevents configuration race conditions.

3. **`wait_queue_head_t read_wait`**:
   - **Context**: Used to block readers in `vs_read` when ring buffer is empty (`wait_event_interruptible`) and woken up by generator workqueue thread (`wake_up_interruptible`).

## Unwind & Cleanup
During `vsensor_exit` or module initialization failure paths:
1. `hrtimer_cancel` stops the high-resolution timer.
2. `cancel_work_sync` waits for pending sample generation work to complete.
3. `wake_up_interruptible_all` unblocks all sleeping reader processes.
4. Device nodes and regions are cleaned up in reverse order of allocation.
