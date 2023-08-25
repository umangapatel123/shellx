#include "headers.h"

char *original_search_directory = NULL;
int count = 0;
bool already_printed = false;
bool flag_change = false;
char *flag_dir_change = NULL;
bool flag_file_change = false;
char *flag_file_print = NULL;

char *string_cut_new(char *str, int start, int end)
{
    char *temp = malloc(MAX_PATH_LEN * sizeof(char));
    temp[0] = '.';
    int j = 1;
    for (int i = start; i < end; i++)
    {
        temp[j++] = str[i];
    }
    temp[j] = '\0';
    return temp;
}

bool CompareFiles(char *a ,char *b)
{
    int len1=strlen(a);
    int len2=strlen(b);
    for(int i=0;i<len1;i++)
    {
        if(a[i]!=b[i])
            return false;
    }
    return true;
}

void seek_serach(char *target, char *target_directory, int flags)
{
    if (strcmp(target, ".") == 0 || strcmp(target, "..") == 0)
    {
        printf("Error: Invalid directory\n");
        already_printed = true;
        return;
    }
    DIR *dir;
    struct dirent *dp;
    if ((dir = opendir(target_directory)) == NULL)
    {
        perror("opendir");
        already_printed = true;
        return;
    }
    while ((dp = readdir(dir)) != NULL)
    {
        struct stat file_stat_temp;
        char *path_file_temp = malloc(MAX_PATH_LEN * sizeof(char));
        sprintf(path_file_temp, "%s/%s", target_directory, dp->d_name);
        if (stat(path_file_temp, &file_stat_temp) < 0)
        {
            already_printed = true;
            perror("stat");
        }
        char *token = NULL;

        if (CompareFiles(target, dp->d_name))
        {

            char *path_file = malloc(MAX_PATH_LEN * sizeof(char));
            sprintf(path_file, "%s/%s", target_directory, dp->d_name);
            struct stat file_stat;
            if (stat(path_file, &file_stat) < 0)
            {
                perror("stat");
                already_printed = true;
                return;
            }
            if ((flags == 0 || flags == 4) && flags != 1 && flags != 2)
            {
                if (S_ISDIR(file_stat.st_mode))
                {
                    char *print=string_cut_new(path_file, strlen(original_search_directory), strlen(path_file));
                    printf("\033[0;34m%s\033[0m\n", print);
                    flag_dir_change = path_file;
                    flag_change = true;
                }
                else
                {
                    char *print=string_cut_new(path_file, strlen(original_search_directory), strlen(path_file));
                    printf("\033[0;32m%s\033[0m\n", print);
                    flag_file_print = path_file;
                    flag_file_change = true;
                }
                count++;
            }
            if (S_ISDIR(file_stat.st_mode))
            {
                if (flags & 1)
                {
                    char *print=string_cut_new(path_file, strlen(original_search_directory), strlen(path_file));
                    printf("\033[0;34m%s\033[0m\n", print);
                    count++;
                    flag_dir_change = path_file;
                    flag_change = true;
                }
                seek_serach(target, path_file, flags);
            }
            else if (!S_ISDIR(file_stat.st_mode))
            {
                if (flags & 2)
                {
                    char *print=string_cut_new(path_file, strlen(original_search_directory), strlen(path_file));
                    printf("\033[0;32m%s\033[0m\n", print);
                    count++;
                    flag_file_print = path_file;
                    flag_file_change = true;
                }
            }
            // count++;
        }
        else
        {
            struct stat file_stat_trial;
            if (stat(path_file_temp, &file_stat_trial) < 0)
            {
                already_printed = true;
                perror("stat");
            }
            if (S_ISDIR(file_stat_trial.st_mode))
            {
                if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
                {
                    char *path_file = malloc(MAX_PATH_LEN * sizeof(char));
                    sprintf(path_file, "%s/%s", target_directory, dp->d_name);
                    seek_serach(target, path_file, flags);
                }
            }
        }
    }
    closedir(dir);
}

void seek(char **args, char *permenant_home, int args_count)
{
    int flags = 0;
    char *target = NULL;
    char *target_directory = ".";

    for (int i = 0; i < args_count; i++)
    {
        if (args[i][0] == '-')
        {
            if (args[i][1] == 'd')
            {
                flags = flags | 1;
            }
            else if (args[i][1] == 'f')
            {
                flags = flags | 2;
            }
            else if (args[i][1] == 'e')
            {
                flags = flags | 4;
            }
            else
            {
                printf("Error: '%s' is not a valid flag\n", args[i]);
                return;
            }
        }
        else if (target == NULL)
        {
            target = args[i];
        }
        else
        {
            target_directory = args[i];
        }
    }

    if (target == NULL)
    {
        printf("Error: No search target specified\n");
        return;
    }
    if (flags & 1 && flags & 2)
    {
        printf("Invalid flags!\n");
        return;
    }

    count = 0;
    original_search_directory = target_directory;
    seek_serach(target, target_directory, flags);
    if (count == 0 && !already_printed)
    {
        printf("No match found!\n");
    }

    if (flags & 4 && count == 1)
    {
        int response=__INT_MAX__;
        if (flag_change)
        {
           response = chdir(flag_dir_change);
        }
        if(response==-1){
            printf("Missing permissions for task!\n");
        }
        
        if (flag_file_change)
        {
            FILE *fp = fopen(flag_file_print, "r");
            if (fp == NULL)
            {
                printf("Missing permissions for task!\n");
                return;
            }
            char ch;
            while ((ch = fgetc(fp)) != EOF)
            {
                printf("%c", ch);
            }
            fclose(fp);
        }
        
    }
    flag_change = false;
    flag_dir_change = NULL;
    flag_file_change = false;
    flag_file_print = NULL;
    original_search_directory = NULL;
    return;
}
