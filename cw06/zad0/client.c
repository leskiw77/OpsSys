#include "configuration.h"
//-----------------------------------------------------
mqd_t server;
mqd_t client;
int working;
pid_t pid;
pid_t child;
char* name;
struct mq_attr attr;
unsigned int priority;
//-----------------------------------------------------
void signalHandler(int signal);
//-----------------------------------------------------
int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Blad w linni komend\n Poprawne uruchomienie programu to: nazwa programu [nazwa uzytkownika]\n");
        return 1;
    }
    name = argv[1];
    working = 1;

    if(signal(SIGTSTP, signalHandler) == SIG_ERR){
        printf("signal(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    char message[4001];
    pid = getpid();

    //Rejestracja kolejki serwera
    if((server = mq_open("/server_queue", O_WRONLY)) == -1){
        printf("mq_open(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    //Rejestracja kolejki klienta
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 3000;
    attr.mq_flags = O_NONBLOCK;

    char queueName[110];
    sprintf(queueName, "/%s_queue", name);

    if((client = mq_open(queueName, O_RDONLY | O_CREAT, S_IRUSR | S_IWUSR, &attr)) == -1){
        printf("mq_open(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    //Rejestrowanie do serwera
    char message2[1000];
    sprintf(message2, "registration %s", name);
    if(mq_send(server, message2, strlen(message2) + 1, 0) < 0){
        printf("mq_send(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    printf("Do zakmniecia nacisnij: CRTL + Z\n\n");

    if((child = fork()) < 0){
        printf("fork(): %d: %s\n", errno, strerror(errno));
        working = 0;
    } else if(child > 0){ //Proces czytający
        while(working){
            usleep(500000);
            if(mq_receive(client, message, 4000, NULL) < 0){
                if(errno != EAGAIN){
                    printf("mq_receive(): %d: %s\n", errno, strerror(errno));
                    working = 0;
                }
            } else{
                printf("%s\n\n", message);
            }
        }
        exit(0);
    } else if(child == 0){ //Proces piszący
        while(working){
            char ch = '\n';
            int i;
            for(i = 0; i < MAXMSGSIZE && (ch = fgetc(stdin)) != '\n'; ++i){
                message[i] = ch;
            }
            message[i] = (char) 0;

            if(strcmp(message, "exit") == 0){
                working = 0;
                //kill(getppid(), SIGTSTP);
                kill(getppid(), SIGTSTP);
            }

            time_t tm;
            tm = time(NULL);
            char result[5000];
            sprintf(result, "%s <%s>  ", name, ctime(&tm));
            strcat(result, message);

            if(mq_send(server, result, strlen(result) + 1, 0) < 0){
                printf("mq_send(): %d: %s\n", errno, strerror(errno));
                working = 0;
            }
        }
    }


    mq_unlink(queueName);
    mq_close(server);
    return 0;
}
//-----------------------------------------------------
void signalHandler(int signal){
    working = 0;
    kill(child, SIGTSTP);
}
