#include "general.h"

int threadsNum;
char *fileName;
int recordsNum;
char *searched;
int file;
int signr;
pthread_t *threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void *threadFunction(void *);


void signalHandler(int signal) {
    printf("PID: %d TID: %ld received signal no %d\n", getpid(), pthread_self(), signal);
    return;
}

int main(int argc, char *argv[]) {

    fileName = (char *)malloc(MAXFILENAME * sizeof(char));
    searched = (char *) malloc(MAXSEARCH * sizeof(char));

    if (argc != 6) {
        printf("Wrong arguments! Threads number, file name, number of records, searched word required and signal number\n");
        return 1;
    }

    threadsNum = atoi(argv[1]);
    fileName = argv[2];
    recordsNum = atoi(argv[3]);
    searched = argv[4];
    signr = atoi(argv[5]);

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


    pthread_kill(threads[0], signr);


    // execute and wait for threads :
    for (int i = 0; i < threadsNum; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);
    close(file);
    return 0;
}


void * threadFunction(void *unused) {

    signal(SIGUSR1, signalHandler);
    signal(SIGTERM, signalHandler);

    // setting asynchronous cancel type :
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

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

    // search word in buffers :
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

