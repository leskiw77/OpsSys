#ifndef FIFO_H
#define FIFO_H

#include <time.h>
#include <stdlib.h>


typedef struct Fifo {
    int max;
    int head;
    int tail;
    pid_t chair;
    pid_t tab[1000];
} Fifo;

void fifoInit(Fifo *fifo, int cn);

pid_t takeFirst(Fifo *fifo);

int addLast(Fifo *fifo, pid_t x);

int isEmpty(Fifo *fifo);

int isFull(Fifo *fifo);

#endif