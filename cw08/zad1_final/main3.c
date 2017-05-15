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

    fileName = (char *) malloc(MAXFILENAME * sizeof(char));
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

    // create threads :
    threads = (pthread_t *) malloc(threadsNum * sizeof(pthread_t));
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
    // setting cancel type :
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    sleep(1);

    char **readRecords = calloc(recordsNum, sizeof(char *));
    for (int i = 0; i < recordsNum; i++) {
        readRecords[i] = calloc(1024, sizeof(char));
    }
    char *num = (char *) malloc(MAXIDDIGITS * sizeof(char));
    int numOfReadChars;

    // lock mutex and read data to buffers :
    pthread_mutex_lock(&mutex);
    for(int i = 0; i < recordsNum; i++){
        if((numOfReadChars = read(file, readRecords[i], BUFFERSIZE)) == -1) {
            printf("read(): %d: %s\n", errno, strerror(errno));
            exit(-1);
        }
    }

    // all records loaded to buffers so cancel others threads :
    if(numOfReadChars == 0) {
        for(int j = 0; j < threadsNum; j++) {
            if(threads[j] != pthread_self()) {
                pthread_cancel(threads[j]);
            }
        }
        pthread_mutex_unlock(&mutex);
        return NULL;
    }
    pthread_mutex_unlock(&mutex);

    // find searched word in records :
    for(int i = 0; i < recordsNum; i++) {
        if(strstr(readRecords[i], searched) != NULL) {
            strncpy(num, readRecords[i], 2);
            printf("%ld: found word in record id = %d\n", pthread_self(), i);
        }
    }
    return NULL;
}