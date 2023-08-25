#ifndef PASTEVENTS_H
#define PASTEVENTS_H

#define MAX_COMMAND_LENGTH 1024
#define MAX_HISTORY_LENGTH 15

struct command
{
    char command_name[MAX_COMMAND_LENGTH];
    struct command *next;
};

typedef struct command* Command;

struct history
{
    Command head;
    Command tail;
    int size;
};

typedef struct history* History;

void pastevents(char **args, char *permenant_home);
void mainToAddHistory(char *command_name, char *permenant_home);

#endif