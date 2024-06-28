#include "headers.h"

extern BgProcess bgProcessHead;

void bg(char **args,char *permanent_home)
{
    if (args[0] == NULL)
    {
        printf("bg Usage: bg <process_id>\n");
        return;
    }
    if (args[1] != NULL)
    {
        printf("bg Usage: bg <process_id>\n");
        return;
    }
    int tmppid = atoi(args[0]);
    BgProcess temp = bgProcessHead;
    while (temp != NULL)
    {
        if (temp->pid == tmppid)
        {
            break;
        }
        temp = temp->next;
    }
    if (temp == NULL)
    {
        printf("Error: Invalid Process ID\n");
        return;
    }
    pid_t pid = temp->pid;
    char *command = temp->command;
    kill(pid, SIGCONT);
    printf("%s with pid %d resumed\n", command, pid);
    return;
}