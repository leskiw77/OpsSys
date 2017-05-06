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

#include "helpers.h" // popFragment, trimWhite, getQID
#include "Fifo.h"
#include "hairdresser.h"



void clearResources(void);

void prepareFifo(int chNum);

void prepareSemafors();

void napAndWorkForever();

void cut(pid_t pid);

pid_t takeChair(struct sembuf *);

key_t fifoKey;
int shmID = -1;
Fifo *fifo = NULL;
int SID = -1;

void sigHandler(int signo) {
    clearResources();
    exit(2);
}


int main(int argc, char **argv){

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
        exit(1);
    }

    fifoKey = ftok(path, PROJECT_ID);
    if (fifoKey == -1){
        exit(1);
    }

    shmID = shmget(fifoKey, sizeof(Fifo), IPC_CREAT | IPC_EXCL | 0666);
    if (shmID == -1){
        exit(1);
    }

    void *tmp = shmat(shmID, NULL, 0);
    fifo = (Fifo *) tmp;

    fifoInit(fifo, chairsAmount);

    SID = semget(fifoKey, 4, IPC_CREAT | IPC_EXCL | 0666);
    if (SID == -1){
        exit(1);
    }

    for (int i = 1; i < 3; i++) {
        if (semctl(SID, i, SETVAL, 1) == -1){
            exit(1);
        }
    }
    if (semctl(SID, 0, SETVAL, 0) == -1){
        exit(1);
    }


    while (1) {
        struct sembuf buf;
        buf.sem_num = BARBER;
        buf.sem_op = -1;
        buf.sem_flg = 0;

        //golibroda czeka na przebudzenie
        if (semop(SID, &buf, 1) == -1){
            exit(1);
        }

        pid_t toCut = takeChair(&buf);
        cut(toCut);

        while (1) {
            buf.sem_num = FIFO; // dla czytelnosci
            buf.sem_op = -1;
            if (semop(SID, &buf, 1) == -1)
                toCut = popFifo(fifo); // zajmij FIFO i pobierz pierwszego z kolejki

            if (toCut != -1) { // jesli istnial, to zwolnij kolejke, ostrzyz i kontynuuj
                buf.sem_op = 1;
                if (semop(SID, &buf, 1) == -1) throwEx("Barber: 4 buf failed!");
                cut(toCut);
            } else { // jesli kolejka pusta, to ustaw, ze spisz, zwolnij kolejke i spij dalej (wyjdz z petli)
                long timeMarker = getMicroTime();
                printf("Time: %ld, Barber: going to sleep...\n", timeMarker);
                fflush(stdout);
                buf.sem_num = BARBER;
                buf.sem_op = -1;
                if (semop(SID, &buf, 1) == -1) throwEx("Barber: 5 buf failed!");

                buf.sem_num = FIFO;
                buf.sem_op = 1;
                if (semop(SID, &buf, 1) == -1) throwEx("Barber: 6 buf failed!");
                break;
            }
        }
    }

    return 0;
}



pid_t takeChair(struct sembuf *buf) {
    buf->sem_num = FIFO;
    buf->sem_op = -1;
    if (semop(SID, buf, 1) == -1) throwEx("Barber: 1 buf failed!");

    pid_t toCut = fifo->chair;

    buf->sem_op = 1;
    if (semop(SID, buf, 1) == -1) throwEx("Barber: 2 buf failed!");

    return toCut;
}

void cut(pid_t pid) {
    long timeMarker = getMicroTime();
    printf("Time: %ld, Barber: preparing to cut %d\n", timeMarker, pid);
    fflush(stdout);

    kill(pid, SIGRTMIN);

    timeMarker = getMicroTime();
    printf("Time: %ld, Barber: finished cutting %d\n", timeMarker, pid);
    fflush(stdout);
}


void clearResources(void) {
    if (shmdt(fifo) == -1) printf("Barber: Error detaching fifo sm!\n");
    else printf("Barber: detached fifo sm!\n");

    if (shmctl(shmID, IPC_RMID, NULL) == -1) printf("Barber: Error deleting fifo sm!\n");
    else printf("Barber: deleted fifo sm!\n");

    if (semctl(SID, 0, IPC_RMID) == -1) printf("Barber: Error deleting semafors!");
    else printf("Barber: deleted semafors!\n");
}