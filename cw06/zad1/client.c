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

void registerClient();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Blad w linni komend\n Poprawne uruchomienie programu to: nazwa programu [nazwa uzytkownika]\n");
        return 1;
    }
    name = argv[1];
    working = 1;

    if (signal(SIGTSTP, signalHandler) == SIG_ERR) {
        printf("CLIENT ERROR : %s\n", strerror(errno)); exit(1);
    }

    struct mymesg message;
    pid = getpid();

    // get server key :
    if ((serverKey = ftok(".", 1)) == -1) {
        printf("CLIENT ERROR : %s\n", strerror(errno)); exit(1);
    }

    // get server queue id :
    if ((server = msgget(serverKey, 0200)) < 0) {
        printf("CLIENT ERROR : %s\n", strerror(errno)); exit(1);
    }

    // get client key :
    if ((clientKey = ftok(".", pid)) == -1) {
        printf("CLIENT ERROR : %s\n", strerror(errno)); exit(1);
    }

    // get client queue id :
    if ((client = msgget(clientKey, IPC_CREAT | 0600)) < 0) {
        printf("CLIENT ERROR : %s\n", strerror(errno)); exit(1);
    }

    // register client :
    message.mtype = pid;
    strcpy(message.mtext, name);
    if (msgsnd(server, &message, MAXNAME, 0) < 0) {
        printf("CLIENT ERROR : %s\n", strerror(errno));
        exit(1);
    }

    printf("Do zakmniecia nacisnij: CRTL + Z\n");
    printf("\nCommand >  ");

    if ((child = fork()) < 0) {
        printf("CLIENT ERROR : %s\n", strerror(errno));
        working = 0;
    } else if (child > 0) {
        // receiving from server :
        while (working) {
            usleep(500000);
            if (msgrcv(client, &message, MAXMSGSIZE, 1, IPC_NOWAIT) < 0) {
                if (errno != EAGAIN && errno != ENOMSG) {
                    printf("CLIENT ERROR : %s\n", strerror(errno));
                    working = 0;
                }
            } else {
                printf("FROM SERVER : %s\n\nCommand >  ", message.mtext);
            }
        }
        exit(0);
    } else if (child == 0) {
        // sending to server :
        while (working) {
            // get command :
            char *cmd = NULL;
            size_t len = MAXMSGSIZE;
            getline(&cmd, &len, stdin);

            for(int i=0;i<MAXMSGSIZE;i++) message.mtext[i] = 0;
            for(int i=0;i<strlen(cmd)-1;i++) message.mtext[i] = cmd[i];

            if (strcmp(message.mtext, "exit") == 0) {
                printf("Koniec\n");
                kill(getpid(), SIGTSTP);
            } else {
                // send to server
                message.mtype = 2;
                if (msgsnd(client, &message, MAXMSGSIZE, 0) < 0) {
                    printf("CLIENT ERROR : %s\n", strerror(errno));
                    working = 0;
                }
            }
        }
    }

    // ?
    if (msgctl(client, IPC_RMID, (struct msqid_ds *) NULL) < 0) {
        printf("CLIENT ERROR : %s\n", strerror(errno));
        exit(1);
    }

    return 0;
}

void signalHandler(int signal) {
    working = 0;
    kill(child, SIGTSTP);
}