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
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <dirent.h>
#include <grp.h>
#include <time.h>
#include <assert.h>
#include <stdbool.h>
#include <ctype.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <termios.h>
#include <sys/time.h>
#include <sys/select.h>


#include "prompt.h"
#include "peek.h"
#include "warp.h"
#include "pastevents.h"
#include "proclore.h"
#include "seek.h"
#include "system.h"
#include "iMan.h"
#include "signal.h"
#include "activities.h"
#include "fg.h"
#include "bg.h"
#include "redirect.h"
#include "pipe.h"
#include "neonate.h"



void execute_command(char *input, char* permenant_home);
#endif