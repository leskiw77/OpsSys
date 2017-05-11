#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

//--------------------------------------------------
int numOfThreads;
char *fileName;
int numOfRecords;
char *word;
int file;
pthread_t *threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int BUFFERSIZE = 1024;
int STARTJOB = 1;

//--------------------------------------------------
void *threadFunction(void *);

//--------------------------------------------------
int main(int argc, char *argv[]) {
    fileName = calloc(20, sizeof(char));
    word = calloc(10, sizeof(char));

    if (argc != 5) {
        printf("Bledna ilosc argumentow!\n"
                       "Poprawny sposob wywolania programu to:\n"
                       "[nazwa programu] [ilosc watkow] [nazwa pliku] [ilosc rekordow] [szukane slowo]\n");
        exit(-1);
    }

    numOfThreads = atoi(argv[1]);
    fileName = argv[2];
    numOfRecords = atoi(argv[3]);
    word = argv[4];


    if ((file = open(fileName, O_RDONLY)) == -1) {
        printf("open(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    threads = calloc(numOfThreads, sizeof(pthread_t));
    int i;
    for (i = 0; i < numOfThreads; i++) {
        if (pthread_create(&threads[i], NULL, threadFunction, NULL) != 0) {
            printf("pthread_create(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
    }
    STARTJOB = 0;
    for (i = 0; i < numOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    close(file);

    return 0;
}

//--------------------------------------------------
void *threadFunction(void *unused) {
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
    int i;
    char **readRecords = calloc(numOfRecords, sizeof(char *));
    char *recID = calloc(2, sizeof(char));
    int numOfReadChars;
    int work = 1;

    for (i = 0; i < numOfRecords; i++) {
        readRecords[i] = calloc(1024, sizeof(char));
    }

    while (STARTJOB);

    while (work) {
        pthread_mutex_lock(&mutex);

        for (i = 0; i < numOfRecords; i++) {
            if ((numOfReadChars = read(file, readRecords[i], BUFFERSIZE)) == -1) {
                printf("read(): %d: %s\n", errno, strerror(errno));
                exit(-1);
            }
        }

        pthread_mutex_unlock(&mutex);
        usleep(1);
        pthread_testcancel();

        for (i = 0; i < numOfRecords; i++) {
            if (strstr(readRecords[i], word) != NULL) {
                strncpy(recID, readRecords[i], 2);
                printf("%ld: znalazl szukane slowo w rekordzie o id = %d\n", pthread_self(), atoi(recID));

                int j;
                for (j = 0; j < numOfThreads; j++) {
                    if (threads[j] != pthread_self()) {
                        pthread_cancel(threads[j]);
                    }
                }
                work = 0;
                i = numOfRecords;
            }
        }
    }

    return NULL;
}