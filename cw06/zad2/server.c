#include "configuration.h"


int queueDesc;                   // obecna kolejka
static int clients[MAX_CLIENTS]; // lista klientow
char message[MAX_SIZE + 1];      // bufor na wiadomosc

/***********************************************/

void init_clients();
int nextId();
void exitHandler(int sig);
void clean();
int numberOfUsers();
void sendFromBufferToClient(int queueId);

/***********************************************/

int main(int argc, char *argv[]) {

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 1;

    // open queue :
    queueDesc = mq_open("/server", O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attr);
    if (queueDesc == -1) {
        perror("open queue");
        exit(1);
    }

    signal(SIGINT, exitHandler);
    atexit(clean);
    init_clients();

    printf("SERVER WORKS\n");
    while (1) {
        if (mq_receive(queueDesc, message, MAX_SIZE, 0) >= 0) {
            printf("\nFrom client : %s\n", message + 1);
            long type = message[0];
            switch (type) {

                case NEW_CLIENT: {
                    int newId = nextId();
                    int clQueue = -1;

                    // create new queue :
                    if (newId != -1) {
                        clQueue = mq_open(message + 1, O_WRONLY, 0, &attr);
                        clients[newId] = clQueue;
                        if (clQueue == -1) {
                            newId = -1;
                        }
                    }
                    printf("SERVER : new client with id = %d\n", newId);
                    printf("SERVER : now we have %d users\n", numberOfUsers());

                    // resend queue id to client :
                    sprintf(message, "%d", newId);
                    sendFromBufferToClient(clQueue);
                    break;
                }

                case ECHO: {
                    int clientId;
                    char buffer[MAX_SIZE];
                    // get from client :
                    sscanf(message + 1, "%d %s", &clientId, buffer);

                    // send to client :
                    sprintf(message, "%s", buffer);
                    sendFromBufferToClient(clients[clientId]);
                    break;
                }

                case TO_UPPER_CASE : {
                    int clientId;
                    char buffer[MAX_SIZE];
                    // get from client :
                    sscanf(message + 1, "%d %s", &clientId, buffer);


                    for (int i = 0; i < MAX_SIZE; i++)
                        buffer[i] = toupper(buffer[i]);

                    // send to client :
                    sprintf(message, "%s", buffer);
                    sendFromBufferToClient(clients[clientId]);
                    break;
                }

                case GET_TIME : {
                    int clientId;
                    //char buffer[MAX_SIZE];
                    struct tm * tInfo;
                    time_t my_time;
                    sscanf(message + 1, "%d %s", &clientId);
                    time (&my_time);
                    tInfo = localtime(&my_time);

                    sprintf(message, "%d-%d-%d  %d:%d:%d", 1900+(tInfo->tm_year),
                            tInfo->tm_mon,tInfo->tm_mday,tInfo->tm_hour,tInfo->tm_min,tInfo->tm_sec);
                    sendFromBufferToClient(clients[clientId]);
                    break;
                }

                case EXIT: {
                    int client_id;
                    sscanf(message + 1, "%d", &client_id);
                    mq_close(clients[client_id]);
                    clients[client_id] = -1;
                    printf("SERVER : client with id = %d  has exit\n", client_id);
                    printf("SERVER : now we have %d users\n", numberOfUsers());
                    break;
                }

                default: {
                    break;
                }
            }
        }
    }
}

void init_clients() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i] = -1;
    }
}

int nextId() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] == -1) {
            return i;
        }
    }
    return -1;
}

void exitHandler(int sig) {
    exit(0);
}


void clean() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i] != -1) {
            mq_close(clients[i]);
        }
    }
    mq_close(queueDesc);
    mq_unlink("/server");
}

int numberOfUsers() {
    int result = 0;
    for(int i=0;i<MAX_CLIENTS;i++) {
        if(clients[i] != -1) {
            result++;
        }
    }
    return result;
}


void sendFromBufferToClient(int queueId) {
    if (mq_send(queueId, message, MAX_SIZE, 0)) {
        printf("SERVER : send error");
        exit(1);
    }
}