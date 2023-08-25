#include "peek.h"
#include "headers.h"
extern bool temp_flag;

struct FileEntry
{
    char name[256];
    struct stat stats;
};

typedef struct FileEntry FileEntry;

int compare(const void *a, const void *b)
{
    FileEntry *file1 = (FileEntry *)a;
    FileEntry *file2 = (FileEntry *)b;
    return strcmp(file1->name, file2->name);
}

int is_hidden(char *name)
{
    if (name[0] == '.')
    {
        return 1;
    }
    return 0;
}

int make_files(char *path, FileEntry **files)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        temp_flag = false;
        perror("Error");
        return -1;
    }
    struct dirent *entry;
    int i = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (is_hidden(entry->d_name))
        {
            continue;
        }
        FileEntry file;
        strcpy(file.name, entry->d_name);
        char *temp_path=(char *)malloc(sizeof(char)*MAX_PATH_LEN);
        strcpy(temp_path,path);
        strcat(temp_path,"/");
        strcat(temp_path,file.name);    
        if (stat(temp_path, &file.stats) == -1)
        {
            temp_flag = false;
            perror("Error");
            return -1;
        }
        free(temp_path);
        *files = realloc(*files, sizeof(struct FileEntry) * (i + 1));
        (*files)[i] = file;
        i++;
    }
    qsort(*files, i, sizeof(struct FileEntry), compare);
    closedir(dir);
    return i;
}

int make_list_hidden(char *path, FileEntry **files)
{
    DIR *dir = opendir(path);
    if (dir == NULL)
    {
        temp_flag = false;
        perror("Error");
        return -1;
    }
    struct dirent *entry;
    int i = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        FileEntry file;
        strcpy(file.name, entry->d_name);
        char *temp_path=(char *)malloc(sizeof(char)*MAX_PATH_LEN);
        strcpy(temp_path,path);
        strcat(temp_path,"/");
        strcat(temp_path,file.name);

        if (stat(temp_path, &file.stats) == -1)
        {
            temp_flag = false;
            perror("Error");
            return -1;
        }
        free(temp_path);
        *files = realloc(*files, sizeof(struct FileEntry) * (i + 1));
        (*files)[i] = file;
        i++;
    }
    qsort(*files, i, sizeof(struct FileEntry), compare);
    closedir(dir);
    return i;
}

void Print_List(FileEntry *files, int n)
{
    for (int j = 0; j < n; j++)
    {
        // give color to files, green for executable, blue for directories and white for other files
        if (S_ISDIR(files[j].stats.st_mode))
        {
            printf("\033[1;34m%s\033[0m\n", files[j].name);
        }
        else if (files[j].stats.st_mode & S_IXUSR)
        {
            printf("\033[1;32m%s\033[0m\n", files[j].name);
        }
        else
        {
            printf("%s\n", files[j].name);
        }
    }
}

char* convert_permissions(mode_t mode)
{
    char *permissions = (char *)malloc(sizeof(char) * 10);
    permissions[0] = (S_ISDIR(mode)) ? 'd' : '-';
    permissions[1] = (mode & S_IRUSR) ? 'r' : '-';
    permissions[2] = (mode & S_IWUSR) ? 'w' : '-';
    permissions[3] = (mode & S_IXUSR) ? 'x' : '-';
    permissions[4] = (mode & S_IRGRP) ? 'r' : '-';
    permissions[5] = (mode & S_IWGRP) ? 'w' : '-';
    permissions[6] = (mode & S_IXGRP) ? 'x' : '-';
    permissions[7] = (mode & S_IROTH) ? 'r' : '-';
    permissions[8] = (mode & S_IWOTH) ? 'w' : '-';
    permissions[9] = (mode & S_IXOTH) ? 'x' : '-';
    permissions[10] = '\0';
    return permissions;
}

void Print_List_With_L(FileEntry *files, int n)
{   
    int total = 0;
    for (int j = 0; j < n; j++)
    {
        total += files[j].stats.st_blocks;
    }
    printf("total %d\n", total);

    for (int j = 0; j < n; j++)
    {
        char *permissions = convert_permissions(files[j].stats.st_mode);

        char time[100];
        strftime(time, 100, "%b %d %H:%M", localtime(&(files[j].stats.st_ctime)));
        
        if (S_ISDIR(files[j].stats.st_mode))
        {
            // just file name in blue
            printf("%s %ld %s %s %ld\t%s \033[1;34m%s\033[0m\n", permissions, files[j].stats.st_nlink, getpwuid(files[j].stats.st_uid)->pw_name, getgrgid(files[j].stats.st_gid)->gr_name, files[j].stats.st_size,time , files[j].name);
        }
        else if (files[j].stats.st_mode & S_IXUSR)
        {
            printf("%s %ld %s %s %ld\t%s \033[1;32m%s\033[0m\n", permissions, files[j].stats.st_nlink, getpwuid(files[j].stats.st_uid)->pw_name, getgrgid(files[j].stats.st_gid)->gr_name, files[j].stats.st_size,time , files[j].name);
        }
        else
        {
            printf("%s %ld %s %s %ld\t%s %s\n", permissions, files[j].stats.st_nlink, getpwuid(files[j].stats.st_uid)->pw_name, getgrgid(files[j].stats.st_gid)->gr_name, files[j].stats.st_size,time , files[j].name);
        }

        // give color 
    }
}

char* get_complete_path(char *path, char *permenant_home)
{
    char *complete_path = (char *)malloc(sizeof(char) * MAX_PATH_LEN);
    if (path[0] == '~')
    {
        strcpy(complete_path, permenant_home);
        strcat(complete_path, path + 1);
    }
    else
    {
        strcpy(complete_path, path);
    }
    complete_path[strlen(complete_path)] = '\0';
    return complete_path;
}

void peek(char **args, char *permenant_home,char* command_name)
{
    // If no directory is given, list files in current directory
    FileEntry *files = NULL;
    if (args[0] == NULL)
    {
        int i = make_files(".", &files);
        Print_List(files, i);
        temp_flag = true;
        return;
    }
    // check if args[0] is a directory or flag
    if (strcmp(args[0], "-a") == 0 || strcmp(args[0], "-l") == 0 || strcmp(args[0], "-al") == 0 || strcmp(args[0], "-la") == 0)
    {
        if (args[1] == NULL)
        {
            if (strcmp(args[0], "-a") == 0)
            {
                int i = make_list_hidden(".", &files);
                Print_List(files, i);
                temp_flag = true;
                return;
            }
            else if (strcmp(args[0], "-l") == 0)
            {
                int i = make_files(".", &files);
                Print_List_With_L(files, i);
                temp_flag = true;
                return;
            }
            else if (strcmp(args[0], "-al") == 0 || strcmp(args[0], "-la") == 0)
            {
                int i = make_list_hidden(".", &files);
                Print_List_With_L(files, i);
                temp_flag = true;
                return;
            }
        }
        else
        {
            if (strcmp(args[0], "-a") == 0)
            {
                if(strcmp(args[1], "-l") == 0)
                {
                    if(args[2] == NULL)
                    {
                        int i = make_list_hidden(".", &files);
                        Print_List_With_L(files, i);
                        temp_flag = true;
                        return;
                    }
                    else
                    {
                        char *complete_path = get_complete_path(args[2], permenant_home);
                        int i = make_list_hidden(complete_path, &files);
                        Print_List_With_L(files, i);
                        temp_flag = true;
                        return;
                    }
                }
                else
                {
                    char *complete_path = get_complete_path(args[1], permenant_home);
                    int i = make_list_hidden(complete_path, &files);
                    Print_List(files, i);
                    temp_flag = true;
                    return;
                }
            }
            else if (strcmp(args[0], "-l") == 0)
            {
                if(strcmp(args[1], "-a") == 0)
                {
                    if(args[2] == NULL)
                    {
                        int i = make_list_hidden(".", &files);
                        Print_List_With_L(files, i);
                        temp_flag = true;
                        return;
                    }
                    else
                    {
                        char *complete_path = get_complete_path(args[2], permenant_home);
                        int i = make_list_hidden(complete_path, &files);
                        Print_List_With_L(files, i);
                        temp_flag = true;
                        return;
                    }
                }
                else
                {
                    char *complete_path = get_complete_path(args[1], permenant_home);
                    int i = make_files(complete_path, &files);
                    Print_List_With_L(files, i);
                    temp_flag = true;
                    return;
                }
            }
            else if (strcmp(args[0], "-al") == 0 || strcmp(args[0], "-la") == 0)
            {
                if(args[1] == NULL)
                {
                    int i = make_list_hidden(".", &files);
                    Print_List_With_L(files, i);
                    temp_flag = true;
                    return;
                }
                else
                {
                    char *complete_path = get_complete_path(args[1], permenant_home);
                    int i = make_list_hidden(complete_path, &files);
                    Print_List_With_L(files, i);
                    temp_flag = true;
                    return;
                }
            }
        }
    }
    else
    {
        char *complete_path = get_complete_path(args[0], permenant_home);
        int i = make_files(complete_path, &files);
        Print_List(files, i);
        temp_flag = true;
        return;
    }
    return;
}