#include "configuration.h"

int clientId = -1;
int server;
int clientQueue;
char queueName[30];
char message[MAX_SIZE+1];

void sendFromBufferToServer(int type);
void exitHandler(int sig);
void clean();

int main(int argc, char *argv[]) {

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 1;

    signal(SIGINT, exitHandler);
    atexit(clean);

    // creating queue :
    sprintf(queueName, "/client%d", getpid());
    server = mq_open("/server", O_WRONLY, 0, &attr);
    clientQueue = mq_open(queueName, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attr);
    if (clientQueue == -1 || server == -1) {
        printf("Client or server queue opening error");
        exit(1);
    }

    // registration request :
    strcpy(message + 1, queueName);
    sendFromBufferToServer(NEW_CLIENT);

    // response :
    mq_receive(clientQueue, message, MAX_SIZE, 0);
    sscanf(message, "%d", &clientId);
    if (clientId == -1) {
        printf("Not registered error");
        exit(1);
    }
    printf("Registered with id = %d\n", clientId);

    char echoCommand[] = {'e', 'c', 'h', 'o', (char) 10};
    char toUpperCommand[] = {'u', 'p', 'p', 'e', 'r', (char) 10};
    char timeCommand[] = {'t', 'i', 'm', 'e', (char) 10};
    char exitCommand[] = {'e', 'x', 'i', 't', (char) 10};

    while (1) {

        char *cmd = NULL;
        size_t len = MAX_SIZE;
        printf("\nCommand >  ");
        getline(&cmd, &len, stdin);

        if (strcmp(cmd, echoCommand) == 0 || strcmp(cmd, toUpperCommand) == 0) {

            char *line = NULL;
            printf("\nLine >  ");

            getline(&line, &len, stdin);
            sprintf(message + 1, "%d %s", clientId, line);


            if (strcmp(cmd, echoCommand) == 0) {
                sendFromBufferToServer(ECHO);
            } else if (strcmp(cmd, toUpperCommand) == 0) {
                sendFromBufferToServer(TO_UPPER_CASE);
            } else {
                printf("Read line error\n : %s\n",line);
                exit(1);
            }

            mq_receive(clientQueue, message, MAX_SIZE, 0);
            printf("From server : %s\n", message);

        } else if (strcmp(cmd, timeCommand) == 0) {

            sprintf(message + 1, "%d", clientId);
            sendFromBufferToServer(GET_TIME);

            mq_receive(clientQueue, message, MAX_SIZE, 0);
            int year, month, day, hour, min, sec;
            sscanf(message, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &min, &sec);
            printf("From server : %d%d.%d%d.%d  %d%d:%d%d:%d%d\n", day / 10, day % 10, month / 10,
                   month % 10, year, hour / 10, hour % 10, min / 10, min % 10, sec / 10, sec % 10);


        } else if (strcmp(cmd, exitCommand) == 0) {
            break;
        } else {
            printf("Komenda %s nieznana\n", cmd);
        }
        sleep(1);
    }
    clean();
    return 0;
}

void sendFromBufferToServer(int type) {
    message[0] = type;
    mq_send(server, message, MAX_SIZE, 0);
}

void exitHandler(int sig) {
    printf("Server stopped\n");
    exit(0);
}

void clean() {
    message[0] = EXIT;
    sprintf(message + 1, "%d", clientId);
    mq_send(server, message, MAX_SIZE, 0);
    mq_close(server);
    mq_close(clientQueue);
    mq_unlink(queueName);
}