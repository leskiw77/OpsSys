#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "general.h"

#define RANGE 1001

int array[ARRAYSIZE];

pthread_t *readers;
pthread_t *writers;

int readersNumber;
int writersNumber;

int howManyToRead;
int howManyToWrite;

int currentReaders = 0;

bool infoMode = false;
bool wait = true;

__time_t timestamp() {
    struct timespec timestamp;
    clock_gettime(CLOCK_MONOTONIC, &timestamp);
    return timestamp.tv_nsec / 1000;
}

sem_t readerSemaphore;
sem_t writerSemaphore;

bool isNumber(char *string);
void *readerFunction(void *arg);
void *writerFunction(void *arg);
void createThreads();


int main(int argc, char **argv) {
    // parse args :
    if( (argc != 5 && argc != 6) || (argc == 6 && strcmp(argv[5], "-i") != 0)) {
        printf("Wrong arguments\n"
                       "Args : number of clients, number of writers, how many write, how many read, [-i]");
        return 1;
    }

    readersNumber = atoi(argv[1]);
    writersNumber = atoi(argv[2]);
    howManyToRead = atoi(argv[3]);
    howManyToWrite = atoi(argv[4]);

    // initialize shared array :
    for (int i = 0; i < ARRAYSIZE; i++) {
        array[i] = rand() % RANGE;
    }

    // create semaphores :
    if (sem_init(&readerSemaphore, 0, 1) == -1) {
        fprintf(stderr, "Cannot create reader semaphore\n");
        return 1;
    }
    if (sem_init(&writerSemaphore, 0, 1) == -1) {
        fprintf(stderr, "Cannot create reader semaphore\n");
        return 1;
    }

    createThreads();

    // clean :
    free(readers);
    free(writers);
    sem_destroy(&readerSemaphore);
    sem_destroy(&writerSemaphore);
    return 0;
}

void createThreads() {
    readers = malloc(sizeof(pthread_t) * readersNumber);
    writers = malloc(sizeof(pthread_t) * writersNumber);

    int *dividers = malloc(sizeof(int) * readersNumber);

    for (int i = 0; i < readersNumber; i++) {
        dividers[i] = rand() % (RANGE - 1) + 1;
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);


    for (int i = 0; i < readersNumber; i++) {
        if (pthread_create(&readers[i], NULL, readerFunction, dividers + i) != 0) {
            fprintf(stderr, "Cannot create thread %d\n", i);
            exit(1);
        }
    }
    wait = false;
    for (int i = 0; i < writersNumber; i++) {
        if (pthread_create(&writers[i], NULL, writerFunction, NULL) != 0) {
            fprintf(stderr, "Cannot create thread %d\n", i);
            exit(1);
        }
    }

    for (int i = 0; i < readersNumber; i++) {
        if (pthread_join(readers[i], NULL) != 0) {
            fprintf(stderr, "Cannot join thread %d\n", i);
            exit(1);
        }
    }
}

void * readerFunction(void * arg) {
    while (wait);
    int divider = *(int *) arg;
    for (int i = 0; i < howManyToRead; i++) {
        sem_wait(&readerSemaphore);

        if (++currentReaders == 1) {
            sem_wait(&writerSemaphore);
        }
        sem_post(&readerSemaphore);

        int counter = 0;
        for (int i = 0; i < ARRAYSIZE; i++) {
            if (array[i] % divider == 0) {
                counter++;
                if (infoMode) {
                    printf("%d: %lu reader: pozycja %d wartosc %d dzielnik %d\n",
                           timestamp(), pthread_self(), i, array[i], divider);
                }
            }
        }
        printf("%d: %lu reader: liczb %d\n", timestamp(), pthread_self(), counter);

        sem_wait(&readerSemaphore);
        if (--currentReaders == 0) {
            sem_post(&writerSemaphore);
        }
        sem_post(&readerSemaphore);
        sleep(1);
    }
}

void * writerFunction(void * arg) {
    while (wait);
    int howMany, index, value;
    for (int i = 0; i < howManyToWrite; i++) {
        //printf("%d: %lu writer chce zajac mutex\n", timestamp(), pthread_self());
        sem_wait(&writerSemaphore);

        howMany = rand() % ARRAYSIZE + 1;

        printf("%d: %lu writer zmienil tablice\n", timestamp(), pthread_self());
        for (int j = 0; j < howMany; j++) {
            index = rand() % ARRAYSIZE;
            value = rand() % RANGE;

            array[index] = value;
            if (infoMode) {
                printf("%d: %lu writer zmienil pozycje %d na wartosc %d\n", timestamp(), pthread_self(), index, value);
            }
        }

        sem_post(&writerSemaphore);
        sleep(1);
    }
}


bool isNumber(char *string) {
    int i = 0;
    while (string[i] != '\0') {
        if (string[i] >= '0' && string[i] <= '9')
            i++;
        else
            return false;
    }
    return true;
}