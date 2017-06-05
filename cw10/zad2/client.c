#define _GNU_SOURCE
#define gettid() syscall(SYS_gettid)

#include "helpers.h"

void exitHandler(int signo);

void cleanUp(void);

char *nameValidation(char *name);
int typeValidation(char *type);
char *pathValidation(char *path);
int addressValidation(char *ip);
short validatePort(short port);
int connectLocal(char *path);
int connectNetwork(int address, short port);
void handleRequest();
void sendMessage(char type, int ctn, int result);
void registerClient(char *name, int sfd);
int sfd; // socket file descriptor
char *name;
char connectType;

int main(int argc, char **argv) {
    if ((argc != 4) && (argc != 5)) {
        printf("Wrong arguments.\nargs:\n ./client name network/local address [port]\n");
        exit(1);
    }
    name = nameValidation(argv[1]);
    int type = typeValidation(argv[2]);
    if (signal(SIGINT, exitHandler) == SIG_ERR) {
        printError("Signal failed!");
    }
    if (atexit(cleanUp) != 0) {
        printError("At exit failed!");
    }
    if ((type == LOCAL) || (argc == 4)) {
        char *path = pathValidation(argv[3]);
        sfd = connectLocal(path);
    } else if ((type == NETWORK) || (argc == 5)) {
        int address = addressValidation(argv[3]);
        short port = validatePort((short) atoi(argv[4]));
        sfd = connectNetwork(address, port);
    } else {
        printf("Wrong arguments.\nargs:\n ./client name network/local address [port]\n");
        exit(1);
    }
    registerClient(name, sfd);
    while (1) {
        char mType;
        if (read(sfd, &mType, 1) != 1) {
            printError("CLIENT: reading type failed!");
        }
        if (mType == PING) {
            sendMessage(PONG, 0, 0);
        } else if (mType == REQ) {
            handleRequest();
        }
    }
}

void handleRequest() {
    char command;
    int ctn, op1, op2, result;
    if (read(sfd, &command, 1) != 1) {
        printError("CLIENT: reading command failed!");
    }
    if (read(sfd, &ctn, sizeof(int)) != sizeof(int)) {
        printError("CLIENT: reading ctn failed!");
    }
    if (read(sfd, &op1, sizeof(int)) != sizeof(int)) {
        printError("CLIENT: reading op1 failed!");
    }
    if (read(sfd, &op2, sizeof(int)) != sizeof(int)) {
        printError("CLIENT: reading op2 failed!");
    }
    op1 = ntohl(op1);
    op2 = ntohl(op2);
    switch (command) {
        case '+':
            result = op1 + op2;
            break;
        case '-':
            result = op1 - op2;
            break;
        case '/':
            result = op1 / op2;
            break;
        case '*':
            result = op1 * op2;
            break;
        default:
            printf("CLIENT : wrong command");
            result = 0;
            break;
    }
    result = htonl(result);
    sendMessage(RESULT, ctn, result);
}

void sendMessage(char type, int ctn, int result) {
    Message mess;
    mess.type = type;
    for (int i = 0; i < 21; i++) mess.name[i] = 0;
    for (int i = 0; i < strlen(name); i++) {
        mess.name[i] = name[i];
    }
    mess.ctn = ctn;
    mess.result = result;
    mess.connectType = connectType;
    if (write(sfd, &mess, sizeof(Message)) != sizeof(Message)) {
        printError("CLIENT: sending message failed!");
    }
}

void registerClient(char *name, int sfd) {
    sendMessage(LOGIN, 0, 0);
    char messageType;
    printf("Waiting for login response...\n");
    if (read(sfd, &messageType, 1) != 1) {
        printError("Receiving login response failed!");
    }
    if (messageType == FAILSIZE) {
        printf("Too many clients, couldnt log in!\n");
        exit(2);
    } else if (messageType == FAILNAME) {
        printf("Name already in use, couldnt log in!\n");
        exit(2);
    } else if (messageType == SUCCESS) {
        printf("Logged in successfully!\n");
    } else {
        printError("Unpredicted behaviour in registerClient!");
    }
}

int connectLocal(char *path) {
    struct sockaddr_un localAddress;
    localAddress.sun_family = AF_UNIX;
    for (int i = 0; i < strlen(path) + 1; i++) {
        localAddress.sun_path[i] = path[i];
    }
    int localSocket = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (localSocket == -1) {
        printError("CLIENT: creating localSocket failed!");
    }
    if (bind(localSocket, &localAddress, sizeof(sa_family_t)) == -1) {
        printError("local abstract binding failed!");
    }
    if (connect(localSocket, &localAddress, sizeof(localAddress)) == -1) {
        printError("CLIENT: localSocket connecting failed!");
    }
    connectType = LOCAL;
    return localSocket;
}

int connectNetwork(int address, short port) {
    int webSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (webSocket == -1) {
        printError("Creating webSocket failed!");
    }
    struct sockaddr_in webAddress;
    webAddress.sin_family = AF_INET;
    webAddress.sin_port = 0;
    webAddress.sin_addr.s_addr = INADDR_ANY;
    if (bind(webSocket, &webAddress, sizeof(webAddress)) == -1) {
        printError("CLIENT: webSocket binding failed!");
    }
    webAddress.sin_family = AF_INET;
    webAddress.sin_port = htons(port);
    webAddress.sin_addr.s_addr = htonl(address);
    if (connect(webSocket, &webAddress, sizeof(webAddress)) == -1) {
        printError("CLIENT: webSocket connecting failed!");
    }
    connectType = NETWORK;
    return webSocket;
}

char *nameValidation(char *name) {
    int l = strlen(name);
    if ((l < 2) || (l > 20)) {
        printError("Name must be of length between 2 and 20");
    }
    return name;
}

int typeValidation(char *type) {
    if (strcmp(type, "local") == 0) {
        return LOCAL;
    } else if (strcmp(type, "network") == 0) {
        return NETWORK;
    } else {
        printError("Type must be local or network!");
        return -1;
    }
}

char *pathValidation(char *path) {
    int l = strlen(path);
    if ((l < 1) || (l > UNIX_PATH_MAX)) {
        printf("Path must be of length between 1 and %d\n", UNIX_PATH_MAX);
        exit(1);
    }
    return path;
}

int addressValidation(char *ip) {
    int x = inet_addr(ip);
    if (x == -1) {
        printError("CLIENT: inet_addr failed! Adrees is wrong");
    }
    return x;
}

short validatePort(short port) {
    if ((port < 1024) || (port > 60999)) {
        printError("Port must be a number from [1024, ... , 60999]");
    }
    return port;
}

void exitHandler(int signo) {
    sendMessage(LOGOUT, 0, 0);
    exit(1);
}

void cleanUp(void) {
    int allOk = 1;
    if (close(sfd) == -1) {
        printf("Error while closing client socket! Errno: %d, %s\n", errno, strerror(errno));
        allOk = 0;
    }
    if (allOk == 1) {
        printf("All closed down!\n");
    }
}
