# ASSIGNMENT 01 — TASK 4: THEORY QUESTIONS 
## Question 1 — Kernel module reference counting 

When a process opens a device whose `file_operations.owner` points to `THIS_MODULE`, the kernel generally obtains a reference that prevents the module from being unloaded while that file is in use. The reference is released when the file is closed and the corresponding reference is dropped. Other kernel users may explicitly call `try_module_get()`/`module_put()` when appropriate. If `rmmod` is attempted while the module is in use, normal unloading fails with a busy/in-use error. The safe solution is to stop the process, close its file descriptors, and unload again. Some kernels can be built with forced-unload support, but force removal is unsafe because executing code or open handles may still reference the module; it should not be used as an ordinary solution.

## Question 2 — Why `copy_to_user()` rather than `memcpy()`? 

The userspace pointer supplied to an ioctl is not an ordinary trusted kernel pointer. It may be invalid, unmapped, read-only or change while the kernel accesses it. The MMU translates virtual addresses according to the active address-space mappings and permission bits; a direct kernel dereference may trigger a page fault or violate memory protection. `copy_to_user()` uses architecture-specific user-memory access handling, checks access constraints, and reports bytes not copied when a fault occurs, allowing the driver to return `-EFAULT`. A direct `memcpy()` does not provide the required user-access fault handling and is not an appropriate substitute. `copy_to_user()` is necessary but does not make the content semantically trustworthy.

## Question 3 — MTD versus the block-device subsystem 

The block layer exposes sector-oriented random-access storage such as eMMC, SD cards and SSDs. MTD exposes characteristics of raw flash and similar memory devices, including erase-before-write behavior, erase blocks, bad blocks and device-specific management. Raw parallel NAND flash generally needs the MTD subsystem and a flash-aware filesystem such as UBIFS (often through UBI), because ordinary block writes cannot directly handle NAND erase and bad-block semantics. An eMMC chip, although internally built from flash, normally appears to Linux as a block device because its controller hides raw flash management.

## Question 4 — Why must counter_driver.read() handle `*ppos`? 

Programs such as `cat` repeatedly call `read(fd, buf, size)` until the call returns **0**, indicating end-of-file. The VFS passes the current file position to the driver's read callback as `*ppos` and retains the updated position for the next call. If the driver returns the counter string on every invocation without recognizing a completed read, `cat` may continue printing values indefinitely because it never receives EOF. A typical design returns 0 when `*ppos > 0`; otherwise it formats the counter value and uses `simple_read_from_buffer()` or equivalent logic to copy data and advance the offset. Each newly opened file starts with its own offset at zero, so a fresh `cat /dev/counter` can read and increment the counter once.

## Technical notes

- These explanations describe standard Linux kernel behavior; verify the exact kernel configuration before claiming forced unload is enabled.
- For Task 4 Q2, the relevant `ASGN1_GET_STATS` implementation must check `copy_to_user()` failure and return `-EFAULT`.
- For Task 4 Q4, the counter's actual offset logic should be cross-checked against the submitted `counter_driver.c`.
