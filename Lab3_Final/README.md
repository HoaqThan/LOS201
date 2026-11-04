# Embedded Linux Lab 03
## Kernel Debugging and Application Performance Analysis

This project demonstrates debugging, memory analysis, performance profiling, and latency measurement for Embedded Linux applications.

## Development Environment

- Host: Ubuntu Linux (x86-64)
- Target: ARMv7 Cortex-A9
- ARM Kernel: Linux 5.15
- Emulator: QEMU
- Compiler: GCC ARM Cross Compiler
- Debugger: GDB Multiarch
- Tools: Valgrind, Strace, Perf, Cyclictest, stress-ng, Gnuplot

## Project Structure

- apps/: C source code and test applications
- logs/: Debugging, profiling, memory, and latency results
- output/: Generated ARM initramfs
- rootfs/: ARM root filesystem

## Part A - GDB Debugging

Three intentional bugs were investigated:

1. Out-of-bounds array access
2. Use-after-free
3. Integer overflow

The corrected implementation is available in apps/buggy_app_fixed.c.

GDB Multiarch and QEMU User Mode were used to investigate the out-of-bounds error.

Evidence: logs/gdb_session.txt

## Part B - Memory Analysis

Valgrind was used to analyze memory leaks.

The original and corrected applications are available in apps/.

Evidence:
- logs/valgrind_report.txt
- logs/valgrind_fixed.txt
- logs/arm_memory_report.txt

## Part C - Performance Profiling

The application performs mathematical computation, linear search, and file I/O.

Tools:
- Strace: System call tracing
- Perf: CPU profiling
- GNU Time: Execution time measurement

Evidence:
- logs/strace_summary.txt
- logs/perf_report.txt
- logs/perf_stat.txt
- logs/time_report.txt

The performance application was also executed on ARM QEMU.

## Part D - Real-Time Latency

Cyclictest was used to measure scheduling latency under normal and stressed conditions.

| Scenario | Maximum Latency |
|----------|-----------------|
| Normal | 169 us |
| Stress | 281 us |

The host kernel uses PREEMPT_DYNAMIC, not PREEMPT_RT.

Evidence:
- logs/hist_standard.txt
- logs/hist_stress.txt
- logs/stress_report.txt
- logs/latency_histogram.png

## Conclusion

This lab demonstrates debugging and performance analysis using GDB, Valgrind, Strace, Perf, and Cyclictest.

The project includes source code and experimental evidence for the four lab sections.
