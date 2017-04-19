#ifndef CONFIG_H
#define CONFIG_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>

#define MAXLENGTH 512
#define MAXNAME 100
#define MAXCLIENTS 30
#define MAXMSGSIZE 4096

#define DEFAULT 1
#define ECHO 2
#define TO_UPPER 3
#define GET_TIME 4
#define EXIT 5

struct mymesg {
    long mtype;
    int mrequest;
    char mtext[MAXMSGSIZE];
};

#endif