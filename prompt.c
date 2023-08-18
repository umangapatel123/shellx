#include "headers.h"

char* string_cut(char *str, int begin, int len)
{
    if (str == NULL || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len))
        return NULL;
    return strndup(str + begin, len);
}

void prompt(char *permenant_home) 
{
    // Do not hardcode the prmopt
    char *user = getlogin();
    char *host = malloc(MAX_PATH_LEN * sizeof(char));
    gethostname(host, MAX_PATH_LEN);
    char *cwd = malloc(MAX_PATH_LEN * sizeof(char));
    getcwd(cwd, MAX_PATH_LEN);
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    char *home = pw->pw_dir;
    char *prompt = malloc(MAX_PATH_LEN * sizeof(char));
    if (strcmp(cwd, permenant_home) == 0) {
        strcpy(prompt, "~");
    }
    else if (strstr(cwd, permenant_home) != NULL) {
        strcpy(prompt, "~");
        cwd = string_cut(cwd, strlen(permenant_home), strlen(cwd) - strlen(permenant_home));
        strcat(prompt, cwd);
    } else {
        strcpy(prompt, cwd);
    }
    // give the prompt a nice color
    printf("<\033[1;32m%s@%s\033[0m:\033[1;34m%s\033[0m> ", user, host, prompt);
    free(host);
    free(cwd);
    free(prompt);
       
}
