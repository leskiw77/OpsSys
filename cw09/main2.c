#include "general.h"

int array[ARRAYSIZE];

pthread_t *readers;
pthread_t *writers;
int readersNumber;
int writersNumber;
int howManyToRead;
int howManyToWrite;
int currentReaders = 0;
sem_t readerSemaphore;
sem_t writerSemaphore;
bool infoMode = false;
bool wait = true;

void *readerFunction(void *arg);

void *writerFunction(void *arg);


void createAndStartThreads() {
    // create readers and writers :
    readers = malloc(sizeof(pthread_t) * readersNumber);
    writers = malloc(sizeof(pthread_t) * writersNumber);

    // create dividers :
    int *dividers = malloc(sizeof(int) * readersNumber);
    for (int i = 0; i < readersNumber; i++) {
        dividers[i] = rand() % (MAXNUMBER - 1) + 1;
    }

    // create and init mutex :
    pthread_mutexattr_t mtx;
    pthread_mutexattr_init(&mtx);
    pthread_mutexattr_settype(&mtx, PTHREAD_MUTEX_ERRORCHECK);

    // create threads :
    for (int i = 0; i < readersNumber; i++) {
        if (pthread_create(&readers[i], NULL, readerFunction, dividers + i) != 0) {
            fprintf(stderr, "Cannot create thread %d\n", i);
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
            fprintf(stderr, "Thread creation error %d\n", i);
            exit(1);
        }
    }
}


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
    if (argc == 6) {
        infoMode = true;
    }

    // initialize shared array :
    for (int i = 0; i < ARRAYSIZE; i++) {
        array[i] = rand() % MAXNUMBER;
    }

    // create semaphores :
    if (sem_init(&readerSemaphore, 0, 1) == -1) {
        fprintf(stderr, "Reader semaphore creation error\n");
        return 1;
    }
    if (sem_init(&writerSemaphore, 0, 1) == -1) {
        fprintf(stderr, "Writer semaphore creation error\n");
        return 1;
    }

    // create and start threads :
    createAndStartThreads();

    // clean :
    free(readers);
    free(writers);
    sem_destroy(&readerSemaphore);
    sem_destroy(&writerSemaphore);
    return 0;
}


/*
 * czytelnik uruchamiany jest z jednym argumentem - dzielnik i znajduje w tablicy wszystkie liczby,
 * które się przez niego dzielą bez reszty, wykonując cyklicznie operację przeszukiwania tablicy
 */
void *readerFunction(void *arg) {
    while(wait);
    int divider = *((int*)arg);

    for (int i = 0; i < howManyToRead; i++) {

        // TODO describe it :
        sem_wait(&readerSemaphore);
        if (++currentReaders == 1) {
            sem_wait(&writerSemaphore);
        }
        sem_post(&readerSemaphore);
        //


        // find how many elements are dividable :
        int counter = 0;
        for (int i = 0; i < ARRAYSIZE; i++) {
            if (array[i] % divider == 0) {
                counter++;
                if (infoMode) {
                    printf("%lu reader: array[%d] = %d , divider = %d\n", pthread_self(), i, array[i], divider);
                }
            }
        }
        printf("%lu reader: %d \n", pthread_self(), counter);


        // TODO describe it :
        sem_wait(&readerSemaphore);
        if (--currentReaders == 0) {
            sem_post(&writerSemaphore);
        }
        sem_post(&readerSemaphore);
        sleep(1);
    }
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
        sem_wait(&writerSemaphore);

        // change n elements in the array :
        printf("%lu writer is going to modify array\n", pthread_self());
        n = rand() % ARRAYSIZE + 1;
        for (int j = 0; j < n; j++) {
            index = rand() % ARRAYSIZE;
            value = rand() % MAXNUMBER;
            array[index] = value;
            if (infoMode) {
                printf("%lu writer has changed array[%d] => %d\n", pthread_self(), index, value);
            }
        }

        sem_post(&writerSemaphore);
        sleep(1);
    }
}