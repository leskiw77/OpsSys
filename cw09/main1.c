#include "general.h"

void *writerFunction(void *);

void *readerFunction(void *);

int *array;
int readersNumber = 0, writersNumber = 0;
int readersWaiting = 0;
bool infoMode = false, wait = true;
sem_t *readers, *writers, *resource;

int main(int argc, char *argv[]) {

    srand(time(NULL));

    if ((argc != 3 && argc != 4) || (argc == 4 && strcmp(argv[3], "-i") != 0)) {
        printf("Wrong arguments\n"
                       "Args : number of clients, number of writers, how many write, how many read, [-i]");
        return 1;
    }

    int arrayId;

    //pthread_t * threadIds;
    pthread_t *writerThreads;
    pthread_t *readerThreads;

    readersNumber = atoi(argv[1]);
    writersNumber = atoi(argv[2]);
    infoMode = (argc == 4);

    printf("%d", infoMode);

    //creating semaphores
    readers = sem_open(READERS_SEM, O_CREAT | O_RDWR, 0666, 1);
    writers = sem_open(WRITERS_SEM, O_CREAT | O_RDWR, 0666, 1);
    resource = sem_open(RESOURCES_SEM, O_CREAT | O_RDWR, 0666, 1);
    if (readers == SEM_FAILED || writers == SEM_FAILED || resource == SEM_FAILED) {
        printf("Semaphore error\n");
        exit(1);
    }


    //creating shared array
    arrayId = shm_open(SHARED_MEM, O_CREAT | O_RDWR, 0666);
    if (arrayId == -1) {
        printf("Error while creating shared array\n");
        exit(1);
    }

    if (ftruncate(arrayId, ARRAYSIZE * sizeof(int)) == -1) {
        printf("Error while truncating array\n");
        exit(1);
    }
    array = mmap(NULL, ARRAYSIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, arrayId, 0);

    if (array == (void *) -1) {
        printf("Error while mapping shared arrayy\n");
        exit(1);
    }

    for (int i = 0; i < ARRAYSIZE; i++) {
        array[i] = rand() % MAXNUMBER;
    }

    //threadIds = (pthread_t *) calloc(writersNumber + readersNumber, sizeof(pthread_t));
    writerThreads = (pthread_t *) calloc(writersNumber, sizeof(pthread_t));
    readerThreads = (pthread_t *) calloc(readersNumber, sizeof(pthread_t));

    for (int i = 0; i < writersNumber; i++) {
        if (pthread_create(&writerThreads[i], NULL, writerFunction, NULL) != 0) {
            printf("Couldn't create thread\n");
            exit(1);
        }
    }

    for (int i = 0; i < readersNumber; i++) {
        int divisor = rand() % MAXDEVIDER + 1;

        if (pthread_create(&readerThreads[i], NULL, readerFunction, (void *) &divisor) != 0) {
            printf("Couldn't create thread\n");
            exit(1);
        }
    }

    wait = false;

    // join threads :
    for (int i = 0; i < writersNumber; i++) {
        pthread_join(writerThreads[i], NULL);
    }

    for (int i = 0; i < readersNumber; i++) {
        pthread_join(readerThreads[i], NULL);
    }

    // free resources :
    munmap(array, ARRAYSIZE * sizeof(int));
    shm_unlink(SHARED_MEM);

    sem_close(resource);
    sem_unlink(RESOURCES_SEM);

    sem_close(readers);
    sem_unlink(READERS_SEM);

    sem_close(writers);
    sem_unlink(WRITERS_SEM);

    free(writerThreads);
    free(readerThreads);

    return 0;
}


void *writerFunction(void *args) {
    while (wait);
    //take semaphore
    if (sem_wait(resource) == -1) {
        printf("Error while taking semaphore\n");
        exit(1);
    }

    printf("%lu WRITER : start to modify array\n", pthread_self());

    int toModify = rand() % ARRAYSIZE;
    for (int j = 0; j < toModify; j++) {
        int index = rand() % ARRAYSIZE;
        int value = rand() % MAXNUMBER;
        if (infoMode) {
            printf("%lu WRITER : changed array[%d] => %d\n", pthread_self(), index, value);
        }
        array[index] = value;
    }

    //give semaphore
    if (sem_post(resource) == -1) {
        printf("Error while giving semaphore\n");
        exit(1);
    }
    return NULL;
}

void *readerFunction(void *args) {
    int divider = *((int *) args);
    while (wait);

    if (sem_wait(readers) == -1) {
        printf("Error while taking semaphore\n");
        exit(1);
    }
    readersWaiting++;

    if (readersWaiting == 1) {
        if (sem_wait(resource) == -1) {
            printf("Error while taking semaphore\n");
            exit(1);
        }
    }

    if (sem_post(readers) == -1) {
        printf("Error while giving semaphore\n");
        exit(1);
    }

    int counter = 0;
    for (int i = 0; i < ARRAYSIZE; i++) {
        if (array[i] % divider == 0) {
            counter++;
            if (infoMode) {
                printf("%lu READER : array[%d] = %d , divider = %d\n", pthread_self(), i, array[i], divider);
            }
        }
    }
    printf("%lu READER : %d elements are dividable with %d\n", pthread_self(), counter, divider);

    if (sem_wait(readers) == -1) {
        printf("Error while taking semaphore\n");
        exit(1);
    }

    readersWaiting--;
    if (readersWaiting == 0) {
        if (sem_post(resource) == -1) {
            printf("Error while giving semaphore\n");
            exit(1);
        }
    }

    if (sem_post(readers) == -1) {
        printf("Error while giving semaphore\n");
        exit(1);

        return NULL;
    }
}

