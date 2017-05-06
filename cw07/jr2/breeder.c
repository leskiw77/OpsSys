#include "Fifo.h"
#include "general.h"


void printTime() {
    struct timespec time;
    if (clock_gettime(CLOCK_MONOTONIC, &time) == -1){
        exit(1);
    }
    printf("Time %ld :", (long)(time.tv_nsec / 1000));
}

void intHandler(int signo){
    exit(2);
}

void prepareFifo();
void prepareSemafors();
void prepareFullMask();
void freeResources(void);
int enterBarber();
void getCut(int ctsNum);

Fifo* fifo = NULL;

sem_t* BARBER;
sem_t* FIFO;
sem_t* CHECKER;
sem_t* SLOWER;

int counter = 0;
sigset_t fullMask;

void rtminHandler(int signo) {
    counter++;
}

int main(int argc, char** argv){
    signal(SIGRTMIN, rtminHandler);
    if (argc != 3) {
        printf("Need 2 arguments: number of clients and cuts\n");
        exit(1);
    }
    int clientsAmount = atoi(argv[1]);
    int serviceAmount = atoi(argv[2]);

    int shmID = shm_open(shmPath, O_RDWR, 0666);
    if(shmID == -1){
        exit(1);
    }

    void* tmp = mmap(NULL, sizeof(Fifo), PROT_READ | PROT_WRITE, MAP_SHARED, shmID, 0);
    fifo = (Fifo*) tmp;

    BARBER = sem_open(barberPath, O_RDWR);
    if(BARBER == SEM_FAILED){
        exit(1);
    }

    FIFO = sem_open(fifoPath, O_RDWR);
    if(FIFO == SEM_FAILED){
        exit(1);
    }

    CHECKER = sem_open(checkerPath, O_RDWR);
    if(CHECKER == SEM_FAILED) {
        exit(1);
    }

    SLOWER = sem_open(slowerPath, O_RDWR);
    if(SLOWER == SEM_FAILED){
        exit(1);
    }

    prepareFullMask();

    sigset_t mask;
    if(sigemptyset(&mask) == -1) {
        exit(1);
    }
    if(sigaddset(&mask, SIGRTMIN) == -1) {
        exit(1);
    }
    if(sigprocmask(SIG_BLOCK, &mask, NULL) == -1){
        exit(1);
    }

    for(int i=0; i<clientsAmount; i++){
        pid_t id = fork();
        if(id == 0){
            getCut(serviceAmount);
            return 0;
        }
    }

    while(1){
        wait(NULL);
        if (errno == ECHILD) break;
    }
    printf("All clients serviced!\n");
    return 0;
}

void getCut(int serviceAmount){
    while(counter < serviceAmount){
        if(sem_wait(CHECKER) == -1) {
            exit(1);
        }

        if(sem_wait(FIFO) == -1) {
            exit(1);
        }

        int res = enterBarber();

        if(sem_post(FIFO) == -1) {
            exit(1);
        }

        if(sem_post(CHECKER) == -1) {
            exit(1);
        }

        if(res != -1){
            sigsuspend(&fullMask);
            printTime();
            printf(" client %d was serviced!\n", getpid());
        }
    }
}

int enterBarber(){
    int barberStat;
    if(sem_getvalue(BARBER, &barberStat) == -1) {
        exit(1);
    }

    pid_t myPID = getpid();

    if(barberStat == 0){
        if(sem_post(BARBER) == -1) {
            exit(1);
        }
        printTime();
        printf(" client %d is awakening barber!\n", getpid());
        if(sem_wait(SLOWER) == -1){
            exit(1);
        }

        fifo->chair = myPID;

        return 1;
    }else{
        int res =  addLast(fifo, myPID);
        if(res == -1){
            printTime();
            printf(" client %d has no place inside\n", getpid());
            return -1;
        }else{
            printTime();
            printf(" client %d is in the queue now\n", getpid());
            return 0;
        }
    }
}

void prepareFullMask(){
    if(sigfillset(&fullMask) == -1){
        exit(1);
    }
    if(sigdelset(&fullMask, SIGRTMIN) == -1) {
        exit(1);
    }
    if(sigdelset(&fullMask, SIGINT) == -1) {
        exit(1);
    }
}

void freeResources(void){
    munmap(fifo, sizeof(fifo));

    sem_close(BARBER);
    sem_close(FIFO);
    sem_close(CHECKER);
    sem_close(SLOWER);
}