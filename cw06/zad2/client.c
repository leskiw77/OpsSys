#include "configuration.h"

int client_id = -1;
int server;
int client_queue;
char queue_name[30];

void handler(int sig) {
    printf("Server stopped\n");
    exit(0);
}

void clean() {
    char message[MAX_SIZE];
    message[0] = EXIT;
    sprintf(message + 1, "%d", client_id);
    mq_send(server, message, MAX_SIZE, 0);
    mq_close(server);
    mq_close(client_queue);
    mq_unlink(queue_name);
}

int main(int argc, char *argv[]) {

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 1;

    signal(SIGINT, handler);
    atexit(clean);

    // utworzenie kolejki :
    sprintf(queue_name, "/client%d", getpid());
    server = mq_open("/server", O_WRONLY, 0, &attr);
    client_queue = mq_open(queue_name, O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attr);
    if (client_queue == -1 || server == -1) {
        printf("Client or server queue opening error");
        exit(1);
    }

    // wysłanie informacji do serwera o dołaczeniu klienta :
    char message[MAX_SIZE];
    message[0] = NEW_CLIENT;
    strcpy(message + 1, queue_name);
    if (mq_send(server, message, MAX_SIZE, 0) < 0) {
        printf("Sending first message error\n");
        exit(1);
    }
    if (mq_receive(client_queue, message, MAX_SIZE, 0) < 0) {
        printf("Receiving first message error\n");
        exit(1);
    }

    sscanf(message + 1, "%d", &client_id);
    if (client_id == -1) {
        printf("Not registered error");
        exit(1);
    }
    printf("Registered with id = %d\n",client_id);

    char lineBuffer[MAX_SIZE];
    while (1) {

        char * line = NULL;
        size_t len = MAX_SIZE;
        printf("\nCommand >  ");
        int n = getline(&line, &len, stdin);

        message[0] = TO_UPPER_CASE;
        sprintf(message + 1, "%d %s", client_id, line);
        mq_send(server, message, MAX_SIZE, 0);
        mq_receive(client_queue, message, MAX_SIZE, 0);

        char buffer[MAX_SIZE];
        sscanf(message + 1, "%s", buffer);
        printf("From server : %s\n", buffer);

        sleep(1);
    }
}