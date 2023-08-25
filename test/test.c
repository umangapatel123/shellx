#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LINE_LENGTH 256

void print_process_info(pid_t pid) {
    char proc_path[100];
    snprintf(proc_path, sizeof(proc_path), "/proc/%d/status", pid);

    FILE* file = fopen(proc_path, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char line[MAX_LINE_LENGTH];
    int found_pid = 0;
    while (fgets(line, sizeof(line), file)) {
        int current_pid;
        char status[MAX_LINE_LENGTH];
        int group;
        unsigned long virtual_memory;
        char executable_path[MAX_LINE_LENGTH];

        if (sscanf(line, "Pid: %d", &current_pid) == 1) {
            if (current_pid == pid || pid == -1) {
                found_pid = 1;
                printf("Pid: %d\n", current_pid);
            }
        } else if (found_pid && sscanf(line, "State: %s", status) == 1) {
            printf("Process Status: %s\n", status);
        } else if (found_pid && sscanf(line, "PPid: %*d\nPgrp: %d", &group) == 1) {
            printf("Process Group: %d\n", group);
        } else if (found_pid && sscanf(line, "VmSize: %lu", &virtual_memory) == 1) {
            printf("Virtual Memory: %lu KB\n", virtual_memory);
        } else if (found_pid && sscanf(line, "Name: %s", executable_path) == 1) {
            printf("Executable Path: %s\n", executable_path);
        }

        if (found_pid && pid != -1) {
            break;  // If a specific PID was provided, only print its info
        }
    }

    fclose(file);
}

int main(int argc, char* argv[]) {
    pid_t target_pid = -1;  // Default value, indicating shell process

    if (argc == 2) {
        target_pid = atoi(argv[1]);
    }

    print_process_info(target_pid);

    return 0;
}
