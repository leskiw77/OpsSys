#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define BUFFER_SIZE 100

int buffer = BUFFER_SIZE/2;


pthread_mutex_t producer_mutex;
pthread_mutex_t consumer_mutex;
pthread_mutex_t buffer_mutex;

void * consumerFunction(void * omit);
void * producerFunction(void * omit);

int main(int argc, char const *argv[]) {
    if (argc != 3) {
        printf("Wrong arguments\n"
                       "args: [producers number] [consumers number] \n");
        exit(1);
    }
    srand(time(NULL));
    pthread_t * producers, * consumers;

    int producersAmount = atoi(argv[1]);
    int consumersAmount = atoi(argv[2]);
    producers = (pthread_t *)malloc(producersAmount * sizeof(pthread_t));
    consumers = (pthread_t *)malloc(consumersAmount * sizeof(pthread_t));

    if (pthread_mutex_init(&buffer_mutex, NULL) != 0) {
        printf("pthread_mutex_init(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }
    if (pthread_mutex_init(&producer_mutex, NULL) != 0) {
        printf("pthread_mutex_init(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }
    if (pthread_mutex_init(&consumer_mutex, NULL) != 0) {
        printf("pthread_mutex_init(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    for(int i=0;i<producersAmount;i++){
        printf("Producers %d\n",i);
        if (pthread_create(&producers[i], NULL, producerFunction, NULL) != 0) {
            printf("pthread_create(): %d: %s\n", errno, strerror(errno));
            exit(1);
        }
    }

    for(int i=0;i<consumersAmount;i++){
        printf("Consumers %d\n",i);

        if (pthread_create(&consumers[i], NULL, consumerFunction, NULL) != 0) {
            printf("pthread_create(): %d: %s\n", errno, strerror(errno));
            exit(1);
        }
    }

    //joiny
    for(int i=0;i<producersAmount;i++){
        pthread_join(producers[i], NULL);

    }

    for(int i=0;i<consumersAmount;i++){
        pthread_join(consumers[i], NULL);
    }

    return 0;
}

void * producerFunction(void * omit){
    while(true){
        pthread_mutex_lock(&producer_mutex);
        pthread_mutex_lock(&buffer_mutex);
        if(buffer<BUFFER_SIZE){
            buffer++;
            printf("Producer add one product. Current amount: %d %lu\n",buffer, pthread_self());
        }
        if(buffer == BUFFER_SIZE){
            pthread_mutex_unlock(&buffer_mutex);
            printf("Producer locked\n");
            pthread_mutex_lock(&producer_mutex);
            printf("Producer unlocked\n");

        }else if(buffer == 1){
            pthread_mutex_unlock(&buffer_mutex);
            pthread_mutex_unlock(&consumer_mutex);
        }
        else{
            pthread_mutex_unlock(&buffer_mutex);
        }

        pthread_mutex_unlock(&producer_mutex);
        usleep(100000*(rand()%5+1));

    }
    return NULL;
}


void * consumerFunction(void * omit){



    while(true){
        pthread_mutex_lock(&consumer_mutex);
        pthread_mutex_lock(&buffer_mutex);
        if(buffer<=BUFFER_SIZE){
            buffer--;
            printf("Consumer take one product. Current amount: %d %lu\n",buffer, pthread_self());
        }
        //juz nie jest pelny
        if(buffer == BUFFER_SIZE-1){
            pthread_mutex_unlock(&buffer_mutex);
            pthread_mutex_unlock(&producer_mutex);

        }else if(buffer == 0){
            pthread_mutex_unlock(&buffer_mutex);
            printf("Consumer locked\n");
            pthread_mutex_lock(&consumer_mutex);
            printf("Consumer unlocked\n");
        }
        else{
            pthread_mutex_unlock(&buffer_mutex);
        }
        pthread_mutex_unlock(&consumer_mutex);

        usleep(100000*(rand()%5+1));
    }
    return NULL;
}
