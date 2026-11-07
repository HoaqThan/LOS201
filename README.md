# LOS201 — Embedded Linux Systems

A collection of practical labs and assignments for **LOS201 (Embedded Linux Systems)**, covering ARM Linux boot, character device drivers, debugging and performance analysis, advanced driver interfaces, and a sensor-monitoring application.

**Repository:** https://github.com/HoaqThan/LOS201

## Project navigation

| Component | Repository directory | Focus |
| --- | --- | --- |
| Lab 01 | [LAB_1](LAB_1/) | ARM Linux kernel, U-Boot, BusyBox, initramfs, QEMU |
| Lab 02 | [LAB_2](LAB_2/) | Character device driver, procfs/sysfs, flash filesystem |
| Lab 03 | [Lab3_Final](Lab3_Final/) | GDB, memory debugging, tracing, profiling, latency |
| Assignment 01 | [ASSIGNMENT_1](ASSIGNMENT_1/) | IOCTL, misc device and driver analysis |
| Assignment 02 | [embedded_asgn2](embedded_asgn2/) | Multithreaded sensor monitoring and kernel integration |

> **Directory naming:** Lab 03 is stored in `Lab3_Final/` and Assignment 02 in `embedded_asgn2/`, not `LAB_3/` and `ASSIGNMENT_2/`.

## Development environment

- **Target:** ARMv7-A / Cortex-A9, QEMU `vexpress-a9`
- **Kernel:** Linux 5.15
- **Toolchain:** `arm-linux-gnueabihf-` cross-compiler
- **Boot and userspace:** U-Boot 2022.04, BusyBox 1.35.0, initramfs
- **Debugging and analysis:** GDB multiarch, Valgrind, strace, perf, cyclictest and gnuplot
- **Languages:** C, shell scripts and Makefiles

## Lab 01 — Embedded Linux boot environment

**Location:** [`LAB_1/`](LAB_1/)

Lab 01 establishes the ARM embedded Linux environment used by subsequent labs. It covers kernel configuration and cross-compilation, U-Boot, a minimal BusyBox root filesystem, initramfs packaging, and QEMU boot on the `vexpress-a9` machine. The repository includes the U-Boot 2022.04 source tree under [`LAB_1/uboot/u-boot-2022.04/`](LAB_1/uboot/u-boot-2022.04/).

**Key concepts:** bootloader → kernel → root filesystem → init process → shell; Device Tree, kernel boot arguments, cross-compilation and serial-console logs.

## Lab 02 — Character device driver and filesystem

**Location:** [`LAB_2/`](LAB_2/)

Lab 02 implements an ARM Linux character device driver and explores filesystem interfaces. The checked-in driver sources and build outputs include:

- [`LAB_2/driver/lab2_driver.c`](LAB_2/driver/lab2_driver.c) — kernel driver source.
- [`LAB_2/driver/Makefile`](LAB_2/driver/Makefile) — module build configuration.
- [`LAB_2/driver/lab2_driver.ko`](LAB_2/driver/lab2_driver.ko) — built kernel module.
- [`LAB_2/output/initramfs_lab2.cpio.gz`](LAB_2/output/initramfs_lab2.cpio.gz) — packaged root filesystem.

The lab addresses `open`, `read`, `write` and `release` operations, kernel/user memory copying, driver statistics exposed through procfs/sysfs, and MTD/JFFS2 filesystem experiments. The character device used during development was `/dev/lab2`, with a 1024-byte internal buffer and major number 240.

## Lab 03 — Debugging and performance analysis

**Location:** [`Lab3_Final/`](Lab3_Final/)

Lab 03 contains a debugging and profiling exercise with buggy C applications, corrected variants and recorded test output.

**Source and documentation:**

- [`Lab3_Final/README.md`](Lab3_Final/README.md) — lab-specific documentation.
- [`Lab3_Final/apps/buggy_app.c`](Lab3_Final/apps/buggy_app.c) and [`buggy_app_fixed.c`](Lab3_Final/apps/buggy_app_fixed.c) — buggy and corrected application.
- [`Lab3_Final/apps/leaky_app.c`](Lab3_Final/apps/leaky_app.c) and [`leaky_app_fixed.c`](Lab3_Final/apps/leaky_app_fixed.c) — memory-management exercises.
- [`Lab3_Final/logs/gdb_session.txt`](Lab3_Final/logs/gdb_session.txt) — debugger session.
- [`Lab3_Final/logs/valgrind_report.txt`](Lab3_Final/logs/valgrind_report.txt) and [`valgrind_fixed.txt`](Lab3_Final/logs/valgrind_fixed.txt) — memory-analysis results.
- [`Lab3_Final/logs/strace_full.txt`](Lab3_Final/logs/strace_full.txt) and [`perf_stat.txt`](Lab3_Final/logs/perf_stat.txt) — system-call and performance records.
- [`Lab3_Final/logs/latency_histogram.png`](Lab3_Final/logs/latency_histogram.png) — latency visualization.

Topics include ARM remote debugging with GDB, detecting memory errors, syscall tracing, CPU profiling, and latency measurement under standard and stressed workloads. Logs provide evidence of work performed; they should be consulted for the exact measurements and outcomes.

## Assignment 01 — Advanced Linux device drivers

**Location:** [`ASSIGNMENT_1/`](ASSIGNMENT_1/)

Assignment 01 extends the driver work from Lab 02. Its specification comprises four tasks:

1. **IOCTL interface:** implement `ASGN1_RESET_BUFFER`, `ASGN1_GET_STATS`, `ASGN1_SET_MODE`, and `ASGN1_GET_VERSION`.
2. **Misc device:** create a separate counter-style driver using the Linux miscdevice framework.
3. **Driver analysis:** document architecture, implementation choices, and comparisons.
4. **Theory:** explain core driver concepts and design decisions.

The directory exists in the repository. Individual files and test results for its tasks have **not yet been independently verified from the supplied listing**; the items above describe the assignment scope, not a claim that every task passed.

## Assignment 02 — Sensor Monitoring System

**Location:** [`embedded_asgn2/`](embedded_asgn2/)

Assignment 02 integrates an ARM kernel sensor driver with a userspace monitoring application. The checked-in deliverables include:

- [`embedded_asgn2/app/sms_app.c`](embedded_asgn2/app/sms_app.c) — userspace application source.
- [`embedded_asgn2/app/sms_app`](embedded_asgn2/app/sms_app) — built application.
- [`embedded_asgn2/driver/sms_sensor_driver.c`](embedded_asgn2/driver/sms_sensor_driver.c) — simulated sensor driver source.
- [`embedded_asgn2/driver/sms_sensor_driver.ko`](embedded_asgn2/driver/sms_sensor_driver.ko) — built kernel module.
- [`embedded_asgn2/driver/Makefile`](embedded_asgn2/driver/Makefile) — driver build configuration.
- [`embedded_asgn2/output/initramfs_asgn2.cpio.gz`](embedded_asgn2/output/initramfs_asgn2.cpio.gz) — ARM root filesystem image.

The assignment specification describes a **Sensor Monitoring System** with a sensor-reading thread, processing thread, alert thread and logging thread; shared-buffer synchronization with mutex/condition variables; POSIX message queues; threshold-based alerts; and runtime logging. The presence of application and driver artifacts is confirmed, but implementation of each concurrency feature and runtime test results should be verified against the source and demo logs before claiming full compliance.

### Intended architecture

```text
ARM Linux / QEMU
  ├── Kernel space
  │    └── Sensor driver → /dev/sms_sensor
  └── User space
       └── Sensor Monitoring Application
            ├── Sensor acquisition
            ├── Data processing
            ├── Alert handling
            └── Logging
```

## Building and testing

The project uses an ARM cross-compiler and a kernel tree configured for the same target/kernel version as the modules. Example checks:

```bash
# Inspect the actual project layout
git ls-tree --name-only HEAD

# Build the Lab 02 kernel module using its own Makefile
cd LAB_2/driver
make

# Inspect the generated module
file lab2_driver.ko
```

For Lab 03, consult [`Lab3_Final/README.md`](Lab3_Final/README.md) and the recorded `logs/`. For Assignment 02, consult the source in `embedded_asgn2/app/` and `embedded_asgn2/driver/`. Running a module requires a matching ARM Linux kernel and root filesystem; a `.ko` file cannot be loaded directly into the x86 host kernel.

## Verification notes

This README separates **files confirmed in the repository listing** from **functional requirements described by the coursework**. A file's presence does not by itself prove that a test passed. For precise results, review each lab's logs, source code, build output and demo evidence.

## Course learning outcomes

Together, these exercises cover the Linux boot sequence, kernel/user-space interfaces, character and misc drivers, filesystem integration, embedded debugging, performance measurement, multithreading, inter-process communication, and the integration of a sensor-style embedded application.
