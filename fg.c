#include "headers.h"

extern BgProcess bgProcessHead;
extern bool fg_running;


void fg(char **args, char *permanent_home)
{
    if (args[0] == NULL)
    {
        printf("fg Usage: fg <process_id>\n");
        return;
    }
    if (args[1] != NULL)
    {
        printf("fg Usage: fg <process_id>\n");
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
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU, SIG_IGN);
    tcsetpgrp(STDIN_FILENO, pid);
    kill(pid, SIGCONT);
    fg_running = true;
    printf("%s with pid %d resumed\n", command, pid);
    int status;
    waitpid(pid, &status, WUNTRACED);
    tcsetpgrp(STDIN_FILENO, getpgrp());
    signal(SIGTTOU, SIG_DFL);
    signal(SIGTTIN, SIG_DFL);
    if (WIFSTOPPED(status))
    {
        addToBgProcessList(&bgProcessHead, pid, command);
        printf("\n%s with pid %d suspended\n", command, pid);
    }
    fg_running = false;
    // remove from bgProcessList
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
            break;
        }
        prev = current;
        current = current->next;
    }
    return;
}