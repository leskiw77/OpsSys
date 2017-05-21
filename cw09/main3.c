#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include "general.h"

#define RANGE 1001

int * numberArray;
int arraySize;

pthread_t * readers;
pthread_t * writers;

int readersSize;
int writersSize;

int readsNumber;
int writesNumber;

int currentReaders = 0;

bool mode = false;
bool wait = true;

__time_t timestamp() {
    struct timespec timestamp;
    clock_gettime(CLOCK_MONOTONIC, &timestamp);
    return timestamp.tv_nsec/1000;
}

sem_t readerSemaphore;
sem_t writerSemaphore;

bool isNumber(char *string);
void* readerFunction(void* arg);
void* writerFunction(void* arg);

void initializeSharedArray(int size);
void createThreads();
void cleanAll();

bool parseArguments(int argc, char **argv) {
    // ile klientow, ile pisarzy, ile czytaja, ile pisza, ile ma tablica rozmiaru, tryb zapisu
    if ((argc != 6) && (argc != 7)) return false;
    for (int i = 1; i < 6; i++){
        if (!isNumber(argv[i]))
            return false;
    }
    if (argv[6] != NULL && strcmp(argv[6], "-i") != 0) return false;

    return true;

}


int main(int argc, char ** argv) {
    // parse args :
    //if (!parseArguments(argc, argv)){
    //    fprintf(stderr, "Invalid arguments\n");
    //    exit(1);
    //}


    initializeSharedArray(ARRAYSIZE);

    readersSize = atoi(argv[1]);
    writersSize = atoi(argv[2]);

    readsNumber = atoi(argv[3]);
    writesNumber = atoi(argv[4]);

    if (argv[6] != NULL)
        mode = true;

    if (sem_init(&readerSemaphore, 0, 1) == -1){
        fprintf(stderr, "Cannot create reader semaphore\n");
        exit(1);
    }
    if (sem_init(&writerSemaphore, 0, 1) == -1){
        fprintf(stderr, "Cannot create reader semaphore\n");
        exit(1);
    }
    createThreads();
    cleanAll();
    return 0;
}

void cleanAll() {
    free(numberArray);
    free(readers);
    free(writers);

    sem_destroy(&readerSemaphore);
    sem_destroy(&writerSemaphore);
}

void createThreads() {
    readers = malloc(sizeof(pthread_t) * readersSize);
    writers = malloc(sizeof(pthread_t) * writersSize);

    int* dividers = malloc(sizeof(int) * readersSize);

    for (int i = 0; i < readersSize; i++){
        dividers[i] = rand() % (RANGE - 1)  + 1;
    }

    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);


    for(int i = 0; i < readersSize; i++){
        if (pthread_create(&readers[i], NULL, readerFunction, dividers + i) != 0){
            fprintf(stderr, "Cannot create thread %d\n", i);
            exit(1);
        }
    }
    wait = false;
    for(int i = 0; i < writersSize; i++){
        if (pthread_create(&writers[i], NULL, writerFunction, NULL) != 0){
            fprintf(stderr, "Cannot create thread %d\n", i);
            exit(1);
        }
    }

    for (int i = 0 ; i < readersSize; i++){
        if (pthread_join(readers[i], NULL) != 0){
            fprintf(stderr, "Cannot join thread %d\n", i);
            exit(1);
        }
    }
}
void* readerFunction(void* arg){
    while(wait);
    int divider = *(int*) arg;
    for (int i = 0; i < readsNumber; i++){
        sem_wait(&readerSemaphore);

        if(++currentReaders == 1){
            sem_wait(&writerSemaphore);
        }
        sem_post(&readerSemaphore);

        int counter = 0;
        for (int i = 0; i < arraySize; i++){
            if (numberArray[i] % divider == 0){
                counter++;
                if (mode){
                    printf("%d: %lu reader: pozycja %d wartosc %d dzielnik %d\n",
                           timestamp(), pthread_self(), i, numberArray[i], divider);
                }
            }
        }
        printf("%d: %lu reader: liczb %d\n", timestamp(), pthread_self(), counter);

        sem_wait(&readerSemaphore);
        if (--currentReaders == 0){
            sem_post(&writerSemaphore);
        }
        sem_post(&readerSemaphore);
        sleep(1);

    }
}
void* writerFunction(void* arg){
    while(wait);
    int howMany, index, value;
    for (int i = 0; i < writesNumber; i++){
        //printf("%d: %lu writer chce zajac mutex\n", timestamp(), pthread_self());
        sem_wait(&writerSemaphore);

        howMany = rand() % arraySize + 1;

        printf("%d: %lu writer zmienil tablice\n", timestamp(), pthread_self());
        for (int j = 0; j < howMany; j++){
            index = rand() % arraySize;
            value = rand() % RANGE;

            numberArray[index] = value;
            if (mode){
                printf("%d: %lu writer zmienil pozycje %d na wartosc %d\n", timestamp(), pthread_self(), index, value);
            }
        }

        sem_post(&writerSemaphore);
        sleep(1);
    }
}
void initializeSharedArray(int size) {
    arraySize = size;
    numberArray = malloc(arraySize * sizeof(int));

    for (int i = 0; i < arraySize; i++){
        numberArray[i] = rand() % RANGE;
    }
    for (int i = 0; i < arraySize; i++){
        printf("%d ", numberArray[i]);
    }
    printf("\n");
}

bool isNumber(char *string) {
    int i = 0;
    while(string[i] != '\0'){
        if (string[i] >= '0' && string[i] <= '9') i++;
        else return false;
    }
    return true;
}