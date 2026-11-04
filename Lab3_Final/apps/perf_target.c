#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define ARRAY_SIZE 100000
#define COMPUTE_ITERATIONS 50
#define SEARCH_ITERATIONS 1000
#define FILE_LINES 10000

volatile double result_sink = 0.0;
volatile int search_sink = 0;

void heavy_compute(double *arr, int size) {
    double result = 0.0;

    for (int j = 0; j < COMPUTE_ITERATIONS; j++) {
        for (int i = 0; i < size; i++) {
            result += sqrt(arr[i]) * log(arr[i] + 1.0);
        }
    }

    result_sink = result;
}

void linear_search(int *arr, int size) {
    int found = 0;

    for (int j = 0; j < SEARCH_ITERATIONS; j++) {
        int target = j % size;

        for (int i = 0; i < size; i++) {
            if (arr[i] == target) {
                found++;
                break;
            }
        }
    }

    search_sink = found;
}

void file_io_test(void) {
    FILE *fp = fopen("/tmp/perf_test.txt", "w");

    if (fp == NULL) {
        perror("fopen");
        return;
    }

    for (int i = 0; i < FILE_LINES; i++) {
        fprintf(fp, "Performance test line %d\n", i);
    }

    fclose(fp);
}

int main(void) {
    double *data = malloc(ARRAY_SIZE * sizeof(double));
    int *search_data = malloc(ARRAY_SIZE * sizeof(int));

    if (data == NULL || search_data == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(data);
        free(search_data);
        return 1;
    }

    for (int i = 0; i < ARRAY_SIZE; i++) {
        data[i] = (double)(i + 1);
        search_data[i] = i;
    }

    printf("=== LAB03 Performance Target ===\n");

    clock_t start = clock();

    heavy_compute(data, ARRAY_SIZE);
    linear_search(search_data, ARRAY_SIZE);
    file_io_test();

    clock_t end = clock();

    printf("Compute result: %.2f\n", result_sink);
    printf("Search result: %d\n", search_sink);
    printf("CPU time: %.4f seconds\n",
           (double)(end - start) / CLOCKS_PER_SEC);

    free(data);
    free(search_data);

    return 0;
}
