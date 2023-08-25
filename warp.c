#include "headers.h"
extern bool temp_flag;

// Function to change directory
void warp(char *path, char *permenant_home, char **previous_directory,char *command_name)
{

    if (path == NULL)
    {
        path = permenant_home;
    }

    if (strcmp(path, "~") == 0)
    {
        path = permenant_home;
    }
    if (strstr(path, "~") != NULL)
    {
        char *temp = malloc(MAX_PATH_LEN * sizeof(char));
        strcpy(temp, permenant_home);
        strcat(temp, path + 1);
        path = temp;
    }

    // If path is -, go to previous directory
    char *temp = malloc(MAX_PATH_LEN * sizeof(char));
    if (strcmp(path, "-") == 0)
    {
        if (*previous_directory == NULL)
        {
            printf("Error: OLDPWD not set\n");
            temp_flag = false;
            return;
        }
        strcpy(temp, *previous_directory);
        path = temp;
    }
    if (*previous_directory == NULL)
    {
        *previous_directory = malloc(MAX_PATH_LEN * sizeof(char));
    }
    getcwd(*previous_directory, MAX_PATH_LEN);
    int ret = chdir(path);
    if (ret == -1)
    {
        temp_flag = false;
        perror("Error");
    }
    else
    {
        temp_flag = true;  
        printf("%s\n", getcwd(temp, MAX_PATH_LEN));
    }
    free(temp);
}