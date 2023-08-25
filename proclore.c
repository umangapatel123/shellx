#include "headers.h"

// get the process info from the pid

int is_number(char *str)
{
    int i = 0;
    while (str[i] != '\0')
    {
        if (isdigit(str[i]) == 0)
        {
            return 0;
        }
        i++;
    }
    return 1;
}

void print_info(int pid, char *permenant_home)
{

    char *path = malloc(MAX_PATH_LEN * sizeof(char));
    sprintf(path, "/proc/%d/stat", pid);
    pid_t gpid = getpgid(pid);
    char *proc_status = malloc(MAX_PATH_LEN * sizeof(char));
    sprintf(proc_status, "/proc/%d/status", pid);
    char *line = NULL;
    size_t len = 0;
    FILE *fp = fopen(proc_status, "r");
    if (fp == NULL)
    {
        perror("Error");
        return;
    }
    printf("pid : %d\n", pid);
    while (getline(&line, &len, fp) != -1)
    {
        char *token = strtok(line, " \t");
        // printf("%s\n",token);
        if (strcmp(token, "State:") == 0)
        {
            token = strtok(NULL, " \t");
            pid_t foreground = tcgetpgrp(STDIN_FILENO);
            if (foreground == gpid)
            {
                strcat(token, "+");
            }
            else
            {
                strcat(token, " ");
            }
            printf("process status : %s\n", token);
        }
        else if (strcmp(token, "PPid:") == 0)
        {
            token = strtok(NULL, " \t");
            printf("Process Group : %d\n", gpid);
        }
        else if (strcmp(token, "VmSize:") == 0)
        {
            token = strtok(NULL, " \t");
            printf("Virtual memory : %s\n", token);
        }
    }

    char *temp_exe_path = malloc(MAX_PATH_LEN * sizeof(char));
    sprintf(temp_exe_path, "/proc/%d/exe", pid);
    char *exe_path = malloc(MAX_PATH_LEN * sizeof(char));
    int ret = readlink(temp_exe_path, exe_path, MAX_PATH_LEN);
    if (ret == -1)
    {
        perror("Error");
        return;
    }
    char *prompt = malloc(MAX_PATH_LEN * sizeof(char));
    exe_path[ret] = '\0';
    if (strcmp(exe_path, permenant_home) == 0)
    {
        strcpy(prompt, "~");
    }
    else if (strstr(exe_path, permenant_home) != NULL)
    {
        strcpy(prompt, "~");
        exe_path = string_cut(exe_path, strlen(permenant_home), strlen(exe_path) - strlen(permenant_home));
        strcat(prompt, exe_path);
    }
    else
    {
        strcpy(prompt, exe_path);
    }
    printf("executable path : %s\n", prompt);
}

void proclore(char **args, char *permenant_home)
{
    int pid;
    if (args[0] == NULL)
    {
        // get the pid of the current process
        pid = getpid();
    }
    else
    {
        // check if args[0] is a number

        if (is_number(args[0]) == 0)
        {
            printf("Invalid pid\n");
            return;
        }
        if (args[1] != NULL)
        {
            printf("proclore: too many arguments\n");
            return;
        }

        pid = atoi(args[0]);
    }
    print_info(pid, permenant_home);
    return;
}
