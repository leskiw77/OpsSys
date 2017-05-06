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

#include <sys/mman.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>



#define MAX_SIZE 1000

//#define PROJECT_ID 37
const char shmPath[] = "/shm";
const char barberPath[] = "/barber";
const char fifoPath[] = "/fifo";
const char checkerPath[] = "/checker";
const char slowerPath[] = "/slower";

#endif