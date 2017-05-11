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
pthread_t *threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int BUFFERSIZE = 1024;
int STARTJOB = 1;
pthread_t someThread;
pid_t tid;

//--------------------------------------------------
void *threadFunction(void *);

void handler(int);

//--------------------------------------------------
int main(int argc, char *argv[]) {
    numOfThreads = 5;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGFPE);

    struct sigaction sa;
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    sigaction(SIGFPE, &sa, NULL);

    threads = calloc(numOfThreads, sizeof(pthread_t));
    int i;
    for (i = 0; i < numOfThreads; i++) {
        if (pthread_create(&threads[i], NULL, threadFunction, NULL) != 0) {
            printf("pthread_create(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
    }
    someThread = threads[1];

    STARTJOB = 0;
    for (i = 0; i < numOfThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    free(threads);

    return 0;
}

//--------------------------------------------------
void *threadFunction(void *unused) {
    tid = pthread_self();

    while (STARTJOB);
    if (pthread_self() == someThread) {
        int x = 1, y = 0;
        x /= y;
    }

    return NULL;
}

void handler(int signal_no) {
    printf("Signal: %d, TID: %ld\n", signal_no, (long) tid);
    exit(0);
}