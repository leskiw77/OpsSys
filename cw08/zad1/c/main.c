#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

int numOfThreads;
char* fileName;
int numOfRecords;
char* searchedWord;
int file;
pthread_t *threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int BUFFERSIZE = 1024;
int STARTJOB = 1;
void* threadFunction(void*);


int main(int argc, char* argv[]) {

    fileName = calloc(20, sizeof(char));
    searchedWord = calloc(10, sizeof(char));

    if (argc != 5) {
        printf("Wrong arguments!\n"
                       "[prog_name] [num of threads] [filename] [num of records] [word]\n");
        exit(-1);
    }

    numOfThreads = atoi(argv[1]);
    fileName = argv[2];
    numOfRecords = atoi(argv[3]);
    searchedWord = argv[4];

    // open file :
    if((file = open(fileName, O_RDONLY)) == -1) {
        printf("open(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    // create threads :
    threads = calloc(numOfThreads, sizeof(pthread_t));
    for(int i = 0; i < numOfThreads; i++){
        if(pthread_create(&threads[i], NULL, threadFunction, NULL) != 0) {
            printf("pthread_create(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
    }

    STARTJOB = 0;
    // execute and wait for threads :
    for(int i = 0; i < numOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    close(file);
    return 0;
}


// Wszystkie wątki powinny być  odłączone a warunkiem zakończenia wątku jest
// odczytanie wszystkich rekordów pliku
void * threadFunction(void *unused) {
    // setting cancel type :
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    char** readRecords = calloc(numOfRecords, sizeof(char*));
    for(int i = 0; i < numOfRecords; i++) {
        readRecords[i] = calloc(1024, sizeof(char));
    }
    char* recID = calloc(2, sizeof(char));
    int numOfReadChars;
    int work = 1;

    // ?
    while(STARTJOB);

    while(work) {
        // lock mutex and read data to buffers :
        pthread_mutex_lock(&mutex);
        for(int i = 0; i < numOfRecords; i++){
            if((numOfReadChars = read(file, readRecords[i], BUFFERSIZE)) == -1) {
                printf("read(): %d: %s\n", errno, strerror(errno));
                exit(-1);
            }
        }

        // all records loaded to buffers so cancel others threads :
        if(numOfReadChars == 0) {
            for(int j = 0; j < numOfThreads; j++) {
                if(threads[j] != pthread_self()) {
                    pthread_cancel(threads[j]);
                }
            }
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);

        // find searched word in records :
        for(int i = 0; i < numOfRecords; i++) {
            if(strstr(readRecords[i], searchedWord) != NULL) {
                strncpy(recID, readRecords[i], 2);
                printf("%ld: found word in record id = %d\n", pthread_self(), atoi(recID));
            }
        }
    }
    return NULL;
}