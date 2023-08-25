#ifndef SYSTEM_H
#define SYSTEM_H

void system_command(char* command,char *permenant_home);
void bgProcessStatus();

// char *promptInclude=NULL;
// bool promptIncludeFlag=false;
struct bgProcess
{
    pid_t pid;
    char *command;
    time_t start;
    struct bgProcess *next;
};

typedef struct bgProcess *BgProcess;

#endif