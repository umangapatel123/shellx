#ifndef __PIPE_H__
#define __PIPE_H__

bool isPiped(char *command);
int countPipes(char **args);
char **TokenizePipe(char *command);
void pipeHandler(char *command, char *permenant_home);

#endif