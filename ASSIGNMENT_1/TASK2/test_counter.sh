#!/bin/sh

DEVICE=/dev/counter
PROC=/proc/counter_info
PASS=0
FAIL=0

check_equal() {
    name="$1"
    actual="$2"
    expected="$3"

    if [ "$actual" = "$expected" ]; then
        echo "[PASS] $name: $actual"
        PASS=$((PASS + 1))
    else
        echo "[FAIL] $name: expected=$expected actual=$actual"
        FAIL=$((FAIL + 1))
    fi
}

echo "========================================"
echo " TASK 2 - COUNTER DRIVER TEST"
echo "========================================"

if [ ! -c "$DEVICE" ] || [ ! -f "$PROC" ]; then
    echo "[FAIL] Device or procfs missing"
    exit 1
fi

echo "========== RESET =========="
echo 0 > "$DEVICE" || exit 1

echo "========== READ TEST =========="
check_equal "Read 0" "$(cat "$DEVICE")" "0"
check_equal "Read 1" "$(cat "$DEVICE")" "1"
check_equal "Read 2" "$(cat "$DEVICE")" "2"

echo "========== WRITE TEST =========="
echo 100 > "$DEVICE" || exit 1
check_equal "Read 100" "$(cat "$DEVICE")" "100"
check_equal "Read 101" "$(cat "$DEVICE")" "101"

echo "========== RESET TEST =========="
echo 0 > "$DEVICE" || exit 1
check_equal "Reset to 0" "$(cat "$DEVICE")" "0"

echo "========== INVALID INPUT =========="
if echo -5 > "$DEVICE" 2>/dev/null; then
    echo "[FAIL] Negative value accepted"
    FAIL=$((FAIL + 1))
else
    echo "[PASS] Negative value rejected"
    PASS=$((PASS + 1))
fi

if echo abc > "$DEVICE" 2>/dev/null; then
    echo "[FAIL] Invalid string accepted"
    FAIL=$((FAIL + 1))
else
    echo "[PASS] Invalid string rejected"
    PASS=$((PASS + 1))
fi

echo "========== PROCFS =========="
cat "$PROC"

echo "========== SUMMARY =========="
echo "PASS=$PASS"
echo "FAIL=$FAIL"

[ "$FAIL" -eq 0 ]
