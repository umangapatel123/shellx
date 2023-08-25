#include "headers.h"
char *previous_directory = NULL;
bool temp_flag = false;

bool ProcessPrintStatus = false;
char *ProcessPrintStatusString = NULL;

extern BgProcess bgProcessHead;

void handle_background_exit()
{
    pid_t pid;
    int status;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        BgProcess current = bgProcessHead;
        BgProcess prev = NULL;
        while (current != NULL)
        {
            if (current->pid == pid)
            {
                if (prev == NULL)
                {
                    bgProcessHead = current->next;
                }
                else
                {
                    prev->next = current->next;
                }
                ProcessPrintStatus = true;
                char *temp = (char *)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
                if (WIFEXITED(status))
                    sprintf(temp, "%s exited normally (%d)\n", current->command, pid);
                else
                    sprintf(temp, "%s exited abnormally (%d)\n", current->command, pid);

                if (ProcessPrintStatusString != NULL)
                {
                    strcat(ProcessPrintStatusString, temp);
                }
                else
                {
                    ProcessPrintStatusString = (char *)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
                    strcpy(ProcessPrintStatusString, temp);
                }
                break;
            }
            prev = current;
            current = current->next;
        }
    }
}

void setSignal()
{
    // signal(SIGINT, SIG_IGN);
    // signal(SIGTSTP, SIG_IGN);
    signal(SIGCHLD, handle_background_exit);
}

void execute_command(char *input, char *permenant_home)
{
    setSignal();
    char dup[4096];
    strcpy(dup, input);
    char dup2[4096];
    strcpy(dup2, input);
    char *token1 = strtok(dup2, ";");
    char **args = malloc(MAX_PATH_LEN * sizeof(char *));
    int i = 0;
    while (token1 != NULL)
    {
        args[i] = token1;
        i++;
        token1 = strtok(NULL, ";");
    }
    bool flag = true;
    bool not_include = false;
    int ct = 0;
    for (int j = 0; j < i; j++)
    {
        temp_flag = false;
        char *token2 = strtok(args[j], " \t\n");
        if (token2 == NULL)
        {
            continue;
        }
        ct++;
        if (strcmp(token2, "exit") == 0)
        {
            if (strstr(dup, "pastevents") == NULL)
            {
                mainToAddHistory(dup, permenant_home);
            }
            exit(0);
        }
        if (strcmp(token2, "warp") == 0)
        {
            int ct = 0;
            while (token2 != NULL)
            {
                token2 = strtok(NULL, " \t\n");
                if (token2 == NULL)
                {
                    if (ct == 0)
                    {
                        warp(NULL, permenant_home, &previous_directory, dup);
                    }
                    break;
                }
                warp(token2, permenant_home, &previous_directory, dup);
                ct++;
            }
            flag = flag && temp_flag;
            continue;
        }
        if (strcmp(token2, "peek") == 0)
        {
            char **args = malloc(MAX_PATH_LEN * sizeof(char *));
            int i = 0;
            while (token2 != NULL)
            {
                token2 = strtok(NULL, " \t\n");
                if (token2 == NULL)
                {
                    break;
                }
                args[i] = token2;
                i++;
            }
            args[i] = NULL;
            peek(args, permenant_home, dup);
            flag = flag && temp_flag;
            continue;
        }
        if (strcmp(token2, "pastevents") == 0)
        {
            char **args = malloc(MAX_PATH_LEN * sizeof(char *));
            int i = 0;
            while (token2 != NULL)
            {
                token2 = strtok(NULL, " \t\n");
                if (token2 == NULL)
                {
                    break;
                }
                args[i] = token2;
                i++;
            }
            args[i] = NULL;
            pastevents(args, permenant_home);
            flag = flag && temp_flag;
            continue;
        }
        if (strcmp(token2, "proclore") == 0)
        {
            char **args = malloc(MAX_PATH_LEN * sizeof(char *));
            int i = 0;
            while (token2 != NULL)
            {
                token2 = strtok(NULL, " \t\n");
                if (token2 == NULL)
                {
                    break;
                }
                args[i] = token2;
                i++;
            }
            args[i] = NULL;
            proclore(args, permenant_home);
            flag = flag && temp_flag;
            continue;
        }
        if (strcmp(token2, "seek") == 0)
        {
            char **args = malloc(MAX_PATH_LEN * sizeof(char *));
            int i = 0;
            while (token2 != NULL)
            {
                token2 = strtok(NULL, " \t\n");
                if (token2 == NULL)
                {
                    break;
                }
                args[i] = token2;
                i++;
            }
            args[i] = NULL;
            seek(args, permenant_home, i);
            flag = flag && temp_flag;
            continue;
        }
        else
        {
            bool bg = false;
            if (strstr(token2, "&") == 0)
            {
                bg = true;
            }

            bool flag_for_echo = false;
            if (strstr(token2, "echo") != NULL)
            {
                flag_for_echo = true;
            }

            char *commands_first = (char *)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
            strcpy(commands_first, token2);

            if (!flag_for_echo)
            {
                while (token2 != NULL)
                {
                    token2 = strtok(NULL, " \t\n");
                    if (token2 == NULL)
                    {
                        break;
                    }
                    strcat(commands_first, " ");
                    strcat(commands_first, token2);
                }
                commands_first[strlen(commands_first)] = '\0';
            }
            else
            {
                strcpy(commands_first, dup);
            }

            system_command(commands_first, permenant_home);
            not_include = true;
            continue;
        }
    }

    // if (!not_include)
    // {
    if (strstr(dup, "pastevents") == NULL && ct != 0)
    {
        mainToAddHistory(dup, permenant_home);
    }
    // }
    flag = true;
    free(args);
}

int main()
{
    char *permenant_home = malloc(MAX_PATH_LEN * sizeof(char));
    getcwd(permenant_home, MAX_PATH_LEN);
    // Keep accepting commands
    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(permenant_home);
        char input[4096];
        fgets(input, 4096, stdin);
        if (ProcessPrintStatus)
        {
            printf("%s", ProcessPrintStatusString);
            ProcessPrintStatus = false;
            ProcessPrintStatusString = NULL;
        }
        execute_command(input, permenant_home);
    }
}
