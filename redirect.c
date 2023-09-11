#include "headers.h"

bool FileExists(char *filename)
{
    struct stat buffer;
    if(stat(filename, &buffer) == 0)
        return true;
    return false;
}

bool isRedirect(char *input)
{
    if(strstr(input, "<") != NULL)
        return true;
    if(strstr(input, ">") != NULL)
        return true;
    return false;
}

bool inputRedirect(char *input, char *permenant_home)
{
    char *temp = (char *)malloc(100000*sizeof(char));
    strcpy(temp, input);
    char *token = strtok(temp, "<");
    token = strtok(NULL, "<");
    char *filename = strtok(token, " \t\r\n");
    if(filename == NULL)
    {
        printf("Error: No filename provided\n");
        return false;
    }
    if(!FileExists(filename))
    {
        printf("Error: File does not exist\n");
        return false;
    }
    int fd = open(filename, O_RDONLY);
    if(fd == -1)
    {
        printf("Error: Cannot open file\n");
        return false;
    }
    dup2(fd, STDIN_FILENO);
    close(fd);
    return true;
}

bool OutputRedirect(char *input, char *permenant_home)
{
    char *temp = (char *)malloc(100000*sizeof(char));
    strcpy(temp, input);
    int redirect_type = 0;
    if(strstr(temp, ">>") != NULL)
        redirect_type = 2;
    else if(strstr(temp, ">") != NULL)
        redirect_type = 1;
    if(redirect_type == 0)
    {
        printf("Error: No redirection symbol provided\n");
        return false;
    }
    char *token = malloc(100000*sizeof(char));
    if(redirect_type==1)
    {
        token = strtok(temp, ">");
        token = strtok(NULL, ">");
    }
    else
    {
        token = strtok(temp, ">>");
        token = strtok(NULL, ">>");
    }
    char *filename = strtok(token, " \t\r\n");
    if(filename == NULL)
    {
        printf("Error: No filename provided\n");
        return false;
    }
    int fd;

    if(redirect_type==1)
    {
        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    } 
    else if (redirect_type==2)
    {
        fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    }
    if(fd == -1)
    {
        printf("Error: Cannot open file\n");
        return false;
    }

    dup2(fd, STDOUT_FILENO);
    close(fd);

    return true;

}

char* changeCommand(char **args)
{
    char *command = malloc(100000*sizeof(char));
    int i=0;
    while(args[i] != NULL)
    {
        if(strcmp(args[i], "<") == 0)
        {
            i++;
        }
        else if(strcmp(args[i], ">") == 0)
        {
            i++;
        }
        else if(strcmp(args[i], ">>") == 0)
        {
            i++;
        }
        else
        {
            strcat(command, args[i]);
            strcat(command, " ");
        }
        i++;
    }
    return command;
}

void Redirect(char *input, char *permenant_home)
{
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);
    if(strstr(input, "<") != NULL)
    {
        if(!inputRedirect(input, permenant_home))
            return;
    }
    if(strstr(input, ">") != NULL)
    {
        if(!OutputRedirect(input, permenant_home))
            return;
    }
    char *temp = (char *)malloc(100000*sizeof(char));
    strcpy(temp, input);
    char **args = malloc(100000*sizeof(char *));
    char *token = strtok(temp, " \t\r\n");
    int i=0;
    while(token != NULL)
    {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t\r\n");
    }
    args[i] = NULL;
    char *command = changeCommand(args);
    execute_command(command, permenant_home);
    dup2(saved_stdin, STDIN_FILENO);
    close(saved_stdin);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
    return;
}