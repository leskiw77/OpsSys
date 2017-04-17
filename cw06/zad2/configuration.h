#ifndef OPSSYS_COMMON_H
#define OPSSYS_COMMON_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <mqueue.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#define MAX_SIZE 100
#define MAXCLIENTS 10

#define NEWCLNT 1
#define SERACCLIENT 2
#define CLRESP 3
#define GET_ID 4

#define NEW_CLIENT 6
#define ECHO 7
#define TO_UPPER_CASE 8
#define GET_TIME 9
#define EXIT 10

#define MAXLENGTH 512
#define MAXNAME 100
#define MAXMSGSIZE 4096

#endif //OPSSYS_COMMON_H
