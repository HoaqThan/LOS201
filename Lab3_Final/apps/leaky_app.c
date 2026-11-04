#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_RECORDS 100
#define MSG_SIZE 256

typedef struct {
    int id;
    char *name;
    int *scores;
} Record;

Record *create_record(int id, const char *name, int num_scores) {
    Record *r = malloc(sizeof(Record));

    if (r == NULL)
        return NULL;

    r->id = id;
    r->name = malloc(strlen(name) + 1);
    r->scores = malloc(sizeof(int) * num_scores);

    if (r->name == NULL || r->scores == NULL) {
        free(r->name);
        free(r->scores);
        free(r);
        return NULL;
    }

    strcpy(r->name, name);

    for (int i = 0; i < num_scores; i++)
        r->scores[i] = id * 10 + i;

    return r;
}

void delete_record(Record *r) {
    if (r == NULL)
        return;

    // BUG 1: Memory leak
    // free(r->name);
    // free(r->scores);

    free(r);
}

void process_messages(int count) {
    for (int i = 0; i < count; i++) {
        char *buf = malloc(MSG_SIZE);

        if (buf == NULL)
            return;

        snprintf(buf, MSG_SIZE,
                 "Message #%d: data payload here", i);

        if (i % 10 == 0)
            printf("Processed: %s\n", buf);

        // BUG 2: Memory leak
        // free(buf);
    }
}

int main(void) {
    printf("=== Leaky App - LAB03 ===\n");

    Record **records =
        malloc(sizeof(Record *) * NUM_RECORDS);

    if (records == NULL)
        return 1;

    for (int i = 0; i < NUM_RECORDS; i++) {
        records[i] =
            create_record(i, "EmbeddedLinuxStudent", 5);

        if (records[i] == NULL) {
            for (int j = 0; j < i; j++)
                delete_record(records[j]);

            free(records);
            return 1;
        }
    }

    for (int i = 0; i < NUM_RECORDS; i++)
        delete_record(records[i]);

    free(records);

    process_messages(50);

    printf("Program finished.\n");

    return 0;
}
