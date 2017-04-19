#include "config.h"

struct client {
    int queue;
    pid_t pid;
    char name[MAXNAME];
    int active;
};

int server;
int working;
int numOfClients;
struct client clients[MAXCLIENTS];

void signalHandler(int signal);
int registerClient(struct mymesg *message);
void sendToAll(struct mymesg *message, int i);


int main(int argc, char *argv[]) {

    working = 1;
    numOfClients = 0;

    // setting exit signals :
    if (signal(SIGTSTP, signalHandler) == SIG_ERR) {
        printf("signal(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    // get server key :
    key_t serverKey;
    struct mymesg message;

    if ((serverKey = ftok(".", 1)) == -1) {
        printf("ftok(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    // create server queue :
    if ((server = msgget(serverKey, IPC_CREAT | 0600)) < 0) {
        printf("msgget(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }


    printf("\nServer works\n");
    while (working) {
        // receive from client :
        if (msgrcv(server, &message, MAXNAME, 0, IPC_NOWAIT) < 0) {
            if (errno != EAGAIN && errno != ENOMSG) {
                printf("msgrcv(): %d: %s\n", errno, strerror(errno));
                working = 0;
            } else if (errno == E2BIG) {
                // TODO : CZEMU TU NIC NIE MA
            }
        } else {
            // client's registration :
            if (registerClient(&message) < 0)
                printf("Nie mozna dodac klienta\n");
            else
                printf("%s zalogował się!\n", message.mtext);
        }

        for (int i = 0; i < numOfClients; i++) {
            if (clients[i].active == 1) {

                // receive from clients :
                if (msgrcv(clients[i].queue, &message, MAXMSGSIZE, 2, IPC_NOWAIT) < 0) {
                    if (errno != EAGAIN && errno != ENOMSG && errno != EINVAL) {
                        printf("msgrcv(): %d: %s\n", errno, strerror(errno));
                        working = 0;
                    } else if (errno == EINVAL) {
                        clients[i].active = 0;
                    }
                } else {
                    // pobierz typ wiadomosci :
                    /*
                     * printf("From client : message.mtext");
                     * int type = getFromClient();
                     * if (type == exit) {
                     *     wyloguj_klienta
                     * } else if (type == ECHO) {
                     *     odeslij wiadomosc
                     * } else if (type == TO_UPPER) {
                     *     odeslij to_upper
                     * } else if (type == GET_TIME){
                     *     odeslij czas
                     * }
                     */
                    sendEcho(&message, i);
                }
            }
        }
        usleep(800000);
    }

    if (msgctl(server, IPC_RMID, (struct msqid_ds *) NULL) < 0) {
        printf("msgctl(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    return 0;
}

void signalHandler(int signal) {
    working = 0;
}

int registerClient(struct mymesg *message) {
    key_t tmp;

    clients[numOfClients].pid = (pid_t) message->mtype;
    strcpy(clients[numOfClients].name, message->mtext);
    clients[numOfClients].active = 1;

    if ((tmp = ftok(".", message->mtype)) == -1) {
        printf("ftok(): %d: %s\n", errno, strerror(errno));
        working = 0;
    }

    if ((clients[numOfClients].queue = msgget(tmp, 0600)) < 0) {
        printf("msgget(): %d: %s\n", errno, strerror(errno));
        working = 0;
    }
    numOfClients++;
    return 0;
}

void sendEcho(struct mymesg *message, int i) {
    struct mymesg response;
    response.mtype = 1;
    //time_t tm;
    //tm = time(NULL);
    //char result[MAXLENGTH];
    //sprintf(result, "\n%s<%s>  ", ctime(&tm), clients[i].name);
    //strcat(result, message->mtext);
    //printf("%s\n", result);
    strcpy(response.mtext, message->mtext);
    if (clients[i].active == 1) {
        if (msgsnd(clients[i].queue, &response, MAXMSGSIZE, 0) < 0) {
            printf("msgsnd(): %d: %s\n", errno, strerror(errno));
            working = 0;
        }
    }
}