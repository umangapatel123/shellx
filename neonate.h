#ifndef __NEONATE_H__
#define __NEONATE_H__

void die(const char *s);
void disableRawMode();
void enableRawMode();
int get_recent_pid();
bool key_detect(char *inp, char c);
void neonate(char **args, char *permenant_home);

#endif