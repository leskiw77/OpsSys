#include "configuration.h"

/*
 * Rodzaje zleceń

Usługa echa:
Klient wysyła ciąg znaków. Serwer odsyła ten sam ciąg z powrotem. Klient po odebraniu wysyła go na standardowe wyjście.
Usługa wersalików:
Klient wysyła ciąg znaków. Serwer zamienia małe litery na duże i odsyła zmieniony ciąg z powrotem. Klient po odebraniu wysyła go na standardowe wyjście.
Usługa czasu:
Po odebraniu takiego zlecenia serwer wysyła do klienta datę i godzinę w postaci łańcucha znaków. Klient po odebraniu informacji wysyła ją na standardowe wyjście.
Nakaz zakończenia:
Po odebraniu takiego zlecenia serwer zakończy działanie, jak tylko opróżni się kolejka zleceń (zostaną wykonane wszystkie pozostałe zlecenia).
Klient nie czeka na odpowiedź.
 */

//-----------------------------------------------------
struct client{
    mqd_t queue;
    char name[MAXNAME];
    int active;
};
//-----------------------------------------------------
mqd_t server;
struct mq_attr attr;
int working;
int numOfClients;
struct client clients[MAXCLIENTS];
//-----------------------------------------------------
void signalHandler(int signal);
int registerClient(char* name);
void sendToAll(char* text, char* sender);
//-----------------------------------------------------
int main(int argc, char* argv[]){
    if(argc != 1){
        printf("Blad w linni komend\n Poprawne uruchomienie programu to: nazwa programu \n");
        return 1;
    }

    working = 1;
    numOfClients = 0;

    if(signal(SIGTSTP, signalHandler) == SIG_ERR){
        printf("signal(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 3000;
    attr.mq_flags = O_NONBLOCK;

    if((server = mq_open("/server_queue", O_RDONLY | O_CREAT | O_NONBLOCK, S_IRUSR | S_IWUSR, &attr)) == -1){
        printf("mq_open(server): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    char message[4001];
    unsigned int priority;
    printf("SERWER DZIAŁA!\n");
    while(working){
        if(mq_receive(server, message, 4000, &priority) == -1){
            if(errno != EAGAIN){
                printf("mq_receive(): %d: %s\n", errno, strerror(errno));
                working = 0;
            }
        } else {
            char text[100];
            int firstSpace = 0;
            char *pch;
            pch = (char*) strchr(message, ' ');
            firstSpace = pch - message;
            strncpy(text, message, firstSpace);
            text[firstSpace] = '\0';

            if(strcmp(text, "registration") == 0){
                char text2[100];
                strncpy(text2, message + firstSpace + 1, strlen(message) - firstSpace + 1);
                if(registerClient(text2) < 0){
                    printf("Nie mozna dodac klienta\n");
                } else{
                    printf("Zarejestrowalem klienta %s\n", text2);
                    char logInText[100];
                    strcpy(logInText, text2);
                    strcat(logInText, " zalogowal sie!");
                    sendToAll(logInText, text2);
                }
            }
            else{//to nie jest registracja tylko wadomosc od uzytkwnika
                char text2[100];
                strncpy(text2, message, strchr(message, ' ') - message);
                text2[firstSpace] = '\0';
                sendToAll(message, text2);
            }
        }
        usleep(8000);
    }

    if(mq_unlink("/server_queue") < 0){
        printf("mq_unlink(): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    return 0;
}
//-----------------------------------------------------
void signalHandler(int signal){
    working = 0;
}
//-----------------------------------------------------
int registerClient(char* name){

    strcpy(clients[numOfClients].name, name);
    clients[numOfClients].active = 1;

    char queueName[110];
    sprintf(queueName, "/%s_queue", name);

    if((clients[numOfClients].queue = mq_open(queueName, O_WRONLY | O_NONBLOCK)) == -1){
        printf("mq_open(clientQueue): %d: %s\n", errno, strerror(errno));
        exit(-1);
    }

    numOfClients++;

    return 0;
}
//-----------------------------------------------------
void sendToAll(char* text, char* sender){
    int j;
    for(j = 0; j < numOfClients; ++j){
        if((strcmp(clients[j].name, sender) != 0) && (clients[j].active == 1)){
            if(mq_send(clients[j].queue, text, strlen(text)+1, 0) < 0){
                printf("mq_send(): %d: %s\n", errno, strerror(errno));
                working = 0;
            }
        }
    }
}