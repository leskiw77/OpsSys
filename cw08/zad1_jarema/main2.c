#include "general.h"

int threadsNum;
char *fileName;
int recordsNum;
char *searched;
int file;
pthread_t *threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


void *threadFunction(void *);


int main(int argc, char *argv[]) {

    fileName = (char *)malloc(MAXFILENAME * sizeof(char));
    searched = (char *) malloc(MAXSEARCH * sizeof(char));

    if (argc != 5) {
        printf("Wrong arguments! Threads number, file name, number of records and searched word required\n");
        return 1;
    }

    threadsNum = atoi(argv[1]);
    fileName = argv[2];
    recordsNum = atoi(argv[3]);
    searched = argv[4];

    // open file :
    if ((file = open(fileName, O_RDONLY)) == -1) {
        printf("Error while open a file\n");
        return 2;
    }

    threads = (pthread_t *)malloc(threadsNum * sizeof(pthread_t));

    for (int i = 0; i < threadsNum; i++) {
        if (pthread_create(&threads[i], NULL, threadFunction, NULL) != 0) {
            printf("pthread_create(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
    }

    // execute and wait for threads :
    for (int i = 0; i < threadsNum; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    close(file);
    return 0;
}


void * threadFunction(void *unused) {
    // setting deferred cancel type :
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    sleep(1);

    char **readRecords = calloc(recordsNum, sizeof(char *));
    for (int i = 0; i < recordsNum; i++) {
        readRecords[i] = calloc(1024, sizeof(char));
    }
    char *num = (char *) malloc(MAXIDDIGITS * sizeof(char));


    pthread_mutex_lock(&mutex);
    for (int i = 0; i < recordsNum; i++) {
        if (read(file, readRecords[i], BUFFERSIZE) == -1) {
            printf("read(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
    }
    pthread_mutex_unlock(&mutex);

    usleep(1);
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_testcancel();
    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);

    for (int i = 0; i < recordsNum; i++) {
        if (strstr(readRecords[i], searched) != NULL) {
            strncpy(num, readRecords[i], MAXIDDIGITS);
            printf("Thread with TID=%ld: found word in record number %d\n", pthread_self(), atoi(num));

            // the word has been found, so cancel others threads :
            for (int j = 0; j < threadsNum; j++) {
                if (threads[j] != pthread_self()) {
                    pthread_cancel(threads[j]);
                }
            }
            break;
        }
    }

    return NULL;
}