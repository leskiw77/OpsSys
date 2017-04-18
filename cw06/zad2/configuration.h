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
#define MAX_CLIENTS 10

#define TEXT 0
#define NEW_CLIENT 1
#define ECHO 2
#define TO_UPPER_CASE 3
#define GET_TIME 4
#define EXIT 5

#endif //OPSSYS_COMMON_H
