#include "general.h"

void *readerFunction(void *arg);

void *writerFunction(void *arg);

int array[ARRAYSIZE];
int readersNumber;
int writersNumber;

int howManyToRead;
int howManyToWrite;

pthread_t *readers;
pthread_t *writers;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t readersQueueCond = PTHREAD_COND_INITIALIZER;
pthread_cond_t writersQueueCond = PTHREAD_COND_INITIALIZER;

int wrt = 0, rdr = 0;
int currentWriters = 0;

bool infoMode = false;
bool wait = true;

int main(int argc, char **argv) {
    // parse args :
    if ((argc != 5 && argc != 6) || (argc == 6 && strcmp(argv[5], "-i") != 0)) {
        printf("Wrong arguments\n"
                       "Args : number of clients, number of writers, how many write, how many read, [-i]");
        return 1;
    }
    readersNumber = atoi(argv[1]);
    writersNumber = atoi(argv[2]);
    howManyToRead = atoi(argv[3]);
    howManyToWrite = atoi(argv[4]);
    infoMode = (argc == 6);

    // initialize array :
    for (int i = 0; i < ARRAYSIZE; i++) {
        array[i] = rand() % MAXNUMBER;
    }

    // create readers and writers :
    readers = malloc(sizeof(pthread_t) * readersNumber);
    writers = malloc(sizeof(pthread_t) * writersNumber);

    // get dividers for readers :
    int *dividers = malloc(sizeof(int) * readersNumber);
    for (int i = 0; i < readersNumber; i++) {
        dividers[i] = rand() % (MAXNUMBER - 1) + 1;
    }

    // create threads :
    for (int i = 0; i < readersNumber; i++) {
        if (pthread_create(&readers[i], NULL, readerFunction, dividers + i) != 0) {
            fprintf(stderr, "Thread creation error %d\n", i);
            exit(1);
        }
    }
    wait = false;
    for (int i = 0; i < writersNumber; i++) {
        if (pthread_create(&writers[i], NULL, writerFunction, NULL) != 0) {
            fprintf(stderr, "Thread creation error %d\n", i);
            exit(1);
        }
    }
    for (int i = 0; i < readersNumber; i++) {
        if (pthread_join(readers[i], NULL) != 0) {
            fprintf(stderr, "Thread joining error %d\n", i);
            exit(1);
        }
    }

    // clean :
    free(readers);
    free(writers);
    return 0;
}


void *readerFunction(void *arg) {
    while (wait);
    int divider = *((int *) arg);

    for (int i = 0; i < howManyToRead; i++) {

        pthread_mutex_lock(&mutex);
        while (wrt != 0) {
            pthread_cond_wait(&readersQueueCond, &mutex);
        }
        rdr++;
        pthread_mutex_unlock(&mutex);

        // find how many elements are dividable :
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

        pthread_mutex_lock(&mutex);
        rdr--;
        if (rdr == 0) {
            pthread_cond_broadcast(&writersQueueCond);
        } else {
            pthread_cond_broadcast(&readersQueueCond);
        }
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    return NULL;
}


/*
 * Pisarz cyklicznie modyfikuje w sposób losowy wybrane liczby w tablicy
 * (losuje ilość liczb do modyfikacji, ich pozycje w tablicy oraz wartości tych liczb)
 *
 * Pisarz raportuje na standardowym wyjściu wykonanie swojej operacji (modyfikacji tablicy),
 * w wersji opisowej programu (opcja -i) wypisuje indeks i wpisaną wartość
 */
void *writerFunction(void *arg) {
    while (wait);
    int n, index, value;
    for (int i = 0; i < howManyToWrite; i++) {

        pthread_mutex_lock(&mutex);
        wrt++;
        while (rdr > 0 || currentWriters > 0) {
            pthread_cond_wait(&writersQueueCond, &mutex);
        }

        currentWriters++;
        pthread_mutex_unlock(&mutex);

        // change n elements in the array :
        printf("%lu WRITER : start to modify array\n", pthread_self());
        n = rand() % ARRAYSIZE + 1;
        for (int j = 0; j < n; j++) {
            index = rand() % ARRAYSIZE;
            value = rand() % MAXNUMBER;
            array[index] = value;
            if (infoMode) {
                printf("%lu WRITER : changed array[%d] => %d\n", pthread_self(), index, value);
            }
        }

        pthread_mutex_lock(&mutex);
        wrt--;
        currentWriters--;
        if (wrt > 0) {
            pthread_cond_signal(&writersQueueCond);
        } else {
            pthread_cond_broadcast(&readersQueueCond);
        }
        pthread_mutex_unlock(&mutex);

        sleep(1);
    }
    return NULL;
}