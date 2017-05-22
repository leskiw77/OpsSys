#include "general.h"


void *writerJob(void *);

void *readerJob(void *);


int readersNumber = 0, writersNumber = 0;
int readersWaiting = 0;
int howManyToRead = 0, howManyToWrite = 0;
bool verbose = false, wait = true;
sem_t *readers, *writers, *resource;
int *memory;

int main(int argc, char *argv[]) {
    if ((argc != 5 && argc != 6) || (argc == 6 && strcmp(argv[5], "-i") != 0)) {
        printf("Wrong arguments\n"
                       "Args : number of clients, number of writers, how many write, how many read, [-i]");
        return 1;
    }

    int memoryId;
    pthread_t *threadIds;

    readersNumber = atoi(argv[1]);
    writersNumber = atoi(argv[2]);
    howManyToRead = atoi(argv[3]);
    howManyToWrite = atoi(argv[4]);
    verbose = (argc == 6);

    printf("%d",verbose);

    //creating semaphores
    readers = sem_open(READERS_SEM, O_CREAT | O_RDWR, 0666, 1);
    writers = sem_open(WRITERS_SEM, O_CREAT | O_RDWR, 0666, 1);
    resource = sem_open(RESOURCES_SEM, O_CREAT | O_RDWR, 0666, 1);
    if(readers == SEM_FAILED || writers == SEM_FAILED || resource == SEM_FAILED){
        printf("Semaphor error\n");
        exit(1);
    }



    //creating shared memory
    memoryId = shm_open(SHARED_MEM, O_CREAT | O_RDWR, 0666);
    if(memoryId == -1){
        printf("Error while creating shared memory\n");
        exit(1);
    }

    if(ftruncate(memoryId, ARRAYSIZE * sizeof(int)) == -1){
        printf("Error while truncating memory\n");
        exit(1);
    }
    memory = mmap(NULL, ARRAYSIZE * sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, memoryId, 0);
    if(memory == (void *) -1){
        printf("Error while mapping shared memory\n");
        exit(1);
    }


    for (int i = 0; i < ARRAYSIZE; i++) {
        memory[i] = rand() % MAXNUMBER;
    }



    threadIds = (pthread_t *) calloc(writersNumber + readersNumber, sizeof(pthread_t));



    for (int i = 0; i < writersNumber; i++) {
        if(pthread_create(&threadIds[i], NULL, writerJob, NULL) != 0){
            printf("Couldn't create thread\n");
            exit(1);
        }
    }

    for (int i = writersNumber; i < writersNumber + readersNumber; i++) {
        int divisor = rand() % MAXDEVIDER + 1;

        if(pthread_create(&threadIds[i], NULL, readerJob, (void *) &divisor) != 0){
            printf("Couldn't create thread\n");
            exit(1);
        }
    }

    wait = false;

    for (int i = 0; i <  writersNumber + readersNumber; i++) {
        pthread_join(threadIds[i], NULL);
    }


    munmap(memory, ARRAYSIZE * sizeof(int));
    shm_unlink(SHARED_MEM);

    sem_close(resource);
    sem_unlink(RESOURCES_SEM);

    sem_close(readers);
    sem_unlink(READERS_SEM);

    sem_close(writers);
    sem_unlink(WRITERS_SEM);

    free(threadIds);

    return 0;
}


void *writerJob(void *args) {
    while (wait);

    for (int i = 0; i < howManyToWrite; i++) {
        //take semaphore
        if(sem_wait(resource) == -1){
            printf("Error while taking semaphore\n");
            exit(1);
        }

        if (verbose)
            printf("%lu WRITER : start to modify array\n", pthread_self());

        int toModify = rand() % ARRAYSIZE;
        for (int j = 0; j < toModify; j++) {
            int index = rand() % ARRAYSIZE;
            int value = rand() % MAXNUMBER;
            if (verbose) {
                printf("%lu WRITER : changed array[%d] => %d\n", pthread_self(), index, value);
            }
            memory[index] = value;
        }

        //give sepahore
        if(sem_post(resource) == -1){
            printf("Error while giving semaphore\n");
            exit(1);
        }
    }
    return NULL;
}

void *readerJob(void *args) {
    int divider = *((int *) args);
    while (wait);
    for (int i = 0; i < howManyToRead; i++) {

        if(sem_wait(readers) == -1){
            printf("Error while taking semaphore\n");
            exit(1);
        }
        readersWaiting++;

        if (readersWaiting == 1){
            if(sem_wait(resource) == -1){
                printf("Error while taking semaphore\n");
                exit(1);
            }
        }

        if(sem_post(readers) == -1){
            printf("Error while giving semaphore\n");
            exit(1);
        }

        int counter = 0;
        for (int j = 0; j < ARRAYSIZE; j++) {
            if (memory[j] % divider == 0) {
                counter++;
                if (verbose) {
                    printf("%lu READER : array[%d] = %d , divider = %d\n", pthread_self(), i, memory[i], divider);
                }
            }
        }
        printf("%lu READER : %d elements are dividable with %d\n", pthread_self(), counter, divider);

        if(sem_wait(readers) == -1){
            printf("Error while taking semaphore\n");
            exit(1);
        }

        readersWaiting--;
        if (readersWaiting == 0){
            if(sem_post(resource) == -1){
                printf("Error while giving semaphore\n");
                exit(1);
            }
        }

        if(sem_post(readers) == -1){
            printf("Error while giving semaphore\n");
            exit(1);
        }
    }

    return (void *) 0;
}

