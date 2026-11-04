#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Process array data
void process_data(int *data, int len) {
    int sum = 0;

    // FIX 1: Prevent out-of-bounds access
    for (int i = 0; i < len; i++) {
        sum += data[i];
    }

    printf("Sum = %d\n", sum);
}

// Create a message using dynamic memory
char *create_message(const char *text) {
    char *buf = malloc(strlen(text) + 1);

    if (buf == NULL) {
        return NULL;
    }

    strcpy(buf, text);

    // FIX 2: Do not free memory before returning
    return buf;
}

// Calculate multiplication
long long calculate(int a, int b) {
    // FIX 3: Use 64-bit arithmetic to prevent overflow
    long long result = (long long)a * b;

    printf("calculate(%d, %d) = %lld\n", a, b, result);

    return result;
}

int main(void) {
    printf("=== Buggy App - LAB03 ===\n");

    // Test array processing
    int arr[5] = {10, 20, 30, 40, 50};
    process_data(arr, 5);

    // Test dynamic memory
    char *msg = create_message("Hello Embedded Linux");

    if (msg != NULL) {
        printf("Message: %s\n", msg);

        // FIX 2: Free memory after use
        free(msg);
    }

    // Test calculation
    calculate(100000, 100000);

    return 0;
}
