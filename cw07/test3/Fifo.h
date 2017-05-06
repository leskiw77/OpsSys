#ifndef OPSSYS_FIFO_H
#define OPSSYS_FIFO_H

#define MAX_SIZE 1000

#include <stdlib.h>
#include <stdio.h>
#include <sys/sem.h>

typedef struct FIFO {
    int size;
    int max;
    pid_t array[MAX_SIZE];
}FIFO;

int isEmpty(FIFO * );
pid_t get_first(FIFO *  );
int put_last(FIFO *, pid_t);


#endif //OPSSYS_FIFO_H
