#ifndef HAIRDRESSER_H
#define HAIRDRESSER_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>


//#define PROJECT_ID 37
#define MAX_SIZE 1000
const char env[] = "HOME";

#define BARBER 0
#define FIFO 1
#define CHECKER 2

#endif