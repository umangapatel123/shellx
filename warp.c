#include "headers.h"

// Function to change directory
void warp(char *path, char *permenant_home, char **previous_directory)
{
    // If no path is given, go to home directory
    if (path == NULL)
    {
        struct passwd *pw = getpwuid(getuid());
        path = pw->pw_dir;
    }
    // If path is ~, go to home directory
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

    // Change directory

    // If path is -, go to previous directory
    char *temp = malloc(MAX_PATH_LEN * sizeof(char));
    if (strcmp(path, "-") == 0)
    {
        if (*previous_directory == NULL)
        {
            printf("Error: OLDPWD not set\n");
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
        perror("Error");
    }
    else
    {
        printf("%s\n", getcwd(temp, MAX_PATH_LEN));
    }
    free(temp);
}