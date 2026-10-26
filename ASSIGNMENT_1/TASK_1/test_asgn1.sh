#!/bin/sh
echo "=== KIEM THU TASK 1 ==="
insmod asgn1_driver.ko
mknod /dev/asgn1 c 241 0 2>/dev/null
chmod 666 /dev/asgn1

echo "\n--- 1. Test ioctl GET_VERSION ---"
./task1_test version

echo "\n--- 2. Test Echo Mode (Mode 1) ---"
echo "Xin chao Embedded Linux" > /dev/asgn1
echo "Doc lai: [$(cat /dev/asgn1)]"

echo "\n--- 3. Test Kernel Mode (Mode 0) ---"
./task1_test mode 0
echo "Tin nhan thu nghiem" > /dev/asgn1
echo "Doc lai: [$(cat /dev/asgn1)]"

echo "\n--- 4. Test ioctl GET_STATS ---"
./task1_test stats

echo "\n--- 5. Test ioctl RESET_BUFFER ---"
./task1_test reset
echo "Doc sau reset: [$(cat /dev/asgn1)]"
./task1_test stats

echo "\n--- 6. Cleanup ---"
rmmod asgn1_driver
dmesg | grep asgn1 | tail -n 12
echo "=== HOAN THANH ==="
