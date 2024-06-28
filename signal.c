#include "headers.h"
extern bool fg_running;
extern char *Home_Permenant;
extern BgProcess bgProcessHead;
extern char *GlobalInput;

bool prev_fg_ctrl_c = false;
bool prev_fg_ctrl_z = false;


void key_handler(int sig)
{
    pid_t pid = getpid();

    if(sig==SIGINT)
    {
        if(fg_running)
        {
            prev_fg_ctrl_c = true;
            kill(pid, SIGINT);
            fg_running = false;
        }
        else
        {
            if(prev_fg_ctrl_c)
            {
                prev_fg_ctrl_c = false;
                printf("\n");
                fflush(stdout);
            }
            else
            {
                printf("\n");
                prompt(Home_Permenant);
                fflush(stdout);
            }
        }
    }
    else if (sig==SIGTSTP)
    {
        if(fg_running)
        {
            prev_fg_ctrl_z = true;
            kill(pid, SIGTSTP);
            fg_running = false;
        }
        else
        {
            if(prev_fg_ctrl_z)
            {
                prev_fg_ctrl_z = false;
                addToBgProcessList(&bgProcessHead, pid, GlobalInput);
                printf("\n");
                fflush(stdout);
            }
            else
            {
                printf("\n");
                prompt(Home_Permenant);
                fflush(stdout);
            }
        }
    }
    else
    {
        printf("Error in handling signal\n");
    }
}

void logout()
{
    BgProcess temp = bgProcessHead;
    if(temp == NULL)
    {
        exit(EXIT_SUCCESS);
    }
    while(temp!=NULL)
    {
        kill(temp->pid, SIGKILL);
        temp = temp->next;
    }
    exit(EXIT_SUCCESS);
}

void ping(char **args, char *permanent_home)
{
    if(args[0] == NULL)
    {
        printf("ping: Usage: ping <pid> <signal>\n");
        return;
    }
    if(args[1] == NULL)
    {
        printf("ping: Usage: ping <pid> <signal>\n");
        return;
    }
    if(args[2] != NULL)
    {
        printf("ping: Usage: ping <pid> <signal>\n");
        return;
    }
    pid_t pid = atoi(args[0]);
    int signal = atoi(args[1]);
    signal= signal % 32;
    if(kill(pid,0) == -1)
    {
        perror("No such process exists");
        return;
    }
    if(kill(pid, signal) == -1)
    {
        printf("ping: Error in sending signal\n");
        return;
    }

}