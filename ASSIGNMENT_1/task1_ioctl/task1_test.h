#!/bin/sh
echo "=== BẮT ĐẦU TEST TASK 1: asgn1_driver ==="
insmod asgn1_driver.ko
mknod /dev/asgn1 c 241 0 2>/dev/null
chmod 666 /dev/asgn1

echo "\n--- 1. Test ioctl GET_VERSION ---"
./asgn1_test version

echo "\n--- 2. Test Echo Mode (Mode 1 - default) ---"
echo "Test Echo String" > /dev/asgn1
echo "Read back: [$(cat /dev/asgn1)]"

echo "\n--- 3. Test Kernel Mode (Mode 0) ---"
./asgn1_test mode 0
echo "Test Kernel String" > /dev/asgn1
echo "Read back: [$(cat /dev/asgn1)]"

echo "\n--- 4. Test ioctl GET_STATS ---"
./asgn1_test stats

echo "\n--- 5. Test ioctl RESET_BUFFER ---"
./asgn1_test reset
echo "Read back after reset: [$(cat /dev/asgn1)]"
./asgn1_test stats

echo "\n--- 6. Unload module & verify dmesg ---"
rmmod asgn1_driver
dmesg | grep asgn1_driver | tail -n 12
echo "=== TASK 1 TEST HOÀN THÀNH ==="
