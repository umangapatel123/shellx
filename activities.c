#include "headers.h"

extern BgProcess bgProcessHead;

struct Activity
{
    char *ActivityName;
    struct Activity *next;
};

typedef struct Activity *Activity;


void addToActivityList(Activity *head, char *ActivityName)
{
    Activity temp=(Activity)malloc(sizeof(struct Activity));
    temp->ActivityName=(char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
    strcpy(temp->ActivityName,ActivityName);
    temp->next=NULL;
    if(*head==NULL)
    {
        *head=temp;
        return;
    }
    Activity temp1=*head;
    while(temp1->next!=NULL)
    {
        temp1=temp1->next;
    }
    temp1->next=temp;
}

void printActivityList(Activity head)
{
    Activity temp=head;
    while(temp!=NULL)
    {
        printf("%s\n",temp->ActivityName);
        temp=temp->next;
    }
}

int comparator(const void *a, const void *b)
{
    Activity a1=*(Activity *)a;
    Activity b1=*(Activity *)b;
    return strcmp(a1->ActivityName,b1->ActivityName);
}

int sizeOfList(Activity head)
{
    int count=0;
    Activity temp=head;
    while(temp!=NULL)
    {
        count++;
        temp=temp->next;
    }
    return count;
}

void activities(char **args, char* permenant_home)
{
    BgProcess temp=bgProcessHead;
    char *path=(char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
    Activity head=NULL;
    while(temp!=NULL)
    {
        sprintf(path,"/proc/%d/stat",temp->pid);
        FILE *file=fopen(path,"r");
        if(file==NULL)
        {
            printf("Unable to open file for the process %d\n",temp->pid);
            temp=temp->next;
            continue;
        }
        char *line=(char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
        fgets(line,MAX_COMMAND_LENGTH,file);
        char *token=strtok(line," \t");
        char *statusCommand=(char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
        for(int i=0;i<2;i++)
        {
            token=strtok(NULL," \t");
        }
        // printf("[%d] ",temp->jobNumber);
        if(strcmp(token,"T")==0)
        {
            statusCommand="Stopped ";
        }
        else
        {
            statusCommand="Running ";
        }
        char *tempToStore=(char *)malloc(sizeof(char)*MAX_COMMAND_LENGTH);
        sprintf(tempToStore,"%d : %s - %s",temp->pid,temp->command,statusCommand);
        // printf("%s\n",tempToStore);
        addToActivityList(&head,tempToStore);
        temp=temp->next;
    }

    Activity *array=(Activity *)malloc(sizeof(Activity)*sizeOfList(head));
    Activity temp1=head;
    int i=0;
    while(temp1!=NULL)
    {
        array[i]=temp1;
        temp1=temp1->next;
        i++;
    }
    qsort(array,sizeOfList(head),sizeof(Activity),comparator);
    for(int i=0;i<sizeOfList(head);i++)
    {
        printf("%s\n",array[i]->ActivityName);
    }
    return;
}