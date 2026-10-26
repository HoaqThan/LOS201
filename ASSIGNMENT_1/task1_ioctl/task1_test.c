#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "asgn1_ioctl.h"

int main(int argc, char *argv[]) {
    int fd = open("/dev/asgn1", O_RDWR);
    if (fd < 0) { perror("open"); return 1; }

    if (argc < 2) { printf("Usage: %s <reset|stats|mode <0|1>|version>\n", argv[0]); return 1; }

    if (strcmp(argv[1], "reset") == 0) {
        if (ioctl(fd, ASGN1_RESET_BUFFER) < 0) perror("ioctl reset");
        else printf("[OK] ASGN1_RESET_BUFFER executed\n");
    } else if (strcmp(argv[1], "stats") == 0) {
        struct asgn1_stats stats;
        if (ioctl(fd, ASGN1_GET_STATS, &stats) < 0) perror("ioctl stats");
        else {
            printf("[STATS] open_count: %d\n", stats.open_count);
            printf("[STATS] write_count: %d\n", stats.write_count);
            printf("[STATS] read_count: %d\n", stats.read_count);
            printf("[STATS] buffer_len: %d\n", stats.buffer_len);
            printf("[STATS] last_write_size: %d\n", stats.last_write_size);
        }
    } else if (strcmp(argv[1], "mode") == 0 && argc >= 3) {
        int mode = atoi(argv[2]);
        if (ioctl(fd, ASGN1_SET_MODE, &mode) < 0) perror("ioctl mode");
        else printf("[OK] ASGN1_SET_MODE set to %d\n", mode);
    } else if (strcmp(argv[1], "version") == 0) {
        char ver[64];
        if (ioctl(fd, ASGN1_GET_VERSION, ver) < 0) perror("ioctl version");
        else printf("[VERSION] Driver version: %s\n", ver);
    }
    close(fd);
    return 0;
}

