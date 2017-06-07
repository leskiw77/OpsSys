#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

#define BUFFER_SIZE 10

int buffer = 5;


pthread_mutex_t producer_mutex;
pthread_mutex_t consumer_mutex;
pthread_mutex_t buffer_mutex;

void * consumerFunction(void * omit);
void * producerFunction(void * omit);

int main(int argc, char const *argv[]) {

    srand(time(NULL));
    pthread_t producer, consumer1, consumer2;

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

    if (pthread_create(&producer, NULL, producerFunction, NULL) != 0) {
        printf("pthread_create(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    if (pthread_create(&consumer1, NULL, consumerFunction, NULL) != 0) {
        printf("pthread_create(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    if (pthread_create(&consumer2, NULL, consumerFunction, NULL) != 0) {
        printf("pthread_create(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    /*

    if (pthread_create(&consumer1, NULL, consumerFunction, NULL) != 0) {
        printf("pthread_create(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

     */

    /*
    if (pthread_create(&consumer2, NULL, consumerFunction, NULL) != 0) {
        printf("pthread_create(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }


    pthread_join(producer, NULL);
    pthread_join(consumer1, NULL);
  */
    pthread_join(producer, NULL);
    pthread_join(consumer1, NULL);
    pthread_join(consumer2, NULL);
    return 0;
}

void * producerFunction(void * omit){

    pthread_mutex_lock(&producer_mutex);

    while(true){
        pthread_mutex_lock(&buffer_mutex);
        if(buffer<BUFFER_SIZE){
            buffer++;
            printf("Producer add one product. Current amount: %d\n",buffer);
        }
        if(buffer == BUFFER_SIZE){
            pthread_mutex_unlock(&buffer_mutex);
            printf("Producer locked\n");
            pthread_mutex_lock(&producer_mutex);
            printf("Producer unlocked\n");

        }else if(buffer == 1){
            pthread_mutex_unlock(&consumer_mutex);
            pthread_mutex_unlock(&buffer_mutex);
        }
        else{
            pthread_mutex_unlock(&buffer_mutex);
        }

        usleep(200000);
    }


    return NULL;
}


void * consumerFunction(void * omit){

    pthread_mutex_lock(&consumer_mutex);

    while(true){
        pthread_mutex_lock(&buffer_mutex);
        if(buffer<=BUFFER_SIZE){
            buffer--;
            printf("Consumer take one product. Current amount: %d\n",buffer);
        }
        //juz nie jest pelny
        if(buffer == BUFFER_SIZE-1){
            pthread_mutex_unlock(&producer_mutex);
            pthread_mutex_unlock(&buffer_mutex);

        }else if(buffer == 0){
            pthread_mutex_unlock(&buffer_mutex);
            printf("Consumer locked\n");
            pthread_mutex_lock(&consumer_mutex);
            printf("Consumer unlocked\n");
        }
        else{
            pthread_mutex_unlock(&buffer_mutex);
        }

        usleep(100000);
    }







    sleep(3);
    printf("konsument dziala\n");
    pthread_mutex_unlock(&buffer_mutex);
    //pthread_mutex_lock(&buffer_mutex);
    //printf("klient zablokowal");
    /*
    while(true){
        pthread_mutex_lock(&buffer_mutex);
        if(buffer>0)
            buffer--;
        printf("Consumer take one product. Current amount: %d\n",buffer);
        pthread_mutex_unlock(&buffer_mutex);

        usleep(100000 * (rand()%10+1));
    }

     */
    return NULL;
}
