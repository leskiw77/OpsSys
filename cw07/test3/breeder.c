#include "general.h"

int memID;//shmID = -1;
FIFO * fifo;// = NULL;
int semId;// = -1;
int counter;

void sigHandler(int sig) {
    counter++;
}

int takePlace2();
void getCut(int serviceAmount);

int main(int argc, char** argv){

    signal(SIGRTMIN, sigHandler);

    if (argc != 3) {
        fprintf(stderr, "You should provide clone order details: number of clones and number of haircuts\n");
        exit(EXIT_FAILURE);
    }
    int clientsAmount = atoi(argv[1]);
    int serviceAmount = atoi(argv[2]);

    //przygotowanie pamieci dzielonej
    key_t memKey = ftok(".", 'm');
    int memId = shmget(memKey, 0, 0);
    FIFO *fifo = (FIFO *) shmat(memId, NULL, 0);

    //przygotowanie semaforow
    key_t semKey = ftok(".", 's');
    semId = semget(semKey, 0, 0);

    if(semId == -1 || memId == -1 || fifo == NULL){
        printf("Cos poszlo nie tak");
        return 1;
    }

    for(int i=0; i<clientsAmount; i++){
        pid_t id = fork();
        if(id == 0){
            getCut(serviceAmount);
            return 0;
        }
    }


    while(1){
        wait(NULL); // czekaj na dzieci
        if (errno == ECHILD) break;
    }

    printf("All clients has been bred!\n");

    return 0;
}



int takePlace(){

    pid_t myPID = getpid();
    int res =  put_last(fifo, myPID);

  int barbState = semctl(semId, 0, GETVAL);



   if(barbState == 0){
       struct sembuf buf;
       buf.sem_num = BARBER;
       buf.sem_op = 1;
       buf.sem_flg = 0;

       //budzenie golibrody
       semop(semId, &buf, 1);

       //why the fuck
       semop(semId, &buf, 1);

   }

   if(res == -1){
       printf("Nie ma miejsca w poczekalni\n");
       return 0;
   }else{
       printf("Usiadl w poczekalni\n");
   }


    return 1;
}


void getCut(int serviceAmount){
    counter = 0;


    while(counter < serviceAmount){
        struct sembuf buf;
        buf.sem_num = VISIT;
        buf.sem_op = -1;
        buf.sem_flg = 0;
        semop(semId, &buf, 1);

        buf.sem_num = CLIENTS_FIFO;
        semop(semId, &buf, 1) == -1;

        int res = takePlace();

        buf.sem_op = 1;
        semop(semId, &buf, 1) == -1;

        buf.sem_num = VISIT;
        semop(semId, &buf, 1);

        if(res){
            pause();
            //wait(-1);
            //sigsuspend(&fullMask);
            //long timeMarker = getMicroTime();
            //printf("Time: %ld, Client %d just got cut!\n", timeMarker, getpid()); fflush(stdout);
        } else{
            printf("nie ma dla mnie miejsca");
        }
    }
}
