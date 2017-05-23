#ifndef SYS_GENERAL_H
#define SYS_GENERAL_H

#define ARRAYSIZE 10
#define MAXOPERATION 10
#define MAXNUMBER 100
#define MAXDEVIDER 25
#define ACCESS 0666

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/stat.h>


static const char *RESOURCES_SEM = "resources";
static const char *READERS_SEM = "readers";
static const char *WRITERS_SEM = "writers";
static const char *SHARED_MEM = "shared_memory";

#endif
