#include "commons.h"

#define TIMEOUT 10

int initSockInternetIn();
int initSockIUnix();
void clean(int);

// sockaddr structures
struct sockaddr_in server_in;
struct sockaddr_un server_ux;
int sock_ux = 0;
int sock_in = 0;
int user_no = 0;

User * users[MAX_USERS];
fd_set readfds;// set of socket descriptors
char buffer[MAX_MSG_LENGTH]; // msg buffer
float getTimeDiff(struct timeval, struct timeval);// zwraca roznice czasow w sekundac
void reactOnClientsRequest(int, int);


int main(int argc, char **argv) {

    int opt = 1;

    signal(SIGINT, clean);

    // try to unlink
    unlink(SERVER_PATH);

    // init array with empty users
    for (int i = 0; i < MAX_USERS; i++) {
        users[i] = (User *) malloc(sizeof(User));
        users[i]->size = -1;
        users[i]->mode = -1;
        users[i]->socket = -1;
        //users[i]->confirmed = 0;
        users[i]->sockaddr = NULL;
    }

    // init sockets :
    sock_ux = initSockIUnix(SERVER_PATH);
    sock_in = initSockInternetIn(PORT_NO);

    // not necessary, but recommended
    if (setsockopt(sock_in, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
        printf("setsockopt(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    if (setsockopt(sock_ux, SOL_SOCKET, SO_REUSEADDR, (char *) &opt, sizeof(opt)) < 0) {
        printf("setsockopt(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    int sd, max_sd;
    int activity;

    printf("Waiting for clients...\n");
    while (1) {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(sock_in, &readfds);
        FD_SET(sock_ux, &readfds);
        max_sd = sock_in;

        //add child sockets to set
        for (int i = 0; i < MAX_USERS; i++) {
            sd = users[i]->socket; //socket descriptor

            //if valid socket descriptor then add to read list
            if (sd > 0) {
                FD_SET(sd, &readfds);
            }

            //highest file descriptor number, need it for the select function
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        //wait for an activity on one of the sockets , timeout is NULL , so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        // react on internet clients :
        if (FD_ISSET(sock_in, &readfds)) {
            reactOnClientsRequest(sock_in, MODE_INET);
        }

        // react on unix clients :
        if (FD_ISSET(sock_ux, &readfds)) {
            reactOnClientsRequest(sock_ux, MODE_UNIX);
        }
    }

    return 0;
}


int initSockInternetIn() {
    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        printf("socket(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    server_in.sin_family = AF_INET;
    server_in.sin_port = htons(PORT_NO);
    server_in.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock, (struct sockaddr *) &server_in, sizeof(server_in)) < 0) {
        printf("bind(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    printf("[server] inet socket created\n");
    return sock;
}


int initSockIUnix() {
    int sock;

    if ((sock = socket(PF_UNIX, SOCK_DGRAM, 0)) == -1) {
        printf("socket(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    server_ux.sun_family = AF_UNIX;
    strcpy(server_ux.sun_path, SERVER_PATH);

    if (bind(sock, (struct sockaddr *) &server_ux, SUN_LEN(&server_ux)) < 0) {
        printf("bind(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    printf("[server] unix socket created\n");
    return sock;
}


void clean(int arg) {
    close(sock_ux);
    close(sock_in);
    for (int i = 0; i < MAX_USERS; ++i) {
        if (users[i] != NULL) {
            free(users[i]->sockaddr);
            free(users[i]);
        }
    }
    unlink(SERVER_PATH);
    printf("[server] clean\n");
    exit(0);
}


// zwraca roznice czasow w sekundach
float getTimeDiff(struct timeval t0, struct timeval t1) {
    return (t1.tv_sec - t0.tv_sec);
}


void reactOnClientsRequest(int socket, int mode) {
    int recv_len;
    Message msg;

    struct sockaddr address;
    socklen_t addrlen = (socklen_t)(sizeof(address));
    if ((recv_len = recvfrom(socket, &msg, sizeof(msg), 0, &address, &addrlen)) < 0) {
        printf("recvfrom(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    // wypisuje co otrzymal od klienta :
    printf("msg: %s %s\n", msg.userName, msg.content);

    //inform user of socket number - used in send and receive commands
    if (mode == MODE_INET) {
        printf("New INET connection\n");
    } else if (mode == MODE_UNIX) {
        printf("New UNIX connection\n");
    }
    printf("UserID: %s, content: %s\n", msg.userName, msg.content);

    // check if user is logged
    bool logged = false;
    int usrIndex = -1;
    int i;
    for (i = 0; i < MAX_USERS; i++) {
        if (users[i]->socket != -1 && strcmp(users[i]->name, msg.userName) == 0) {
            logged = true;
            usrIndex = i;
            break;
        } else if (users[i]->socket == -1) {
            usrIndex = i;
        }
    }

    if (usrIndex == -1) {
        printf("[server] can't handle new connection - server is full\n");
        strcpy(msg.userName, "[server]");
        strcpy(msg.content, "server is full");
    } else {
        // add new user :
        if (!logged) {
            users[usrIndex]->mode = mode;
            strcpy(users[usrIndex]->name, msg.userName);
            users[usrIndex]->size = msg.size;
            users[usrIndex]->socket = socket;
            //users[usrIndex]->confirmed = 0;
            gettimeofday(&users[usrIndex]->time, NULL);
            users[usrIndex]->sockaddr = (struct sockaddr *) malloc(sizeof(struct sockaddr));

            memcpy(users[usrIndex]->sockaddr, &address, sizeof(address));

            printf("Adding to list of sockets as %d\n", usrIndex);
            strcpy(msg.userName, "[serv1er]");
            strcpy(msg.content, "User registered, type \"confirm\" to confirm.");

        } else {
            // send message to logged user :
            Message msg_to_sent;
            strcpy(msg_to_sent.userName, msg.userName);
            strcpy(msg_to_sent.content, msg.content);
            strcpy(msg.userName, "[server]");
            strcpy(msg.content, "result from server");

            // sending message :
            if (sendto(users[usrIndex]->socket, &msg_to_sent, sizeof(msg_to_sent), 0,
                       users[usrIndex]->sockaddr, users[usrIndex]->size) == -1) {
                printf("[server] can't send message to user#%d\n", i);
                printf("[server] deleting %d user\n", i);

                // delete
                users[usrIndex]->socket = -1;
                strcpy(users[usrIndex]->name, "-");
            }
        }
    }

    //send new connection message
    if (sendto(socket, &msg, sizeof(msg), 0, &address, addrlen) == -1) {
        printf("sendto(): %d: %s\n", errno, strerror(errno));
        exit(1);
    }

    printf("\n");
}