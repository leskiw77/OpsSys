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



void clearResources(void);

void cut(pid_t pid);

pid_t inviteNew();

Fifo *fifo = NULL;


void intHandler(int signo) {
    clearResources();
    exit(2);
}
void sigHandler(int signo) {
    clearResources();
    exit(2);
}

sem_t *BARBER_VAL;
sem_t *FIFO_VAL;
sem_t *WAKE_VAL;
sem_t *BLOCK_SAME;

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

    int shmID = shm_open(shmPath, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (shmID == -1) {
        exit(1);
    }

    if (ftruncate(shmID, sizeof(Fifo)) == -1) {
        exit(1);
    }

    void *tmp = mmap(NULL, sizeof(Fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    if (tmp == (void *) (-1)){
        exit(1);
    }
    fifo = (Fifo *) tmp;

    fifoInit(fifo, chairsAmount);

    BARBER_VAL = sem_open(barberPath, O_CREAT | O_EXCL | O_RDWR, 0666, 0);
    if (BARBER_VAL == SEM_FAILED) {
        exit(1);
    }

    FIFO_VAL = sem_open(fifoPath, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    if (FIFO_VAL == SEM_FAILED)  {
        exit(1);
    }

    WAKE_VAL = sem_open(wakePath, O_CREAT | O_EXCL | O_RDWR, 0666, 1);
    if (WAKE_VAL == SEM_FAILED)  {
        exit(1);
    }

    BLOCK_SAME = sem_open(blockPath, O_CREAT | O_EXCL | O_RDWR, 0666, 0);
    if (BLOCK_SAME == SEM_FAILED) {
        exit(1);
    }


    while (1) {
        if (sem_wait(BARBER_VAL) == -1)  {
            exit(1);
        }

        if (sem_post(BARBER_VAL) == -1)  {
            exit(1);
        }

        if (sem_post(BLOCK_SAME) == -1) {
            exit(1);
        }

        printTime();
        printf(" barber awaken and mad\n");

        pid_t toCut = inviteNew();
        cut(toCut);

        while (1) {
            if (sem_wait(FIFO_VAL) == -1) {
                exit(1);
            }
            toCut = takeFirst(fifo);

            if (toCut != -1) {
                if (sem_post(FIFO_VAL) == -1) {
                    exit(1);
                }
                cut(toCut);
            } else {
                printTime();
                printf(" barber fall asleep\n");

                if (sem_wait(BARBER_VAL) == -1){
                    exit(1);
                }

                if (sem_post(FIFO_VAL) == -1){
                    exit(1);
                }
                break;
            }
        }
    }

    return 0;
}

pid_t inviteNew() {
    if (sem_wait(FIFO_VAL) == -1){
        exit(1);
    }

    pid_t toCut = fifo->chair;

    if (sem_post(FIFO_VAL) == -1){
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
    munmap(fifo, sizeof(fifo));
    shm_unlink(shmPath);

    sem_close(BARBER_VAL) ;
    sem_unlink(barberPath);

    sem_close(FIFO_VAL) ;
    sem_unlink(fifoPath);

    sem_close(WAKE_VAL) ;
    sem_unlink(wakePath);

    sem_close(BLOCK_SAME);
    sem_unlink(blockPath);

    printf("\nBarber cleaned his workplace and went home\n");
}