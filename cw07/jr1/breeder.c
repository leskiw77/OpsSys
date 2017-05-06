#define _GNU_SOURCE

#include "Fifo.h"
#include "general.h"


void printTime() {
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) == -1){
        exit(1);
    }
    printf("Time %ld :", (long)(time.tv_nsec / 1000));
}



void prepareFullMask();

void freeResources(void);

int enterBarber();

void getCut(int);

key_t fifoKey;
int shmID = -1;
Fifo *fifo = NULL;
int SID = -1;

int counter = 0;
sigset_t fullMask;

void rtminHandler(int signo) {
    counter++;
}

int main(int argc, char **argv) {

    if (argc != 3) {
        printf("Need 2 arguments: number of clients and cuts\n");
        exit(1);
    }
    int clientsAmount = atoi(argv[1]);
    int serviceAmount = atoi(argv[2]);

    signal(SIGRTMIN, rtminHandler);


    char *path = getenv(env);
    if (path == NULL){
        exit(1);
    }

    fifoKey = ftok(path, 'f');
    if (fifoKey == -1){
        exit(1);
    }

    shmID = shmget(fifoKey, 0, 0);
    if (shmID == -1){
        exit(1);
    }

    void *tmp = shmat(shmID, NULL, 0);
    if (tmp == (void *) (-1)){
        exit(1);
    }
    fifo = (Fifo *) tmp;

    SID = semget(fifoKey, 0, 0);
    if (SID == -1) {
        exit(1);
    }


    prepareFullMask();

    sigset_t mask;
    if (sigemptyset(&mask) == -1){
        exit(1);
    }
    if (sigaddset(&mask, SIGRTMIN) == -1) {
        exit(1);
    }
    if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1){
        exit(1);
    }



    for (int i = 0; i < clientsAmount; i++) {
        pid_t id = fork();
        if (id == 0) {
            getCut(serviceAmount);
            return 0;
        }
    }


    while (1) {
        wait(NULL);
        if (errno == ECHILD) break;
    }
    printf("All clients serviced!\n");
    return 0;
}

void getCut(int serviceAmount) {
    while (counter < serviceAmount) {
        struct sembuf buf;
        buf.sem_num = CHECKER;
        buf.sem_op = -1;
        buf.sem_flg = 0;
        if (semop(SID, &buf, 1) == -1){
            exit(1);
        }

        buf.sem_num = FIFO;
        if (semop(SID, &buf, 1) == -1){
            exit(1);
        }

        int res = enterBarber();

        buf.sem_op = 1;
        if (semop(SID, &buf, 1) == -1){
            exit(1);
        }

        buf.sem_num = CHECKER;
        if (semop(SID, &buf, 1) == -1){
            exit(1);
        }

        if (res != -1) {
            sigsuspend(&fullMask);
            printTime();
            printf(" client %d was serviced!\n", getpid());
        }
    }
}

int enterBarber() {
    int barbState = semctl(SID, 0, GETVAL);
    if (barbState == -1){
        exit(1);
    }

    if (barbState == 0) {
        struct sembuf buf;
        buf.sem_num = BARBER;
        buf.sem_op = 1;
        buf.sem_flg = 0;

        if (semop(SID, &buf, 1) == -1){
            exit(1);
        }
        printTime();
        printf(" client %d is awakening barber!\n", getpid());
        if (semop(SID, &buf, 1) == -1) {
            exit(1);
        }

        fifo->chair = getpid();

        return 1;
    } else {
        int res = addLast(fifo, getpid());
        if (res == -1) {
            printTime();
            printf(" client %d has no place inside\n", getpid());
            return -1;
        } else {
            printTime();
            printf(" client %d is in the queue now\n", getpid());
            return 0;
        }
    }
}

void prepareFullMask() {
    if (sigfillset(&fullMask) == -1) {
        exit(1);
    }
    if (sigdelset(&fullMask, SIGRTMIN) == -1) {
        exit(1);
    }
    if (sigdelset(&fullMask, SIGINT) == -1) {
        exit(1);
    }
}
