#ifndef __REDIRECT_H__
#define __REDIRECT_H__

bool FileExists(char *filename);
bool isRedirect(char *input);
bool inputRedirect(char *input, char *permenant_home);
bool OutputRedirect(char *input, char *permenant_home);
char* changeCommand(char **args);
void Redirect(char *input, char *permenant_home);

#endif