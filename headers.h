#ifndef HEADERS_H_
#define HEADERS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <grp.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>


#include "prompt.h"
#include "peek.h"
#include "warp.h"
#include "pastevents.h"
#include "proclore.h"
#include "seek.h"
#include "system.h"




void execute_command(char *input, char* permenant_home);
#endif