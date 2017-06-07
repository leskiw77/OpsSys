#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <zconf.h>
#include <signal.h>
#include <stdbool.h>

#define LAND 0
#define START 1

typedef struct Plane {
    int id;
    volatile bool alive;
} Plane;

pthread_mutex_t mtx;
pthread_cond_t starting;
pthread_cond_t landing;

int numberOfPlanes;
int n, k;

volatile int plainsWaitingToStart = 0;
volatile int plainsWaitingToLand = 0;
volatile bool isFree = true;
volatile int onBoard = 0;

pthread_t * threads;
struct Plane * planes;

void freeAll();
void signalHandler(int sig);
void waitForPermission(struct Plane *p, int tmp);
void freeAir();
void land(struct Plane *p);
void start(struct Plane *p);
void *threadFunction(void *arg);


int main(int argc, char const *argv[]) {
    if (argc != 4) {
        printf("Wrong arguments\n"
                       "args: [number of planes] [N] [K]\n");
        exit(1);
    }

    srand(time(NULL));

    // parse args and set atexit :
    signal(SIGINT, signalHandler);
    atexit(freeAll);
    numberOfPlanes = atoi(argv[1]);
    n = atoi(argv[2]);
    k = atoi(argv[3]);
    
    threads = (pthread_t *)malloc(numberOfPlanes * sizeof(pthread_t));
    planes = (struct Plane *)malloc(numberOfPlanes * sizeof(struct Plane));

    // init mutexes and cond :
    pthread_mutex_init(&mtx, NULL);
    pthread_cond_init(&starting, NULL);
    pthread_cond_init(&landing, NULL);

    // set signals mask :
    sigset_t mask;
    sigset_t old;
    sigfillset(&mask);
    pthread_sigmask(SIG_SETMASK, &mask, &old);

    // init planes :
    for (int i = 0; i < numberOfPlanes; i++) {
        planes[i].id = i;
        planes[i].alive = true;
        if (pthread_create(&threads[i], NULL, threadFunction, &planes[i]) != 0) {
            exit(1);
        }
    }

    pthread_sigmask(SIG_SETMASK, &old, NULL);

    while (true) {
        pause();
    }
}

void freeAll() {
    for (int i = 0; i < numberOfPlanes; i++) {
        planes[i].alive = false;
    }
    for (int i = 0; i < numberOfPlanes; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_destroy(&mtx);
    pthread_cond_destroy(&starting);
    pthread_cond_destroy(&landing);
    free(threads);
    free(planes);
}

void signalHandler(int sig) {
    exit(0);
}

void waitForPermission(struct Plane *p, int decision) {
    pthread_mutex_lock(&mtx);
    if (decision == START) {
        // start :
        plainsWaitingToStart++;
        printf("Plane[%d] wait for permission to start\n\n", p->id);
        while (!isFree) {
            pthread_cond_wait(&starting, &mtx);
        }
        isFree = false;
        plainsWaitingToStart--;
    } else if (decision == LAND) {
        // land :
        plainsWaitingToLand++;
        printf("Plane[%d] wait for permission to land\n\n", p->id);
        while (!isFree || onBoard == n) {
            pthread_cond_wait(&landing, &mtx);
        }
        isFree = false;
        plainsWaitingToLand--;
    }
    pthread_mutex_unlock(&mtx);
    return;
}

void freeAir() {
    if (onBoard < k) {
        if (plainsWaitingToLand > 0) {
            pthread_cond_signal(&landing);
        } else {
            pthread_cond_signal(&starting);
        }
    } else {
        if (plainsWaitingToStart > 0) {
            pthread_cond_signal(&starting);
        } else if (onBoard < n) {
            pthread_cond_signal(&landing);
        }
    }
}

void land(struct Plane *p) {
    pthread_mutex_lock(&mtx);
    onBoard++;
    isFree = true;
    freeAir();
    printf("Plane[%d] landed ;    planes : on board = %d , want to start =  %d , want to land = %d\n\n",
           p->id, onBoard, plainsWaitingToStart, plainsWaitingToLand);
    pthread_mutex_unlock(&mtx);
}

void start(struct Plane *p) {
    pthread_mutex_lock(&mtx);
    onBoard--;
    isFree = true;
    freeAir();
    printf("Plane[%d] started ;    planes : on board = %d , want to start =  %d , want to land = %d\n\n",
           p->id, onBoard, plainsWaitingToStart, plainsWaitingToLand);
    pthread_mutex_unlock(&mtx);
}

void *threadFunction(void *arg) {
    struct Plane *p = arg;
    while (p->alive) {
        waitForPermission(p, LAND);
        land(p);
        sleep(1);
        waitForPermission(p, START);
        start(p);
        sleep(1);
    }
    return NULL;
}
