# LOS201 — Embedded Linux Systems

> **ARMv7 Embedded Linux | Kernel & Bootloader | Character Drivers | Debugging | IOCTL & Misc Devices | Sensor Monitoring**

This repository collects the laboratory exercises and assignments for **LOS201 — Embedded Linux Systems**. The work progresses from booting a minimal ARM Linux system in QEMU to implementing kernel-space interfaces, debugging embedded C applications, and integrating a simulated sensor driver with a userspace monitoring application.

**Repository:** [HoaqThan/LOS201](https://github.com/HoaqThan/LOS201)  
**Target platform:** ARMv7-A / Cortex-A9 (`qemu-system-arm -M vexpress-a9`)  
**Primary language:** C, with shell scripts and Makefiles.

## Contents

1. [Repository map](#1-repository-map)
2. [Development environment and architecture](#2-development-environment-and-architecture)
3. [Lab 01 — Kernel, U-Boot and minimal Linux boot](#3-lab-01--kernel-u-boot-and-minimal-linux-boot)
4. [Lab 02 — Character device driver and file systems](#4-lab-02--character-device-driver-and-file-systems)
5. [Lab 03 — Debugging and performance analysis](#5-lab-03--debugging-and-performance-analysis)
6. [Assignment 01 — Advanced Linux device drivers](#6-assignment-01--advanced-linux-device-drivers)
7. [Assignment 02 — Sensor Monitoring System](#7-assignment-02--sensor-monitoring-system)
8. [Build, boot and verification guide](#8-build-boot-and-verification-guide)
9. [Evidence and verification scope](#9-evidence-and-verification-scope)

---

## 1. Repository map

| Coursework | Actual path on `main` | Description |
|---|---|---|
| Lab 01 | [`LAB_1/`](LAB_1/) | ARM Linux kernel setup, U-Boot, BusyBox root filesystem and QEMU boot |
| Lab 02 | [`LAB_2/`](LAB_2/) | Character device driver, `/proc`, `/sys`, MTD/JFFS2, BusyBox init |
| Lab 03 | [`Lab3_Final/`](Lab3_Final/) | GDB, Valgrind, strace, perf, latency and stress analysis |
| Assignment 01 | [`ASSIGNMENT_1/`](ASSIGNMENT_1/) | IOCTL extensions, misc counter driver, design analysis, theory |
| Assignment 02 | [`embedded_asgn2/`](embedded_asgn2/) | Sensor kernel driver and userspace monitoring application |

**Naming note:** Lab 03 is `Lab3_Final`, not `LAB_3`; Assignment 02 is `embedded_asgn2`, not `ASSIGNMENT_2`.

**Important repository note:** The supplied recursive Git listing includes the `ASSIGNMENT_1` entry at the repository root but does **not** list its nested files. It may be a Git submodule/gitlink or another special tree entry. See [Assignment 01](#6-assignment-01--advanced-linux-device-drivers) and [Verification](#9-evidence-and-verification-scope) before assuming that all its source files are present on `main`.

## 2. Development environment and architecture

| Component | Environment used / targeted |
|---|---|
| Development host | Ubuntu Linux, x86-64 |
| Target CPU | ARMv7-A Cortex-A9 |
| Emulator | QEMU system emulation, `vexpress-a9` |
| Linux kernel | Linux 5.15 |
| Bootloader | U-Boot 2022.04 |
| Minimal userspace | BusyBox 1.35.0, initramfs |
| Cross-compiler | `arm-linux-gnueabihf-` (GCC 11 used during development) |
| Kernel build | Kbuild, `ARCH=arm`, `CROSS_COMPILE=arm-linux-gnueabihf-` |
| Debug/profiling | GDB multiarch, gdbserver, Valgrind, strace, perf, cyclictest, gnuplot |
| Application programming | C, POSIX threads/IPC (Assignment 02 scope) |

### Learning progression

```text
Lab 01: ARM boot environment
   Kernel 5.15 + DTB + U-Boot + BusyBox + initramfs
                          |
                          v
Lab 02: Kernel-space device interfaces
   /dev/lab2 + procfs + sysfs + MTD/JFFS2
                          |
            +-------------+-------------+
            |                           |
            v                           v
Lab 03: Debug & profiling       Assignment 01: Driver APIs
   GDB / Valgrind / perf            ioctl / miscdevice
            |                           |
            +-------------+-------------+
                          |
                          v
Assignment 02: Integrated Sensor Monitoring System
   Sensor kernel module + C application + IPC/logging
```

The diagram describes the **coursework progression**, not a claim that every requested feature passed an automated test.

---

## 3. Lab 01 — Kernel, U-Boot and minimal Linux boot

**Directory:** [`LAB_1/`](LAB_1/)  
**Course chapters:** 4–7 (Kernel, initialization, userspace and bootloaders)

### Objective

Build and boot a minimal embedded Linux environment for ARM Cortex-A9, understanding the path from the bootloader to a userspace shell.

### Work covered

**Part A — Kernel configuration and compilation**

- Set up ARM cross-compilation using `ARCH=arm` and `CROSS_COMPILE=arm-linux-gnueabihf-`.
- Configure Linux 5.15 for the emulated ARM platform; inspect options with `menuconfig`.
- Build a compressed `zImage` and the `vexpress-v2p-ca9.dtb` Device Tree.
- Understand how the kernel initializes devices and hands control to userspace.

**Part B — U-Boot bootloader**

- Work with U-Boot 2022.04 for ARM and the `vexpress-a9` environment.
- Inspect bootloader variables such as `bootargs` and `bootcmd`.
- Understand the bootloader's role in supplying kernel boot parameters and Device Tree data.
- Investigate the boot sequence and kernel console output.

**Part C — BusyBox root filesystem and initramfs**

- Build a minimal ARM userspace using BusyBox 1.35.0.
- Prepare `/bin`, `/sbin`, `/etc`, `/proc`, `/sys`, `/dev` and an init entry point.
- Package the root filesystem as a compressed `cpio` initramfs.
- Boot into a shell using QEMU and verify with `uname`, `mount`, `ps` and boot logs.

### Recorded development results

Previous build and boot sessions recorded a working **Linux 5.15 `zImage` (~4.8 MB)**, **Device Tree (~14 KB)** and **compressed initramfs (~1.1 MB)**, with a QEMU ARM boot reaching the BusyBox environment. These figures describe the recorded development environment, **not independently verified current file sizes in the GitHub tree**.

### Repository evidence

- [`LAB_1/uboot/u-boot-2022.04/`](LAB_1/uboot/u-boot-2022.04/) — tracked U-Boot 2022.04 source tree.
- The supplied Git listing contains many U-Boot source files; it does not establish that the generated kernel, BusyBox and initramfs artifacts are all committed inside `LAB_1/`.

### Typical boot command

Run this **on the Ubuntu host** after producing the files in the indicated local directory:

```bash
qemu-system-arm \
  -M vexpress-a9 -m 512M -nographic \
  -kernel ~/embedded_lab1/output/zImage \
  -dtb ~/embedded_lab1/output/vexpress-v2p-ca9.dtb \
  -initrd ~/embedded_lab1/output/initramfs.cpio.gz \
  -append 'console=ttyAMA0 rdinit=/init'
```

The paths above are from the original local Lab 01 workflow; adjust them if the build outputs were moved. The course also explores booting through U-Boot as a separate exercise.

---

## 4. Lab 02 — Character device driver and file systems

**Directory:** [`LAB_2/`](LAB_2/)  
**Course chapters:** 8–11 (Drivers, filesystems, MTD and BusyBox init)

### Objective

Develop a loadable Linux kernel **character device driver**, expose device statistics to userspace and experiment with embedded flash filesystems.

### Part A — Character device driver

The `lab2_driver` kernel module supports character-device operations through `file_operations`:

| Interface | Role |
|---|---|
| `open()` | Open the device and track accesses |
| `read()` | Copy buffered data from kernel to userspace |
| `write()` | Copy data from userspace into the driver buffer |
| `release()` | Close the device |
| `module_init()` / `module_exit()` | Register and clean up the driver |
| `mutex` | Protect shared buffer and statistics |

The development implementation uses `/dev/lab2`, **major number 240**, and a **1024-byte internal buffer**. Kernel/user transfers use `copy_to_user()` and `copy_from_user()`.

### Part B — procfs and sysfs

- `/proc/lab2_info`: human-readable module and buffer statistics, using the `seq_file` pattern.
- `/sys/class/lab2_class/lab2/`: individual attributes exposed through sysfs.
- Recorded attributes include `buffer_len`, `buffer_size`, `open_count`, `last_data`, `read_only`, `word_count` and `status` in the extended driver exercises. Attribute availability may vary between the checked-in Lab 02 version and later exam-practice revisions.
- The `read_only` extension was tested with allowed/blocked writes; `word_count` counts words in the current buffer; `status` reports `EMPTY` or `DATA`.

### Part C — MTD and JFFS2

- Explore the Memory Technology Device subsystem and raw flash concepts.
- Use NAND simulation and MTD tools to create and inspect a JFFS2 filesystem.
- Format, mount, write, unmount and remount a test filesystem.
- A recorded experiment used an emulated NAND device and checked persistence after remounting.

### Part D — BusyBox startup integration

- Integrate the module into the ARM initramfs.
- Configure startup scripts and BusyBox init to initialize the environment and load the driver.
- Test module load/unload and device access from the QEMU shell.

### Confirmed repository artifacts

| File | Purpose |
|---|---|
| [`driver/lab2_driver.c`](LAB_2/driver/lab2_driver.c) | Character driver source |
| [`driver/Makefile`](LAB_2/driver/Makefile) | Kernel module build |
| [`driver/lab2_driver.ko`](LAB_2/driver/lab2_driver.ko) | Compiled ARM kernel module |
| [`output/initramfs_lab2.cpio.gz`](LAB_2/output/initramfs_lab2.cpio.gz) | Compressed root filesystem |
| [`rootfs/initramfs/`](LAB_2/rootfs/initramfs/) | Embedded userspace files |

### Example checks (inside the ARM target)

```sh
insmod /lib/modules/lab2_driver.ko  # adjust to installed module path
ls -l /dev/lab2
printf 'Embedded Linux' > /dev/lab2
cat /dev/lab2
cat /proc/lab2_info
ls /sys/class/lab2_class/lab2/
dmesg | grep lab2
```

The device node may need to be created if startup scripts or devtmpfs have not created it. Use `mknod /dev/lab2 c 240 0` only when appropriate for the running driver.

### Recorded verification

The Lab 02 development session demonstrated successful module loading, reads/writes through `/dev/lab2`, procfs statistics and sysfs attribute queries. A later practice driver also demonstrated `read_only`, `word_count` and `status`; those extensions should not automatically be attributed to the committed `LAB_2/driver/lab2_driver.c` without checking its contents.

---

## 5. Lab 03 — Debugging and performance analysis

**Directory:** [`Lab3_Final/`](Lab3_Final/)  
**Course chapters:** 16–19 (Memory, development tools, kernel debugging, real-time)

### Objective

Diagnose embedded C bugs, memory-management errors, system-call behavior, CPU overhead and timing latency using standard Linux tools.

### Part A — ARM application debugging with GDB

- Build a deliberately buggy C application with debug symbols.
- Investigate incorrect array bounds and other memory/logic defects.
- Use GDB breakpoints, variable inspection, stack analysis and stepping.
- Compare original and corrected code.
- The recorded ARM debug session identified an off-by-one loop condition (`i <= len`) in `process_data()`.

**Files:** [`apps/buggy_app.c`](Lab3_Final/apps/buggy_app.c), [`apps/buggy_app_fixed.c`](Lab3_Final/apps/buggy_app_fixed.c), [`logs/gdb_session.txt`](Lab3_Final/logs/gdb_session.txt).

### Part B — Memory leak analysis with Valgrind

- Compile the test application with debugging information.
- Use Valgrind Memcheck to identify leaks or invalid memory operations.
- Correct allocation/deallocation mistakes and compare reports.
- Record host-side memory diagnostics; distinguish host x86-64 analysis from ARM-target execution.

**Files:** [`apps/leaky_app.c`](Lab3_Final/apps/leaky_app.c), [`apps/leaky_app_fixed.c`](Lab3_Final/apps/leaky_app_fixed.c), [`logs/valgrind_report.txt`](Lab3_Final/logs/valgrind_report.txt), [`logs/valgrind_fixed.txt`](Lab3_Final/logs/valgrind_fixed.txt), [`logs/arm_memory_report.txt`](Lab3_Final/logs/arm_memory_report.txt).

### Part C — System-call tracing and profiling

- Run `strace` to observe system calls and their return values.
- Collect `perf` statistics and profile a CPU workload.
- Compare timing and resource-use observations with the application's behavior.

**Files:** [`apps/perf_target.c`](Lab3_Final/apps/perf_target.c), [`logs/strace_full.txt`](Lab3_Final/logs/strace_full.txt), [`logs/strace_summary.txt`](Lab3_Final/logs/strace_summary.txt), [`logs/perf_stat.txt`](Lab3_Final/logs/perf_stat.txt), [`logs/perf_report.txt`](Lab3_Final/logs/perf_report.txt).

### Part D — Scheduling latency and stress

- Measure scheduling latency with the `cyclictest` workflow.
- Compare baseline and stressed measurements.
- Generate a histogram using gnuplot.
- Discuss standard-kernel versus PREEMPT_RT behavior; **no completed PREEMPT_RT kernel comparison is asserted here**.

**Files:** [`logs/hist_standard.txt`](Lab3_Final/logs/hist_standard.txt), [`logs/hist_stress.txt`](Lab3_Final/logs/hist_stress.txt), [`logs/latency_histogram.png`](Lab3_Final/logs/latency_histogram.png), [`logs/plot_latency.gnuplot`](Lab3_Final/logs/plot_latency.gnuplot), [`logs/stress_report.txt`](Lab3_Final/logs/stress_report.txt).

### Repository documentation

Read the lab-specific [`Lab3_Final/README.md`](Lab3_Final/README.md) and the associated `logs/` for the exact commands, measurements and recorded output.

---

## 6. Assignment 01 — Advanced Linux device drivers

**Repository entry:** [`ASSIGNMENT_1/`](ASSIGNMENT_1/)  
**Course chapters:** 8–10 (Linux drivers, interfaces and device models)

### Objective

Extend the Lab 02 driver with a structured **IOCTL control interface**, implement a separate misc-device counter driver, and document design decisions.

### Task 1 — IOCTL-enabled character driver (40 points)

The working project was organized locally as `ASSIGNMENT_1/TASK_1` and used `asgn1_driver.c`, `asgn1_ioctl.h` and a kernel-module Makefile.

| IOCTL command | Required operation | Development evidence |
|---|---|---|
| `ASGN1_RESET_BUFFER` | Clear buffer and reset read/write counters | Implemented and rebuilt in the recorded working copy |
| `ASGN1_GET_STATS` | Return `open_count`, `write_count`, `read_count`, `buffer_len`, `last_write_size` | Implemented with `copy_to_user()` and rebuilt |
| `ASGN1_SET_MODE` | Switch between prefixed kernel mode and raw echo mode | Present as a defined command; complete behavior not confirmed in the latest available code snapshot |
| `ASGN1_GET_VERSION` | Return a bounded driver version string | Present as a defined command; complete behavior not confirmed in the latest available code snapshot |

Additional design elements: `unlocked_ioctl`, an IOCTL header shared with userspace, `mutex` protection, and kernel log messages. The recorded working copy retained `/dev/lab2` and major number 240 from Lab 02; **the assignment specification instead requests `/dev/asgn1` and a distinct major number (241–254)**. This difference should be checked before grading.

### Task 2 — Misc counter device (30 points)

The assignment specification requires a separate `counter_driver` using `miscdevice`, a counter stored as `atomic_t`, read/write behavior and procfs reporting. **Its actual implementation and runtime results have not been established from the supplied repository listing.**

### Task 3 — Driver design and analysis (20 points)

The required technical discussion covers architecture, interfaces, design trade-offs and comparisons among driver registration models. No particular report file is claimed here without a verified file path.

### Task 4 — Theory (10 points)

The written component explains Linux device driver concepts and design choices. Its submitted answers have not been independently inspected.

### Important verification note

`ASSIGNMENT_1` appears as a top-level entry in the supplied `git ls-tree` output, but no `ASSIGNMENT_1/...` files appear in the recursive file list. Therefore **do not infer that all four tasks are committed and complete**. On a local clone, inspect the entry with:

```bash
git ls-tree HEAD ASSIGNMENT_1
git submodule status
find ASSIGNMENT_1 -maxdepth 3 -type f | sort
```

If the tree entry has mode `160000`, it is a Git submodule/gitlink; its files are tracked in another repository, not as normal files in this repository tree.

---

## 7. Assignment 02 — Sensor Monitoring System

**Directory:** [`embedded_asgn2/`](embedded_asgn2/)  
**Course chapters:** 8–19 (Drivers, concurrency, memory, debugging and real-time)

### Objective

Build an ARM Linux **Sensor Monitoring System (SMS)** integrating a simulated sensor kernel module with a C userspace application. The specification combines data acquisition, concurrent processing, threshold alerts, logging and performance analysis.

### Architecture described by the assignment

```text
                         ARM Linux (QEMU)
+------------------------------------------------------------+
| Kernel space                                               |
|  sms_sensor_driver.ko                                      |
|    |                                                       |
|    +--> /dev/sms_sensor     (simulated sensor readings)     |
|    +--> /proc/sms_stats     (driver statistics, specified)  |
+----------------------------|-------------------------------+
                             | read()
+----------------------------v-------------------------------+
| Userspace: sms_app                                         |
|                                                            |
|  Sensor Thread --> Shared Circular Buffer --> Processor    |
|                      (mutex + condvar)         Thread       |
|                                                 |          |
|                                                 v          |
|                                           Alarm Queue      |
|                                                 |          |
|                                                 v          |
|                                            Alert Thread    |
|                                                 |          |
|                                                 v          |
|                                            Logger Thread   |
|                                                 |          |
|                                                 v          |
|                                             /tmp/sms.log   |
+------------------------------------------------------------+
```

This is the **target architecture from the assignment brief**, not a line-by-line validation of the checked-in application.

### Task 1 — Multithreaded application and architecture (30 points)

- **Kernel driver:** expose simulated sensor readings through a character device.
- **Sensor thread:** periodically read sensor data and place it into a shared buffer.
- **Processor thread:** consume readings, compute statistics and detect threshold violations.
- **Alert thread:** receive and handle alarm events.
- **Logger thread:** persist monitoring messages.
- **Synchronization:** mutex and condition variable for shared data; POSIX message queue for alarms (per specification).

### Task 2 — Memory management and driver integration (20 points)

- Integrate the application with the sensor device.
- Manage buffers, allocations, cleanup and resource ownership.
- Test device open/read/close behavior and inspect driver statistics.

### Task 3 — Debugging and profiling (20 points)

- Inspect thread execution with GDB.
- Trace application system calls using `strace`.
- Analyze CPU and runtime behavior using `perf` and related tools.

### Task 4 — Real-time analysis (15 points)

- Investigate periodic sensor acquisition and scheduling latency.
- Evaluate real-time scheduling approaches, including `SCHED_FIFO` as specified.
- Discuss limitations, trade-offs and improvements.

### Task 5 — Technical report and demo (15 points)

- Document architecture, synchronization, data flow and testing.
- Demonstrate kernel module integration and application behavior.
- Prepare the specified 3–5 minute demo video and report.

### Confirmed repository artifacts

| File | Role |
|---|---|
| [`app/sms_app.c`](embedded_asgn2/app/sms_app.c) | Userspace monitoring source |
| [`app/sms_app`](embedded_asgn2/app/sms_app) | Compiled application |
| [`driver/sms_sensor_driver.c`](embedded_asgn2/driver/sms_sensor_driver.c) | Simulated sensor driver source |
| [`driver/sms_sensor_driver.ko`](embedded_asgn2/driver/sms_sensor_driver.ko) | Compiled kernel module |
| [`driver/Makefile`](embedded_asgn2/driver/Makefile) | Kernel module build rules |
| [`output/initramfs_asgn2.cpio.gz`](embedded_asgn2/output/initramfs_asgn2.cpio.gz) | Packaged ARM root filesystem |

The files establish that both the driver and application were developed and built. The listing alone **does not prove** that all four threads, IPC, logging, real-time scheduling, profiling and report/demo requirements have passed.

### Quick inspection commands

```bash
# On the Ubuntu host, from the repository root:
file embedded_asgn2/app/sms_app
file embedded_asgn2/driver/sms_sensor_driver.ko
sed -n '1,180p' embedded_asgn2/app/sms_app.c
sed -n '1,180p' embedded_asgn2/driver/sms_sensor_driver.c
```

To run the application, boot a matching ARM kernel with the packaged root filesystem, load the sensor module, verify the device node and execute the app **inside the ARM target**. Do not try to load an ARM `.ko` into the x86-64 Ubuntu host kernel.

---

## 8. Build, boot and verification guide

### Clone and inspect the repository

```bash
git clone https://github.com/HoaqThan/LOS201.git
cd LOS201

git ls-tree --name-only HEAD
find Lab3_Final -maxdepth 2 -type f | sort
```

### Toolchain and dependencies

Typical Ubuntu packages for these exercises:

```bash
sudo apt update
sudo apt install -y build-essential git make \
  gcc-arm-linux-gnueabihf binutils-arm-linux-gnueabihf \
  qemu-system-arm cpio bison flex libncurses-dev \
  libssl-dev libelf-dev mtd-utils gdb-multiarch \
  valgrind strace rt-tests gnuplot
```

The kernel source and target-specific module build environment must match the ARM kernel actually booted in QEMU. `perf` and `gdbserver` availability depends on the host/target environment.

### Build a kernel module

Example for Lab 02 (uses the Makefile stored in the repository):

```bash
cd LAB_2/driver
make
file lab2_driver.ko
```

If the Makefile expects `~/embedded_lab1/kernel/linux-5.15`, ensure that kernel tree exists and has been configured/built for ARM. A `.ko` artifact is specific to the kernel version/configuration used to compile it.

### Check Lab 03 evidence

```bash
cd Lab3_Final
cat README.md
cat logs/gdb_session.txt
cat logs/valgrind_report.txt
cat logs/valgrind_fixed.txt
cat logs/perf_stat.txt
```

### Inspect Assignment 02

```bash
cd embedded_asgn2
file app/sms_app
file driver/sms_sensor_driver.ko
```

### Useful target-side commands

```sh
uname -a
cat /proc/version
lsmod
cat /proc/mtd
dmesg | tail -50
```

**Note:** This repository contains coursework source, generated outputs and logs. It is **not** a single top-level `make` project; each lab/assignment has its own build and runtime assumptions.

---

## 9. Evidence and verification scope

This README distinguishes three types of information:

1. **Repository-confirmed:** paths visible in the supplied `git ls-tree -r --name-only` output from `main` (e.g., Lab 02 driver, Lab 03 logs, Assignment 02 app/driver).
2. **Recorded development:** earlier build/test commands and outputs from the implementation sessions (e.g., Lab 01 boot, Lab 02 driver tests, Assignment 01 IOCTL progress).
3. **Specification scope:** functionality explicitly requested by the five coursework documents, but not necessarily confirmed by a source inspection or passing test.

### Current verification summary

| Component | What is supported by available evidence | What remains unverified |
|---|---|---|
| Lab 01 | Tracked U-Boot 2022.04 source; previously recorded working ARM boot artifacts | Current Git-tracked kernel/rootfs outputs and exact boot log |
| Lab 02 | Driver C source, Makefile, `.ko`, initramfs; recorded device/procfs/sysfs tests | Whether every later exam-practice extension is in the committed driver; complete MTD/auto-load evidence |
| Lab 03 | Original/fixed C sources and GDB, Valgrind, strace, perf, latency logs | Exact numerical outcomes without reading each log; PREEMPT_RT comparison |
| Assignment 01 | Top-level entry and recorded local Task 1 IOCTL development | Nested tracked files; full four-command test; Task 2–4 deliverables |
| Assignment 02 | Driver source and `.ko`, app source and binary, packaged initramfs | Full source-level validation of threading/IPC/RT and runtime demo/report |

### Coursework references

The section structure follows the supplied course briefs:

- **LAB01_EmbeddedLinux** — Kernel Configuration, U-Boot, BusyBox and QEMU.
- **LAB02_EmbeddedLinux** — Character Driver, procfs/sysfs, MTD/JFFS2 and BusyBox init.
- **LAB03_EmbeddedLinux** — GDB, Valgrind, strace/perf and cyclictest.
- **ASSIGNMENT01_EmbeddedLinux** — IOCTL, Misc Counter Driver, Analysis and Theory.
- **ASSIGNMENT02_EmbeddedLinux** — Sensor Monitoring System, Memory, Debugging, Real-Time and Demo.

---

**LOS201 — Embedded Linux Systems**  
*From ARM Linux boot to kernel-driver development and integrated embedded applications.*
