#include "config.h"

int server;
int client;
int working;
pid_t pid;
pid_t child;
char *name;
key_t serverKey;
key_t clientKey;

void signalHandler(int signal);

void registerClient();;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Blad w linni komend\n Poprawne uruchomienie programu to: nazwa programu [nazwa uzytkownika]\n");
        return 1;
    }
    name = argv[1];
    working = 1;

    if (signal(SIGTSTP, signalHandler) == SIG_ERR) {
        printf("signal(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    struct mymesg message;
    pid = getpid();

    //Rejestracja kolejki serwera
    if ((serverKey = ftok(".", 1)) == -1) {
        printf("ftok(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }
    if ((server = msgget(serverKey, 0200)) < 0) {
        printf("msgget(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    //Rejestracja kolejki klienta
    if ((clientKey = ftok(".", pid)) == -1) {
        printf("ftok(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }
    if ((client = msgget(clientKey, IPC_CREAT | 0600)) < 0) {
        printf("msgget(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    //Rejestracja na serwerze
    message.mtype = pid;
    strcpy(message.mtext, name);
    if (msgsnd(server, &message, MAXNAME, 0) < 0) {
        printf("msgsnd(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    printf("Do zakmniecia nacisnij: CRTL + Z\n");

    if ((child = fork()) < 0) {
        printf("fork(): %d: %s\n", errno, strerror(errno));
        working = 0;
    } else if (child > 0) { //proces czytający
        while (working) {
            usleep(500000);
            if (msgrcv(client, &message, MAXMSGSIZE, 1, IPC_NOWAIT) < 0) {
                if (errno != EAGAIN && errno != ENOMSG) {
                    printf("msgrcv(): %d: %s\n", errno, strerror(errno));
                    working = 0;
                }
            } else {
                printf("%s\n", message.mtext);
            }
        }
        exit(0);
    } else if (child == 0) { //proces piszący
        char ch = '\n';
        int i;
        while (working) {
            for (i = 0; i < MAXMSGSIZE && (ch = fgetc(stdin)) != '\n'; ++i) {
                message.mtext[i] = ch;
            }
            message.mtext[i] = (char) 0;

            if (strcmp(message.mtext, "exit") == 0) {
                kill(getpid(), SIGTSTP);
            } else {
                message.mtype = 2;
                if (msgsnd(client, &message, MAXMSGSIZE, 0) < 0) {
                    printf("msgsnd(): %d: %s\n", errno, strerror(errno));
                    working = 0;
                }
            }
        }
    }

    if (msgctl(client, IPC_RMID, (struct msqid_ds *) NULL) < 0) {
        printf("msgctl(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    return 0;
}

//-----------------------------------------------------
void signalHandler(int signal) {
    working = 0;
    kill(child, SIGTSTP);
}