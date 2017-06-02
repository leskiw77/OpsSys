#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

pthread_t *philosophers;
pthread_mutex_t *mutexes;
bool isEating[5];
bool isForkTaken[5];
bool START_JOB;

void *threadFunction(void *);

void getFork(int);

void leaveFork(int);

void atexitFunction(void);

void printOutput(void);

int main(int argc, char const *argv[]) {
    srand(time(NULL));
    atexit(atexitFunction);
    int IDs[5];
    START_JOB = true;

    philosophers = calloc(5, sizeof(pthread_t));
    for (int i = 0; i < 5; i++) {
        IDs[i] = i;
        if (pthread_create(&philosophers[i], NULL, threadFunction, IDs + i) != 0) {
            printf("pthread_create(): %d: %s\n", errno, strerror(errno));
            exit(1);
        }
    }

    mutexes = calloc(5, sizeof(pthread_mutex_t));

    for (int i = 0; i < 5; i++) {
        if (pthread_mutex_init(&mutexes[i], NULL) != 0) {
            printf("pthread_mutex_init(): %d: %s\n", errno, strerror(errno));
            exit(1);
        }
    }

    START_JOB = false;

    for (int i = 0; i < 5; i++) {
        pthread_join(philosophers[i], NULL);
    }

    free(philosophers);
    return 0;
}


void *threadFunction(void *arg) {
    while (START_JOB);
    int philosopherID = *((int *) arg);
    int leftFork = philosopherID;
    int rightFork = (philosopherID + 1) % 5;
    while (true) {
        usleep(rand() % 1500000 + 1000000);
        // wait for forks :
        if (philosopherID % 2) {
            getFork(rightFork);
            getFork(leftFork);
        } else {
            getFork(leftFork);
            getFork(rightFork);
        }
        isEating[philosopherID] = true;

        // print output :
        printOutput();

        usleep(100000);

        isEating[philosopherID] = false;
        leaveFork(leftFork);
        leaveFork(rightFork);
    }
    return NULL;
}

void printOutput() {
    for (int i = 0; i < 5; i++) {
        if (isForkTaken[i]) {
            printf("! ");
        } else {
            printf("  ");
        }

        if (isEating[i]) {
            printf("O ");
        } else {
            printf("  ");
        }
    }
    printf("\n");
    for (int i = 0; i < 5; i++) {
        if (!isForkTaken[i]) {
            printf("! ");
        } else {
            printf("  ");
        }

        if (!isEating[i]) {
            printf("O ");
        } else {
            printf("  ");
        }
    }
    printf("\n\n----------------------------\n\n");
}

void getFork(int forkId) {
    pthread_mutex_lock(&mutexes[forkId]);
    isForkTaken[forkId] = true;
}

void leaveFork(int forkId) {
    isForkTaken[forkId] = false;
    pthread_mutex_unlock(&mutexes[forkId]);
}

void atexitFunction() {
    for (int i = 0; i < 5; i++) {
        pthread_cancel(philosophers[i]);
    }
}
