#include "config.h"

struct client {
    int queue;
    pid_t pid;
    char name[MAXNAME];
    int active;
};

//-----------------------------------------------------
int server;
int working;
int numOfClients;
struct client clients[MAXCLIENTS];

//-----------------------------------------------------
void signalHandler(int signal);

int registerClient(struct mymesg *message);

void sendToAll(struct mymesg *message, int i);

//-----------------------------------------------------
int main(int argc, char *argv[]) {

    working = 1;
    numOfClients = 0;

    // obsluga sygnalow :
    if (signal(SIGTSTP, signalHandler) == SIG_ERR) {
        printf("signal(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    // ?
    key_t serverKey;
    struct mymesg message;

    if ((serverKey = ftok(".", 1)) == -1) {
        printf("ftok(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    // create ser
    if ((server = msgget(serverKey, IPC_CREAT | 0600)) < 0) {
        printf("msgget(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }


    printf("\nSerwer działa!\n");
    while (working) {
        if (msgrcv(server, &message, MAXNAME, 0, IPC_NOWAIT) < 0) {
            if (errno != EAGAIN && errno != ENOMSG) {
                printf("msgrcv(): %d: %s\n", errno, strerror(errno));
                working = 0;
            } else if (errno == E2BIG) {

            }
        } else {
            if (registerClient(&message) < 0) {
                printf("Nie mozna dodac klienta\n");
            } else {
                printf("%s zalogował się!\n", message.mtext);
            }
        }
        int i;
        for (i = 0; i < numOfClients; ++i) {
            if (clients[i].active == 1) {
                if (msgrcv(clients[i].queue, &message, MAXMSGSIZE, 2, IPC_NOWAIT) < 0) {
                    if (errno != EAGAIN && errno != ENOMSG && errno != EINVAL) {
                        printf("msgrcv(): %d: %s\n", errno, strerror(errno));
                        working = 0;
                    } else if (errno == EINVAL) {
                        clients[i].active = 0;
                    }
                } else {
                    sendToAll(&message, i);
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

//-----------------------------------------------------
void signalHandler(int signal) {
    working = 0;
}

//-----------------------------------------------------
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

//-----------------------------------------------------
void sendToAll(struct mymesg *message, int i) {
    struct mymesg message2;
    message2.mtype = 1;
    time_t tm;
    tm = time(NULL);
    char result[MAXLENGTH];
    sprintf(result, "\n%s<%s>  ", ctime(&tm), clients[i].name);
    strcat(result, message->mtext);
    printf("%s\n", result);
    strcpy(message2.mtext, result);
    int j;
    for (j = 0; j < numOfClients; ++j) {
        if ((j != i) && (clients[j].active == 1)) {
            if (msgsnd(clients[j].queue, &message2, MAXMSGSIZE, 0) < 0) {
                printf("msgsnd(): %d: %s\n", errno, strerror(errno));
                working = 0;
            }
        }
    }
}