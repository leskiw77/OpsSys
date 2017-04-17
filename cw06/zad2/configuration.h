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

int is_prime(int n) {
    if (n == 2 || n == 3 || n == 5)
        return 1;
    if (n < 2 || n % 2 == 0 || n % 3 == 0 || n % 5 == 0)
        return 0;
    for (int i = 7; i * i <= n;) {
        if (n % i == 0)
            return 0;
        i += 4;
        if (n % i == 0)
            return 0;
        i += 2;
    }
    return 1;
}

#define MAX_SIZE 100
#define MAXCLIENTS 10

#define NEWCLNT 1
#define SERACCLIENT 2
#define CLRESP 3
#define GET_ID 4
#define CLSCLNT 5

#define NEW_CLIENT 6
#define ECHO 7
#define TO_UPPER_CASE 8
#define GET_TIME 9
#define EXIT 10

#define MAXLENGTH 512
#define MAXNAME 100
#define MAXMSGSIZE 4096

#endif //OPSSYS_COMMON_H
