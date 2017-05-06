#define _GNU_SOURCE

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


#include "hairdresser.h"
#include "Fifo.h"

void printTime() {
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) == -1){
        exit(1);
    }
    printf("Time %ld :", (long)(time.tv_nsec / 1000));
}


void clearResources(void);


void cut(pid_t pid);

pid_t inviteNew(struct sembuf *);

void sigHandler(int signo) {
    clearResources();
    exit(2);
}

key_t fifoKey;
int shmID = -1;
Fifo *fifo = NULL;
int SID = -1;

int main(int argc, char **argv) {
    signal(SIGINT, sigHandler);

    if (argc != 2) {
        printf("Barber needs argument specifying amount of chairs in waiting room\n");
        exit(1);
    }

    int chairsAmount = atoi(argv[1]);
    if (chairsAmount >= MAX_SIZE) {
        printf("No one has so many chairs\n");
        exit(1);
    }


    char *path = getenv(env);
    if (path == NULL){
        return 1;
    }

    fifoKey = ftok(path, PROJECT_ID);
    if (fifoKey == -1){
        return 1;
    }

    shmID = shmget(fifoKey, sizeof(Fifo), IPC_CREAT | 0666);
    if (shmID == -1){
        return 1;
    }

    void *tmp = shmat(shmID, NULL, 0);
    fifo = (Fifo *) tmp;

    fifoInit(fifo, chairsAmount);

    SID = semget(fifoKey, 4, IPC_CREAT | 0666);
    if (SID == -1){
        return 1;
    }

    for (int i = 1; i < 3; i++) {
        if (semctl(SID, i, SETVAL, 1) == -1){
            return 1;
        }
    }
    if (semctl(SID, 0, SETVAL, 0) == -1){
        return 1;
    }


    while (1) {
        struct sembuf buf;
        buf.sem_num = BARBER;
        buf.sem_op = -1;
        buf.sem_flg = 0;

        //golibroda czeka na obudzenie
        if (semop(SID, &buf, 1) == -1){
            return 1;
        }

        printTime();
        printf(" barber awaken and mad\n");

        //klient ktory budzi golibrode
        pid_t toCut = inviteNew(&buf);
        cut(toCut);

        while (1) {
            buf.sem_num = FIFO;
            buf.sem_op = -1;
            if (semop(SID, &buf, 1) == -1){
                return 1;
            }
            toCut = popFifo(fifo); // zajmij FIFO i pobierz pierwszego z kolejki

            if (toCut != -1) { // jesli istnial, to zwolnij kolejke, ostrzyz i kontynuuj
                buf.sem_op = 1;
                if (semop(SID, &buf, 1) == -1){
                    exit(1);
                }
                cut(toCut);
            } else { // jesli kolejka pusta, to ustaw, ze spisz, zwolnij kolejke i spij dalej (wyjdz z petli)
                printTime();
                printf(" barber fall asleep\n");
                buf.sem_num = BARBER;
                buf.sem_op = -1;
                if (semop(SID, &buf, 1) == -1){
                    exit(1);
                }

                buf.sem_num = FIFO;
                buf.sem_op = 1;
                if (semop(SID, &buf, 1) == -1){
                    exit(1);
                }
                break;
            }
        }
    }
    return 0;
}


pid_t inviteNew(struct sembuf *buf) {
    buf->sem_num = FIFO;
    buf->sem_op = -1;
    if (semop(SID, buf, 1) == -1){
        exit(1);
    }

    pid_t toCut = fifo->chair;

    buf->sem_op = 1;
    if (semop(SID, buf, 1) == -1){
        exit(1);
    }

    return toCut;
}

void cut(pid_t pid) {
    printTime();
    printf(" barber cut user %d\n", pid);

    kill(pid, SIGRTMIN);
}


void clearResources(void) {
    shmdt(fifo);
    shmctl(shmID, IPC_RMID, NULL);
    semctl(SID, 0, IPC_RMID);
    printf("\nBarber cleaned his workplace and went home\n");
}