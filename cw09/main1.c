#include "general.h"

int array[ARRAYSIZE];
int verbose = 1;

void * writerFunction(void *unused) {

    int operations = rand()%MAXOPERATION;

    for(int i=0;i<operations;i++){

        int index = rand()%ARRAYSIZE;
        int exchangeElem = rand()%MAXNUMBER;
        if(verbose){
            printf("Writer tid=%ld exchange element %d. Old value was %d and a new one is %d\n",pthread_self(), index,array[index],exchangeElem);
        }
        array[index] = exchangeElem;

    }

    return NULL;
}

void * readerFunction(void * number) {
    int divider = *((int *)number);
    if(divider == 0){
        return NULL;
    }
    int correct[ARRAYSIZE];
    int j=0;

    for(int i=0;i<ARRAYSIZE;i++){
        if((array[i] % divider) == 0){
            correct[j] = array[i];
            j++;
        }
    }

    if(verbose){
        printf("Dividers of %d: ",divider);
        for(int i=0;i<j;i++){
            printf("%d ",correct[i]);
        }
        printf("\n");
    }

    return NULL;
}

int main(int argc, char *argv[]) {

    srand (time(NULL));

    if (argc != 3) {
        printf("Wrong arguments!\n");
        return 1;
    }

    for(int i=0;i<ARRAYSIZE;i++)
        array[i] = i;

    int writersNumber = atoi(argv[1]);
    int readersNumber = atoi(argv[2]);

    pthread_t * writerThreads = (pthread_t *)malloc(writersNumber * sizeof(pthread_t));
    pthread_t * readerThreads = (pthread_t *)malloc(readersNumber * sizeof(pthread_t));

    for (int i = 0; i < writersNumber; i++) {
        if (pthread_create(&writerThreads[i], NULL, writerFunction, NULL) != 0) {
            printf("pthread_create(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
    }

    int tmp;
    for (int i = 0; i < readersNumber; i++) {
        tmp = rand()%MAXDEVIDER+1;
        if (pthread_create(&readerThreads[i], NULL, readerFunction,(void *)&tmp) != 0) {
            printf("pthread_create(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
        usleep(1);
    }

    for (int i = 0; i < writersNumber; i++) {
        pthread_join(writerThreads[i], NULL);
    }

    for (int i = 0; i < readersNumber; i++) {
        pthread_join(readerThreads[i], NULL);
    }

    free(writerThreads);
    free(readerThreads);
    return 0;
}

