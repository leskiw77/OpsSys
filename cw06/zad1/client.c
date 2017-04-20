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
bool startsWith(char * prefix, char * text);
void registerClient();

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Blad w linii komend\n Poprawne uruchomienie programu to: nazwa programu [nazwa uzytkownika]\n");
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
                printf("FROM SERVER : %s\n", message.mtext);
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
            for (int i = 0; i < MAXMSGSIZE; i++) message.mtext[i] = 0;
            for (int i = 0; i < strlen(cmd) - 1; i++) message.mtext[i] = cmd[i];

            if (strcmp(message.mtext, "exit") == 0) {
                printf("Koniec\n");
                kill(getpid(), SIGTSTP);
            } else if (strcmp(message.mtext, "time") == 0) {
                message.mtype = 2;
                message.mrequest = GET_TIME;
                if (msgsnd(client, &message, MAXMSGSIZE, 0) < 0) {
                    printf("CLIENT ERROR : %s\n", strerror(errno));
                    working = 0;
                }
            } else if (startsWith("echo",message.mtext)) {
                // send to server
                message.mtype = 2;
                message.mrequest = ECHO;
                strcpy(message.mtext,message.mtext+5);
                if (msgsnd(client, &message, MAXMSGSIZE, 0) < 0) {
                    printf("CLIENT ERROR : %s\n", strerror(errno));
                    working = 0;
                }
            } else if (startsWith("upper",message.mtext)) {
                // send to server
                message.mtype = 2;
                message.mrequest = TO_UPPER;
                strcpy(message.mtext,message.mtext+6);
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

bool startsWith(char * prefix, char * text) {
    size_t lenpre = strlen(prefix),
            lenstr = strlen(text);
    return lenstr < lenpre ? false : strncmp(prefix, text, lenpre) == 0;
}