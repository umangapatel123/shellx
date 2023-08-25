#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_BACKGROUND_PROCESSES 10
#define MAX_COMMAND_LENGTH 100

bool ProcessPrintStatus=false;
char *ProcessPrintStatusString=NULL;

pid_t background_processes[MAX_BACKGROUND_PROCESSES];
char background_commands[MAX_BACKGROUND_PROCESSES][MAX_COMMAND_LENGTH];

int background_count = 0;

void handle_background_exit(int signum) {
    pid_t pid;
    int status;
    
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        for (int i = 0; i < background_count; i++) {
            if (background_processes[i] == pid) {
                ProcessPrintStatus=true;
                char *temp = (char *)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
                sprintf(temp, "%s exited normally (%d)\n", background_commands[i], pid);
                if(ProcessPrintStatusString!=NULL)
                {
                    strcat(ProcessPrintStatusString,temp);
                }
                else
                {
                    ProcessPrintStatusString=(char *)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
                    strcpy(ProcessPrintStatusString,temp);
                }
                background_processes[i] = 0;
                break;
            }
        }
    }
}

int main() {
    
    signal(SIGCHLD, handle_background_exit);
    
    char input[MAX_COMMAND_LENGTH];
    
    while (1) {
        printf("<JohnDoe@SYS:~> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';  // Remove newline
        
        if (strcmp(input, "exit") == 0) {
            break;
        } else if (strlen(input) > 0 && input[strlen(input) - 1] == '&') {
            input[strlen(input) - 1] = '\0';  // Remove trailing '&'
            
            if (background_count < MAX_BACKGROUND_PROCESSES) {
                pid_t pid = fork();
                if (pid == 0) {
                    // Child process
                    system(input);
                    exit(0);
                } else if (pid > 0) {
                    // Parent process
                    background_processes[background_count] = pid;
                    strcpy(background_commands[background_count], input);
                    background_count++;
                } else {
                    // Fork error
                    printf("Error creating background process.\n");
                }
            } else {
                printf("Maximum number of background processes reached.\n");
            }
        } else {
            pid_t pid = fork();
            if (pid == 0) {
                // Child process
                system(input);
                exit(0);
            } else if (pid > 0) {
                // Parent process
                waitpid(pid, NULL, 0);
            } else {
                // Fork error
                printf("Error creating process.\n");
            }
        }
        if(ProcessPrintStatus)
        {
            printf("%s",ProcessPrintStatusString);
            ProcessPrintStatus=false;
            ProcessPrintStatusString=NULL;
        }
    }

    return 0;
}
