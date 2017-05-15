#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

int threadsNum = 10;
pthread_t *threads;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int START_FLAG = 1;
pthread_t aThread;
pid_t tid;

void *threadFunction(void *);

void handler(int);

int main(int argc, char *argv[]) {
    sigset_t s;
    sigemptyset(&s);
    //sigaddset(&s, SIGFPE);

    struct sigaction sigAct;
    sigAct.sa_flags = 0;
    sigAct.sa_handler = handler;
    //sigaction(SIGFPE, &sigAct, NULL);

    threads = calloc(threadsNum, sizeof(pthread_t));
    int i;
    for (i = 0; i < threadsNum; i++) {
        if (pthread_create(&threads[i], NULL, threadFunction, NULL) != 0) {
            printf("pthread_create(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
    }
    aThread = threads[5];
    START_FLAG = 0;
    for (i = 0; i < threadsNum; i++) {
        pthread_join(threads[i], NULL);
    }
    free(threads);
    return 0;
}

void * threadFunction(void *unused) {
    tid = pthread_self();
    while (START_FLAG);
    int x = 1;
    if (pthread_self() == aThread) {
        x /= 0;
    }
    return NULL;
}

void handler(int signal_no) {
    printf("Thread with TID=%ld: signal %d\n", (long) tid, signal_no);
    exit(0);
}