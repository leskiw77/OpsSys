#define _GNU_SOURCE

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <poll.h>
#include "structs.h"

char *socket_path;
int port;
client clients[MAX_CLIENTS];
struct pollfd pfd[MAX_CLIENTS + 2];
int pinged[MAX_CLIENTS];
int listen_fd[2];
pthread_t threads[3];

char toString(Operator o);
int randomClient();
int getEmpty();
Operation * parseData(char data[1000]);
void *terminalThread(void *arg);
void handleResult(Operation o);
void *listenerThread(void *arg);
void *pingingThread(void *arg);
void parseArgs(int argc, char **argv);
void init();
void exitHandle(int s);

int main(int argc, char **argv) {
    // PARSE ARGUMENTS :
    if (argc != 3) {
        printf("Wrong arguments.\nUsage:\n./server (TCP/UDP port) (UNIX socket path)\n");
        exit(1);
    }
    port = atoi(argv[1]);
    socket_path = malloc(strlen(argv[2]) + 1);
    strcpy(socket_path, argv[2]);

    // INIT :
    srand(time(NULL));
    for (int i = 0; i < MAX_CLIENTS; i++) {
        strcpy(clients[i].name, "");
        clients[i].fd = -1;
        pfd[i].fd = -1;
        pfd[i].events = POLLRDHUP;
        pinged[i] = 0;
    }
    pfd[MAX_CLIENTS].events = POLLIN;
    pfd[MAX_CLIENTS + 1].events = POLLIN;

    signal(SIGINT, exitHandle);

    struct sockaddr_in addr_in;
    struct sockaddr_un addr_un;

    // INTERNET LISTENER:
    listen_fd[0] = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    pfd[MAX_CLIENTS].fd = listen_fd[0];

    // UNIX LISTENER :
    listen_fd[1] = socket(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0);
    pfd[MAX_CLIENTS + 1].fd = listen_fd[1];

    // ?
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_in.sin_port = htons(port);

    addr_un.sun_family = AF_UNIX;
    strcpy(addr_un.sun_path, socket_path);

    // BINDING LISTENERS :
    bind(listen_fd[0], (struct sockaddr *) &addr_in, sizeof(struct sockaddr_in));
    bind(listen_fd[1], (struct sockaddr *) &addr_un, sizeof(struct sockaddr_un));

    // CREATING PROPER THREADS :
    if (pthread_create(&threads[0], NULL, listenerThread, NULL) != 0) {
        printf("Error while creating thread.\n");
        exit(1);
    }

    if (pthread_create(&threads[1], NULL, terminalThread, NULL) != 0) {
        printf("Error while creating thread.\n");
        exit(1);
    }

    if (pthread_create(&threads[2], NULL, pingingThread, NULL) != 0) {
        printf("Error while creating thread.\n");
        exit(1);
    }

    // JOINING THREADS :
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }
    return 0;
}

char toString(Operator o) {
    if (o == ADD) return '+';
    if (o == SUBTRACT) return '-';
    if (o == MULTIPLY) return '*';
    if (o == DIVIDE) return '/';
    return '\0';
}

int randomClient() {
    int t[MAX_CLIENTS];
    int id = 0;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd > 0) {
            t[id++] = i;
        }
    }
    if (id == 0) {
        return -1;
    }
    return t[rand() % id];
}

int getEmpty() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].fd <= 0) {
            return i;
        }
    }
    return -1;
}

Operation * parseData(char data[1000]) {
    char num1[25];
    char num2[25];
    char op;
    int i, j;
    for (i = 0; i < strlen(data) && data[i] >= '0' && data[i] <= '9'; i++) {
        num1[i] = data[i];
    }
    op = data[i++];
    for (j = i; j < strlen(data); j++) {
        num2[j - i] = data[j];
    }
    num2[j - i - 1] = num1[i - 1] = '\0';

    Operation *o = malloc(sizeof(Operation));
    o->arg1 = atoi(num1);
    o->arg2 = atoi(num2);
    if (op == '+') {
        o->op = ADD;
    } else if (op == '-') {
        o->op = SUBTRACT;
    } else if (op == '*') {
        o->op = MULTIPLY;
    } else if (op == '/') {
        o->op = DIVIDE;
    }
    return o;
}


// READ LINES AND SEND REQUEST TO RANDOM CLIENT :
void * terminalThread(void *arg) {
    char buf[1000];
    Operation *op;
    while (1) {
        fgets(buf, 1000, stdin);
        op = parseData(buf);
        int id = randomClient();
        if (id < 0) {
            printf("No client is connected to server.\n");
            continue;
        }
        int fd = clients[id].fd;
        write(fd, (void *) op, sizeof(Operation));
        free(op);
    }
    return NULL;
}

// RECEIVE RESPONSE FROM CLIENT :
void handleResult(Operation o) {
    if (o.op == ADD || o.op == SUBTRACT || o.op == DIVIDE || o.op == MULTIPLY) {
        printf("%d%c%d=%d (calculated by client %s)\n", o.arg1, toString(o.op), o.arg2, o.result, o.name);
    } else if (o.op == PING) {
        int i;
        for(i = 0; i < MAX_CLIENTS; i++) {
            if(strcmp(clients[i].name, o.name) == 0) {
                break;
            }
        }
        pinged[i] = 0;
    } else if (o.op == EXIT) {
        // CLIENT EXITING :
        int i;
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (strcmp(clients[i].name, o.name) == 0) {
                break;
            }
        }
        shutdown(pfd[i].fd, SHUT_RDWR);
    }
}

// LISTEN TO SOCKETS :
void * listenerThread(void *arg) {
    listen(listen_fd[0], MAX_CLIENTS);
    listen(listen_fd[1], MAX_CLIENTS);
    int f;
    Operation o;
    char buf[1024];
    while (1) {

        for (int i = 0; i < MAX_CLIENTS; i++) {
            pfd[i].events = POLLIN;
        }

        poll(pfd, MAX_CLIENTS + 2, -1);

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if ((pfd[i].revents & (POLLRDHUP | POLLHUP)) != 0) {
                pfd[i].fd = -1;
                clients[i].fd = -1;
                strcpy(clients[i].name, "");
                pinged[i] = 0;
            } else if ((pfd[i].revents & POLLIN) != 0) {
                read(pfd[i].fd, buf, sizeof(Operation));
                handleResult(*(Operation *) buf);
            }
        }

        while ((f = accept(pfd[MAX_CLIENTS].fd, NULL, NULL)) > 0) {
            read(f, buf, sizeof(Operation));
            o = *(Operation *) buf;
            int id = getEmpty();
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].fd > 0 && strcmp(clients[i].name, o.name) == 0) {
                    id = -1;
                }
            }
            if (id < 0) {
                o.op = DENY;
                write(f, (void *) &o, sizeof(Operation));
                shutdown(f, SHUT_RDWR);
                close(f);
                break;
            }
            o.op = ACCEPT;
            write(f, (void *) &o, sizeof(Operation));
            pfd[id].fd = f;
            strcpy(clients[id].name, o.name);
            clients[id].fd = f;
        }

        while ((f = accept(pfd[MAX_CLIENTS + 1].fd, NULL, NULL)) > 0) {
            read(f, buf, sizeof(Operation));
            o = *(Operation *) buf;
            int id = getEmpty();
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].fd > 0 && strcmp(clients[i].name, o.name) == 0) {
                    id = -1;
                }
            }
            if (id < 0) {
                o.op = DENY;
                write(f, (void *) &o, sizeof(Operation));
                shutdown(f, SHUT_RDWR);
                close(f);
                break;
            }
            o.op = ACCEPT;
            write(f, (void *) &o, sizeof(Operation));
            pfd[id].fd = f;
            strcpy(clients[id].name, o.name);
            clients[id].fd = f;
        }
    }
    return NULL;
}


// PINGING THREAD ???????????????????????????????? CZEMU ????????????????????????????
void *pingingThread(void *arg) {
    Operation op;
    op.op = PING;
    while (1) {
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].fd > 0) {
                if (pinged[i] == 1) {
                    printf("Client %s did not respond to ping. Connection closed.\n", clients[i].name);
                    shutdown(clients[i].fd, SHUT_RDWR);
                    close(clients[i].fd);
                    clients[i].fd = -1;
                    strcpy(clients[i].name, "");
                    pfd[i].fd = -1;
                    pinged[i] = 0;
                    continue;
                }
                write(clients[i].fd, (void *) &op, sizeof(Operation));
                pinged[i] = 1;
            }
        }
        usleep(2500000);
    }
    return NULL;
}


void exitHandle(int s) {
    unlink(socket_path);
    exit(0);
}