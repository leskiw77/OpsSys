#ifndef OPSSYS_GENERAL_H
#define OPSSYS_GENERAL_H

#include "Fifo.h"

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


#define BARBER 0
#define CLIENTS_FIFO 1
#define VISIT 2

#endif //OPSSYS_GENERAL_H
