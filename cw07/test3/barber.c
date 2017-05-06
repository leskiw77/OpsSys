#define _GNU_SOURCE


#include "general.h"
#include "Fifo.h"

key_t memKey;// fifoKey;
int memId;//shmID = -1;
FIFO * fifo;// = NULL;
int semId;//SID = -1;

void sigHandler(int sig) {
    printf("Barber finish his work\n");
    shmdt(fifo);
    shmctl(memId, IPC_RMID, NULL);

    semctl(semId, 0, IPC_RMID, 0);
    exit(1);
}

void cut(pid_t pid){
    printf("Gole uzytkownika %d\n",pid);
    kill(pid, SIGRTMIN);
}

int main(int argc, char** argv){
    signal(SIGINT, sigHandler);

    if (argc != 2) {
        printf("Barber needs argument specifying amount of chairs in waiting room\n");
        exit(1);
    }

    int chairsAmount = atoi(argv[1]);
    if (chairsAmount >= MAX_SIZE) {
        printf("No one has so many chairs\n");
        exit(1);
    }




    key_t memKey = ftok(".", 'm');
    memId = shmget(memKey, sizeof(FIFO), IPC_CREAT | 0666);
    fifo = (FIFO *) shmat(memId, NULL, 0);
    fifo->size = 0;
    fifo->max = chairsAmount;


    key_t semKey = ftok(".", 's');
    semId = semget(semKey, 4, IPC_CREAT | IPC_EXCL | 0666);

    //szybciej bylo 0 przy barber
    //1 to mozliwosc korzystania
    semctl(semId, BARBER, SETVAL, 0);
    semctl(semId, CLIENTS_FIFO, SETVAL, 0);
    semctl(semId, VISIT, SETVAL, 0);

    if(semId == -1 || memId == -1 || fifo == NULL){
        printf("Cos poszlo nie tak");
        return 1;
    }


    while(1){
        struct sembuf buf;
        buf.sem_num = BARBER;
        buf.sem_op = -1;
        buf.sem_flg = 0;

        semop(semId, &buf, 1);

        printf("dupa");

        //moze uda zapakowac sie do petli
        buf.sem_num = CLIENTS_FIFO;
        buf.sem_op = -1;
        semop(semId, &buf, 1);

        pid_t toCut = get_first(fifo);

        buf.sem_op = 1;
        semop(semId, &buf, 1);

        cut(toCut);


        while(1){
            buf.sem_num = CLIENTS_FIFO;
            buf.sem_op = -1;
            semop(semId, &buf, 1);

            pid_t toCut = get_first(fifo);



            if(toCut != -1){ // jesli istnial, to zwolnij kolejke, ostrzyz i kontynuuj
                buf.sem_op = 1;
                semop(semId, &buf, 1);

                cut(toCut);

            }else{ // jesli kolejka pusta, to ustaw, ze spisz, zwolnij kolejke i spij dalej (wyjdz z petli)

                //golibroda idzie spac
                buf.sem_num = BARBER;
                buf.sem_op = -1;
                semop(semId, &buf, 1);

                buf.sem_num = CLIENTS_FIFO;
                buf.sem_op = 1;
                semop(semId, &buf, 1);
                break;
            }
        }
    }
}
