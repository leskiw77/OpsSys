#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

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
int test, signr;

//--------------------------------------------------
void *threadFunction(void *);

void signalHandler(int);

//--------------------------------------------------
int main(int argc, char *argv[]) {
    fileName = calloc(20, sizeof(char));
    word = calloc(10, sizeof(char));

    if (argc != 3) {
        printf("\nBledna ilosc argumentow!\n"
                       "Poprawny sposob wywolania programu to:\n"
                       "[nazwa programu] [numer testu] [numer signru]\n\n"
                       "Available signals:\t10 - SIGUSR1\t15 - SIGTERM\t 9 - SIGKILL\t19 - SIGSTOP\n"
                       "Tests: 1 - 5 wg. tresci zadania\n\n");
        exit(-1);
    }

    numOfThreads = 5;
    strcpy(fileName, "somefile");
    numOfRecords = 2;
    strcpy(word, "slowo");
    test = atoi(argv[1]);
    signr = atoi(argv[2]);

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

    if (test == 1) {
        kill(getpid(), signr);
    } else if (test == 2) {
        signal(signr, SIG_IGN);
        kill(getpid(), signr);
    } else if (test == 4) {
        if (pthread_kill(threads[0], signr) != 0) {
            printf("pthread_kill(): %d: %s\n", errno, strerror(errno));
            exit(1);
        }
    } else if (test == 3) {
        if (signal(SIGUSR1, signalHandler) == SIG_ERR) {
            printf("signal(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
        if (signal(SIGTERM, signalHandler) == SIG_ERR) {
            printf("signal(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
        kill(getpid(), signr);
    } else if (test == 5) {
        if (pthread_kill(threads[0], signr) != 0) {
            printf("pthread_kill(): %d: %s\n", errno, strerror(errno));
            exit(1);
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
    int i;
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    char **readRecords = calloc(numOfRecords, sizeof(char *));
    char *recID = calloc(2, sizeof(char));
    int numOfReadChars;
    int work = 1;

    for (i = 0; i < numOfRecords; i++) {
        readRecords[i] = calloc(1024, sizeof(char));
    }

    if (test == 4) {
        signal(signr, SIG_IGN);
    } else if (test == 3 || test == 5) {
        if (signal(SIGUSR1, signalHandler) == SIG_ERR) {
            printf("signal(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
        if (signal(SIGTERM, signalHandler) == SIG_ERR) {
            printf("signal(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
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

//-----------------------------------------------------
void signalHandler(int signal) {
    if (signal == SIGUSR1) {
        printf("Otrzymano SIGUSR1\n");
    } else if (signal == SIGTERM) {
        printf("Otrzymano SIGTERM\n");
    }
    printf("PID: %d TID: %ld\n", getpid(), pthread_self());
    return;
}