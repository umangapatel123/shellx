#include "headers.h"
#define _OPEN_SYS_ITOA_EXT


// Path: system.c



char *PromptInclude=NULL;
bool PromptIncludeFlag=false;




BgProcess bgProcessHead = NULL;

void background_run(char *command,char **args)
{
    int pid = fork();
    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        setpgid(0,0);
        if (execvp(args[0], args) == -1)
        {
            printf("\nError: '%s' is not a valid command\n", args[0]);
        }
        exit(EXIT_FAILURE);
    }
    else if (pid < 0)
    {   
        perror("Error");
    }
    else
    {
        BgProcess bgProcess = (BgProcess)malloc(sizeof(struct bgProcess));
        bgProcess->pid = pid;
        bgProcess->command = (char *)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
        strcpy(bgProcess->command, command);
        bgProcess->start = time(NULL);
        bgProcess->next = bgProcessHead;
        bgProcessHead = bgProcess;
        printf("%d\n", pid);
    }
    return;
}

void addToBgProcessList(BgProcess *head, pid_t pid, char *command)
{
    BgProcess temp = (BgProcess)malloc(sizeof(struct bgProcess));
    temp->pid = pid;
    temp->command = (char *)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
    strcpy(temp->command, command);
    temp->start = time(NULL);
    temp->next = NULL;
    if (*head == NULL)
    {
        *head = temp;
        return;
    }
    temp->next = *head;
    *head = temp;
    return;
}

// void bgProcessStatus()
// {
//     BgProcess current=bgProcessHead;
//     BgProcess prev=NULL;
    
//     pid_t tempPid;
//     int status;

//     while ((tempPid = waitpid(-1, &status, WNOHANG)) > 0) {
//         while (current != NULL)
//         {
//             if (current->pid == tempPid)
//             {
//                 char *temp = (char *)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
//                 sprintf(temp, "%s exited normally (%d)\n", current->command, tempPid);
//                 ProcessPrintStatus=true;
//                 if(ProcessPrintStatusString!=NULL)
//                 {
//                     strcat(ProcessPrintStatusString,temp);
//                 }
//                 else
//                 {
//                     ProcessPrintStatusString=(char *)malloc(sizeof(char) * MAX_COMMAND_LENGTH);
//                     strcpy(ProcessPrintStatusString,temp);
//                 }
//                 if (prev == NULL)
//                 {
//                     bgProcessHead = current->next;
//                 }
//                 else
//                 {
//                     prev->next = current->next;
//                 }
//                 free(current);
//                 break;
//             }
//             prev = current;
//             current = current->next;
//         }
//         current = bgProcessHead;
//         prev = NULL;
//     }
// }

int backgroundProcessCount(char* command)
{
    int len=strlen(command);
    int count=0;
    for(int i=0;i<len;i++)
    {
        if(command[i]=='&')
        {
            count++;
        }
    }
    return count;
}

void call_backgrounds(char **backgroundProcess)
{
    int i=0;
    while(backgroundProcess[i]!=NULL)
    {
        char **args = malloc(MAX_PATH_LEN * sizeof(char *));
        int j = 0;
        char *token2=strtok(backgroundProcess[i]," \t\n");
        while (token2 != NULL)
        {
            args[j] = token2;
            j++;
            token2 = strtok(NULL, " \t\n");
        }
        args[j] = NULL;
        background_run(backgroundProcess[i],args);
        i++;
    }
}

void foreground(char **args)
{
    int pid = fork();
    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);
        signal(SIGTSTP, SIG_DFL);
        setpgid(0,0);
        if (execvp(args[0], args) == -1)
        {
            printf("\nError: '%s' is not a valid command\n", args[0]);
        }
        exit(EXIT_FAILURE);

    }
    else if (pid < 0)
    {
        perror("Error");
    }
    else
    {
        int status;
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        tcsetpgrp(STDIN_FILENO, pid);
        time_t start = time(NULL);
        waitpid(pid, &status, WUNTRACED);
        time_t end = time(NULL);
        tcsetpgrp(STDIN_FILENO, getpgrp());
        signal(SIGTTOU, SIG_DFL);
        signal(SIGTTIN, SIG_DFL);
        if(end - start >= 2)
        {
            char *temp=(char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
            char buffer [sizeof(int64_t)*8+1];
            strcpy(temp,args[0]);
            strcat(temp," : ");
            sprintf(buffer, "%ld", end - start);
            strcat(temp,buffer);
            strcat(temp,"s");
            strcat(temp, "\0");
            PromptIncludeFlag=true;
            if(PromptInclude!=NULL)
            {
                strcat(PromptInclude," ");
                strcat(PromptInclude,temp);
            }
            else
            {
                PromptInclude=(char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
                strcpy(PromptInclude,temp);
            }
        }
        if (WIFSTOPPED(status))
        {
            addToBgProcessList(&bgProcessHead, pid, args[0]);
            printf("\n%s with pid %d suspended\n", args[0], pid);
        }
    }
}

void call_foreground(char **foregroundProcess,char *permenant_home)
{
    int i=0;
    while(foregroundProcess[i]!=NULL)
    {
        char *duplication=(char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
        strcpy(duplication,foregroundProcess[i]);
        char **args = malloc(MAX_PATH_LEN * sizeof(char *));
        int j = 0;
        char *token2=strtok(duplication," \t\n");
        bool flag_for_echo=false;
        if(strstr(token2,"echo")!=NULL)
        {
            flag_for_echo=true;
        }
        while (token2 != NULL)
        {
            args[j] = token2;
            j++;
            if(flag_for_echo)
            {
                token2 = strtok(NULL, "\n");
            }
            else
                token2 = strtok(NULL, " \t\n");
        }
        args[j] = NULL;
        if(strstr(foregroundProcess[i],"exit")!=NULL ||
        strstr(foregroundProcess[i],"warp")!=NULL ||
        strstr(foregroundProcess[i],"peek")!=NULL ||
        strstr(foregroundProcess[i],"pastevents")!=NULL ||
        strstr(foregroundProcess[i],"proclore")!=NULL ||
        strstr(foregroundProcess[i],"seek")!=NULL)
        {
            execute_command (foregroundProcess[i],permenant_home);
        }
        else
        {
            foreground(args);
        }
        i++;
    }
}


void system_command(char* command,char *permenant_home)
{
    // run system command via execvp
    char *dup_commmand_first=(char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
    strcpy(dup_commmand_first,command);
    char *duplicate_command=(char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
    strcpy(duplicate_command,command);
    char *duplicate=(char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
    strcpy(duplicate,command);
    char *token2= strtok(command, " \t\n");
    char *token = strtok(duplicate_command, "&");
    char **args = malloc(MAX_PATH_LEN * sizeof(char *));
    bool bg = false;
    int i = 0;
    char **backgroundProcess=(char **)malloc(sizeof(char *)*MAX_PATH_LEN);
    if(strstr(duplicate,"&")!=NULL)
    {
        bg=true;
    }
    // printf("%d\n",bg);
    if(bg)
    {
        backgroundProcess[0]=strtok(duplicate,"&");
        // printf("%s\n",backgroundProcess[0]);
        int count=backgroundProcessCount(dup_commmand_first);
        // printf("%d\n",count);
        for(int i=1;i<count;i++)
        {
            backgroundProcess[i]=strtok(NULL,"&");
        }
        backgroundProcess[count]=NULL;
    }
    else
    {
        backgroundProcess[0]=NULL;
    }

    while (token != NULL)
    {
        if(!bg)
        {
            args[i] = token;
            i++;
        }
        token = strtok(NULL, "&");
        if (token == NULL)
        {
            break;
        }
        args[i] = token;
        i++;
    }
    args[i] = NULL;
    i=0;
    // while(backgroundProcess[i]!=NULL)
    // {
    //     printf("%s\n",backgroundProcess[i]);
    //     i++;

    // }
    if(bg)
    {
        call_backgrounds(backgroundProcess);
    }
    
    if(args[0]!=NULL)
    {
        call_foreground(args,permenant_home);
    }
    


}