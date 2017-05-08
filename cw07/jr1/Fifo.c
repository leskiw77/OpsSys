#include "Fifo.h"

int isEmpty(Fifo* fifo){
    if(fifo->head != -1)
        return 0;
    return 1;
}

void fifoInit(Fifo* fifo, int cn){
    fifo->chair = 0;
    fifo->head = -1;
    fifo->tail = 0;
    fifo->max = cn;
}

int isFull(Fifo* fifo){
    if(fifo->head != fifo->tail)
        return 0;
    return 1;
}

pid_t takeFirst(Fifo* fifo){
    if(fifo->head == -1)
        return -1;

    fifo->chair = fifo->tab[fifo->head++];

    if(fifo->head == fifo->tail)
        fifo->head = -1;

    if(fifo->head == fifo->max)
        fifo->head = 0;

    return fifo->chair;
}

int addLast(Fifo* fifo, pid_t x){
    if(fifo->head == fifo->tail)
        return -1;
    if(fifo->head == -1){
        fifo->head = fifo->tail;
    }

    fifo->tab[fifo->tail++] = x;

    if(fifo->tail == fifo->max)
        fifo->tail = 0;
    return 0;
}