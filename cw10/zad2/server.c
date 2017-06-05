#define _GNU_SOURCE
#define gettid() syscall(SYS_gettid)

#include "helpers.h"

short validatePort(short port);
char *validatePath(char *path);
void exitHandler(int signo);
void cleanUp(void);
int getWebSocket(short portNum);
int getLocalSocket(char *path);
void *pingerFunction(void *);
void *terminalReaderFunction(void *);
int prepareMonitor();
void handleNewMessage(int fd);
void registration(int fd, struct sockaddr *addr, socklen_t aSize, Message *mess);
void removeClient(int index);
int pushCommand(char command, int op1, int op2, char *name);
Client *findClient(char *name);
int getSFD(char type);

int webSocket;
int localSocket;
char *unixPath;

int counter = 0;
int cN = 0;
Client clients[50];
int epoll;
pthread_t pinger;
pthread_t terminalReader;
pthread_mutex_t clientsLock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Wrong arguments.\nargs:\n./server (TCP/UDP port) (UNIX socket path)\n");
        exit(1);
    }
    if (signal(SIGINT, exitHandler) == SIG_ERR) {
        printError("Exit signal fail!");
    }
    if (atexit(cleanUp) != 0) {
        printError("At exit failed!");
    }
    short portNum = validatePort((short) atoi(argv[1]));
    unixPath = validatePath(argv[2]);
    webSocket = getWebSocket(portNum);
    localSocket = getLocalSocket(unixPath);
    epoll = prepareMonitor();
    if (pthread_create(&pinger, NULL, pingerFunction, NULL) != 0) {
        printError("Creating thread failed!");
    }
    if (pthread_create(&terminalReader, NULL, terminalReaderFunction, NULL) != 0) {
        printError("Creating thread failed!");
    }
    struct epoll_event event;
    while (1) {
        if (epoll_wait(epoll, &event, 1, -1) == -1) {
            printError("Epoll_wait failed!");
        }
        handleNewMessage(event.data.fd);
    }
}

void handleNewMessage(int fd) {
    struct sockaddr *addr = malloc(ASIZE);
    socklen_t aSize = sizeof(struct sockaddr);
    Message message;
    if (recvfrom(fd, &message, sizeof(Message), 0, addr, &aSize) != sizeof(Message)) {
        printError("receiving new messageage failed!");
    }
    if (message.type == LOGIN) {
        registration(fd, addr, aSize, &message);
    } else if (message.type == RESULT) {
        int result = ntohl(message.result);
        printf("\" = %d (calculated by client %s)\n", result, message.name);
    } else if (message.type == PONG) {
        pthread_mutex_lock(&clientsLock);
        for (int i = 0; i < cN; i++) {
            if (strcmp(clients[i].name, message.name) == 0) {
                clients[i].rec++;
            }
        }
        pthread_mutex_unlock(&clientsLock);
    } else if (message.type == LOGOUT) {
        pthread_mutex_lock(&clientsLock);
        for (int i = 0; i < cN; i++) {
            if (strcmp(clients[i].name, message.name) == 0) {
                printf("Client %s logged out!\n", message.name);
                removeClient(i);
            }
        }
        pthread_mutex_unlock(&clientsLock);
    }
}

void registration(int fd, struct sockaddr *addr, socklen_t aSize, Message *mess) {
    char mType;
    pthread_mutex_lock(&clientsLock);
    if (cN == 50) {
        mType = FAILSIZE;
        if (sendto(fd, &mType, 1, 0, addr, aSize) != 1) {
            printError("Failsize message failed!");
        }
        free(addr);
    } else {
        Client *client = findClient(mess->name);
        if (client != NULL) {
            mType = FAILNAME;
            if (sendto(fd, &mType, 1, 0, addr, aSize) != 1) {
                printError("Failname message failed!");
            }
            free(addr);
        } else {
            clients[cN].addr = addr;
            clients[cN].name = concat("", mess->name);
            clients[cN].sent = 0;
            clients[cN].rec = 0;
            clients[cN].type = mess->connectType;
            clients[cN].aSize = aSize;
            cN++;
            mType = SUCCESS;
            if (sendto(fd, &mType, 1, 0, addr, aSize) != 1) {
                printError("Success message failed!");
            }
        }
    }
    pthread_mutex_unlock(&clientsLock);
}

void *pingerFunction(void *arg) {
    while (1) {
        //printf("ping!\n");
        pthread_mutex_lock(&clientsLock);
        int j = 0;
        for (int i = 0; i < cN; i++) {
            if (clients[j].sent != clients[j].rec) {
                printf("Client %s has been removed\n", clients[j].name);
                removeClient(j);
            } else {
                char mType = PING;
                int sfd = getSFD(clients[j].type);
                if (sendto(sfd, &mType, 1, 0, clients[j].addr, clients[j].aSize) != 1) {
                    printf("Error while sending PING to client %s\n", clients[j].name);
                }
                clients[j].sent++;
                j++;
            }
        }
        pthread_mutex_unlock(&clientsLock);
        sleep(1);
    }
    return NULL;
}

void removeClient(int i) {
    free(clients[i].addr);
    free(clients[i].name);
    for (int j = i; j + 1 < cN; j++) {
        clients[j] = clients[j + 1];
    }
    cN--;
}

void *terminalReaderFunction(void *arg) {
    char command;
    int op1, op2;
    char name[20];
    char buff[100];
    while (1) {
        printf("Enter command : arg1 math_command arg2 name_of_client\n");
        fgets(buff, 100, stdin);
        if (sscanf(buff, "%d %c %d %s\n", &op1, &command, &op2, name) != 4) {
            printf("Wrong args format of input!\n");
            continue;
        }
        if ((command != '+') && (command != '-') && (command != '/') && (command != '*')) {
            printf("Use command like : +, -, *, / !\n");
            continue;
        }

        int res = pushCommand(command, op1, op2, name);
        if (res == 0) {
            printf("Command %d: %d %c %d Has been sent to client %s!\n", counter - 1, op1, command, op2, name);
        } else if (res == -1) {
            printf("Command couldn't be sent. Client not found!\n");
        } else {
            printf("Command couldn't be sent.\n");
        }
    }
    return NULL;
}

int pushCommand(char command, int op1, int op2, char *name) {
    pthread_mutex_lock(&clientsLock);
    Client *client = findClient(name);
    if (client == NULL) {
        pthread_mutex_unlock(&clientsLock);
        return -1;
    }
    int sfd = getSFD(client->type);
    char mType = REQ;
    int currCtn = htonl(counter++);
    int ope1 = htonl(op1);
    int ope2 = htonl(op2);
    int ok = 0;
    if (sendto(sfd, &mType, 1, 0, client->addr, client->aSize) != 1) {
        ok = -2;
    }
    if (sendto(sfd, &command, 1, 0, client->addr, client->aSize) != 1) {
        ok = -2;
    }
    if (sendto(sfd, &currCtn, sizeof(int), 0, client->addr, client->aSize) != sizeof(int)) {
        ok = -2;
    }
    if (sendto(sfd, &ope1, sizeof(int), 0, client->addr, client->aSize) != sizeof(int)) {
        ok = -2;
    }
    if (sendto(sfd, &ope2, sizeof(int), 0, client->addr, client->aSize) != sizeof(int)) {
        ok = -2;
    }
    pthread_mutex_unlock(&clientsLock);
    return ok;
}

Client *findClient(char *name) {
    for (int i = 0; i < cN; i++) {
        if (strcmp(clients[i].name, name) == 0) {
            return &clients[i];
        }
    }
    return NULL;
}

int getSFD(char type) {
    if (type == LOCAL) {
        return localSocket;
    } else if (type == NETWORK) {
        return webSocket;
    }
    return -1;
}

int prepareMonitor() {
    int epoll = epoll_create1(0);
    if (epoll == -1) {
        printError("Error while creating epoll monitor");
    }
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLPRI;
    event.data.fd = webSocket;
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, webSocket, &event) == -1) {
        printError("Error of ctl for webSocket");
    }
    event.data.fd = localSocket;
    if (epoll_ctl(epoll, EPOLL_CTL_ADD, localSocket, &event) == -1) {
        printError("Error of ctl for localSocket");
    }
    return epoll;
}

int getWebSocket(short portNum) {
    struct sockaddr_in webAddress;
    webAddress.sin_family = AF_INET;
    webAddress.sin_port = htons(portNum);
    webAddress.sin_addr.s_addr = INADDR_ANY;
    int webSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (webSocket == -1) {
        printError("Error while creating webSocket");
    }
    if (bind(webSocket, &webAddress, sizeof(webAddress)) == -1) {
        printError("Error while webSocket binding");
    }
    printf("Server address is %s\n", inet_ntoa(webAddress.sin_addr));
    return webSocket;
}

int getLocalSocket(char *path) {
    struct sockaddr_un localAddress;
    localAddress.sun_family = AF_UNIX;
    for (int i = 0; i < strlen(path) + 1; i++) {
        localAddress.sun_path[i] = path[i];
    }
    int localSocket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (localSocket == -1) {
        printError("Error while creating localSocket");
    }
    if (bind(localSocket, &localAddress, sizeof(localAddress)) == -1) {
        printError("Error while localSocket binding");
    }
    return localSocket;
}

short validatePort(short port) {
    if ((port < 1024) || (port > 60999)) {
        printError("Port must be a number between 1024 and 60999!");
    }
    return port;
}

char *validatePath(char *path) {
    int l = strlen(path);
    if ((l < 1) || (l > UNIX_PATH_MAX)) {
        printf("Path must be of length between 1 and %d\n", UNIX_PATH_MAX);
        exit(1);
    }
    return path;
}

void exitHandler(int signo) {
    exit(1);
}

void cleanUp(void) {
    int allOk = 1;
    if (close(webSocket) == -1) {
        printf("Error while closing webSocket! Errno: %d, %s\n", errno, strerror(errno));
        allOk = 0;
    }
    if (close(localSocket) == -1) {
        printf("Error while closing localSocket! Errno: %d, %s\n", errno, strerror(errno));
        allOk = 0;
    }
    if (unlink(unixPath) == -1) {
        printf("Error while unlinking unixPath! Errno: %d, %s\n", errno, strerror(errno));
        allOk = 0;
    }
    if (close(epoll) == -1) {
        printf("Error while closing epoll! Errno: %d, %s\n", errno, strerror(errno));
        allOk = 0;
    }
    if (allOk == 1) {
        printf("All cleaned up!\n");
    }
}