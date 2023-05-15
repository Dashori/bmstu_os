#include "print.h"

int main(int argc, char *argv[]) {
    char *pid = argv[1];
    FILE *file = fopen("output.txt", "w");
    if (!file) {
        perror("fopen() error");
        exit(1);
    }
    printf("PID: %s\n", pid);
    // get_pagemap_info(pid, file);
    get_maps_info(pid, file);
    get_stat_info(pid, file);
    get_task_info(pid, file);
    return 0;
}
