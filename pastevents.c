#include "headers.h"


History history;

void init_history()
{
    history = (History)malloc(sizeof(struct history));
    history->head = NULL;
    history->tail = NULL;
    history->size = 0;
}


char *getCommand(History history, int index)
{
    if(index<=0 || index>history->size)
    {
        return NULL;
    }
    int temp_index=history->size-index+1;
    Command command = history->head;
    while(temp_index>1)
    {
        command = command->next;
        temp_index--;
    }
    return command->command_name;
}

void addHistory(History history, const char* command_name)
{
    if(history->size>0 && strcmp(history->tail->command_name,command_name)==0)
    {
        return;
    }
    Command command = (Command)malloc(sizeof(struct command));
    if(command==NULL)
    {
        perror("Error");
        return;
    }
    char *temp = (char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
    strcpy(temp,command_name);
    temp = strtok(temp,"\n");
    strcpy(command->command_name,temp);
    command->next = NULL;

    if(history->size == MAX_HISTORY_LENGTH)
    {
        Command temp = history->head;
        history->head = history->head->next;
        free(temp);
        history->size--;
    }

    if(history->size == 0)
    {
        history->head = command;
        history->tail = command;
    }
    else
    {
        history->tail->next = command;
        history->tail = command;
    }
    history->size++;
}

void printHistory(History history)
{
    Command command = history->head;
    int i = 1;
    while(command!=NULL)
    {
        printf("%d. %s\n",i,command->command_name);
        command = command->next;
        i++;
    }
}

void freeHistory(History history)
{
    Command command = history->head;
    while(command!=NULL)
    {
        Command temp = command;
        command = command->next;
        free(temp);
    }
    free(history);
}

void fileToLL(History history, char *permenant_home)
{
    char *temp_path = (char *)malloc(sizeof(char)*MAX_PATH_LEN);
    strcpy(temp_path,permenant_home);
    strcat(temp_path,"/.history");
    FILE *fp = fopen(temp_path,"r");
    if(fp==NULL)
    {
        fp=fopen(temp_path,"wb");
        if(fp==NULL)
        {
            perror("Error");
            return;
        }
        fclose(fp);
        return;
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while((read = getline(&line,&len,fp))!=-1)
    {
        char *temp = (char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
        strcpy(temp,line);
        temp = strtok(temp,"\n");
        if(temp==NULL)
        {
            continue;
        }
        // printf("%s\n",temp);
        // printf("%ld\n",strlen(temp));
        addHistory(history,temp);
    }
    fclose(fp);
    if(line)
    {
        free(line);
    }
}

void LLToFile(History history, char *permenant_home)
{
    char *temp_path = (char *)malloc(sizeof(char)*MAX_PATH_LEN);
    strcpy(temp_path,permenant_home);
    strcat(temp_path,"/.history");
    FILE *fp = fopen(temp_path,"w");
    if(fp==NULL)
    {
        fp=fopen(temp_path,"wb");
        if(fp==NULL)
        {
            perror("Error");
            return;
        }
        fclose(fp);
        return;
    }
    Command command = history->head;
    while(command!=NULL)
    {
        if(command->command_name==NULL)
        {
            command = command->next;
            continue;
        }
        fprintf(fp,"%s\n",command->command_name);
        command = command->next;
    }
    fclose(fp);
}

void mainToAddHistory(char *command_name, char *permenant_home)
{
    init_history();
    fileToLL(history,permenant_home);
    // printf("here1\n");
    addHistory(history,command_name);
    // printf("here2\n");
    LLToFile(history,permenant_home);
    // printf("here3\n");
    freeHistory(history);
}

void pastevents(char **args, char *permenant_home)
{
    if(args[0]==NULL)
    {
        init_history();
        fileToLL(history,permenant_home);
        if(history->size==0)
        {
            printf("pastevents: No past events\n");
            freeHistory(history);
            return;
        }
        printHistory(history);
        freeHistory(history);
        return;
    }
    if(strcmp(args[0],"purge")==0)
    {
        init_history();
        fileToLL(history,permenant_home);
        freeHistory(history);
        init_history();
        LLToFile(history,permenant_home);
        printf("pastevents: All past events cleared\n");
        return;
    }
    if(strcmp(args[0],"execute")==0)
    {
        if(args[1]==NULL)
        {
            printf("Error: No index number given for execute\n");
            return;
        }
        int index = atoi(args[1]);
        init_history();
        fileToLL(history,permenant_home);

        if(index<=0 || index>history->size)
        {
            printf("Error: Invalid index number given for execute\n");
            return;
        }

        char *command_name = getCommand(history,index);
        if(command_name==NULL)
        {
            printf("Error: Invalid index number given for execute\n");
            return;
        }
        execute_command(command_name,permenant_home);
        mainToAddHistory(command_name,permenant_home);
        return;
    }
}






