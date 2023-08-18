#include "headers.h"
char *previous_directory = NULL;

int main()
{
    char *permenant_home = malloc(MAX_PATH_LEN * sizeof(char));
    getcwd(permenant_home, MAX_PATH_LEN);
    // Keep accepting commands
    while (1)
    {
        // Print appropriate prompt with username, systemname and directory before accepting input
        prompt(permenant_home);
        char input[4096];
        fgets(input, 4096, stdin);
        char dup[4096];
        strcpy(dup, input);
        char *token = strtok(dup, " \t\n");
        // If no command is given, continue
        if (token == NULL)
        {
            continue;
        }
        // If command is exit, exit
        if (strcmp(token, "exit") == 0)
        {
            exit(0);
        }
        // If command is warp, change directory
        if (strcmp(token, "warp") == 0)
        {
            while (token != NULL)
            {
                token = strtok(NULL, " \t\n");
                if (token == NULL)
                {
                    break;
                }
                warp(token, permenant_home, &previous_directory);
            }
            continue;
        }

        // If command is peek, list files in directory
        if (strcmp(token, "peek") == 0)
        {
            char **args = malloc(MAX_PATH_LEN * sizeof(char *));
            int i = 0;
            while (token != NULL)
            {
                token = strtok(NULL, " \t\n");
                if (token == NULL)
                {
                    break;
                }
                args[i] = token;
                i++;
            }
            printf("%d\n", i);
            args[i] = NULL;
            peek(args);
            continue;
        }

    }
}
