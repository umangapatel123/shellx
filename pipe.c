#include "headers.h"

bool isPiped(char *command)
{
    return (strstr(command, "|") != NULL);
}

int countPipes(char **args)
{
    int count = 0;
    while (args[count] != NULL)
    {
        count++;
    }
    return count;
}


char **TokenizePipe(char *command)
{
    char **args = malloc(MAX_PATH_LEN * sizeof(char *));
    char *token = strtok(command, "|");
    int i = 0;
    while (token != NULL)
    {
        args[i] = token;
        i++;
        token = strtok(NULL, "|");
    }
    args[i] = NULL;
    return args;
}



void pipeHandler(char *command, char *permenant_home)
{
    int pipefd[2];
    int fd_in = STDIN_FILENO;
    int fd_out = STDOUT_FILENO;
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stdin = dup(STDIN_FILENO);
    char **args = TokenizePipe(command);
    int count = countPipes(args);
    // printf("%d\n", count);
    for (int i = 0; i < count; i++)
    {
        if(pipe(pipefd) == -1)
        {
            perror("pipe");
            return;
        }
        pid_t pid = fork();
        if (pid == -1)
        {
            perror("fork");
            return;
        }
        else if (pid == 0)
        {
            dup2(fd_in, STDIN_FILENO);
            if (i != count - 1)
            {
                dup2(pipefd[1], STDOUT_FILENO);
            }
            close(pipefd[0]);
            char *temp=(char *)malloc(sizeof(char)*MAX_PATH_LEN);
            strcpy(temp,args[i]);
            execute_command(temp,permenant_home);
            exit(EXIT_SUCCESS);
        }
        else
        {
            wait(NULL);
            close(pipefd[1]);
            fd_in = pipefd[0];
        }    
    }
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdout);
    close(saved_stdin);
    return;
}
