# 06 - Computer Architecture & Systems Mapping

## 1. User Mode vs Kernel Mode Boundary
User programs run in CPU Ring 3 (unprivileged user space). The Linux kernel module executes in Ring 0 (privileged kernel space).
System call boundary transitions (`open`, `read`, `write`, `ioctl`, `poll`) trap into kernel space via software interrupts/sysenter.

## 2. Syscall Boundary Data Transfer (`copy_to_user` / `copy_from_user`)
Kernel code cannot directly dereference user-space pointers. `copy_to_user` and `copy_from_user` validate user virtual memory mapping, check page permissions, and handle page faults safely.

## 3. Kernel Memory Allocation
`vsensor_main.c` uses `kzalloc(..., GFP_KERNEL)` for kernel-space allocation of `struct vs_device`.

## 4. I/O Abstraction & Character Devices
Unix philosophy presents devices as files. `alloc_chrdev_region`, `cdev_init`, `cdev_add`, and `device_create` expose VirtualSensorX under `/dev/virtualsensor`.

## 5. Interrupt-Like Behavior (Top/Bottom Half Simulation)
Without physical hardware interrupts, an `hrtimer` acts as the hardware clock trigger (top half), scheduling a workqueue item (`schedule_work`) to execute bottom-half work (`vs_work_func`) in process context.

## 6. Concurrency & Synchronization
Process context uses blocking `struct mutex` for state modifications. High-resolution timer and workqueue contexts use `spin_lock_irqsave` for atomic, non-blocking ring buffer synchronization.
