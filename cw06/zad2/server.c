#include "configuration.h"

/*

Usługa echa:
 Klient wysyła ciąg znaków.
 Serwer odsyła ten sam ciąg z powrotem.
 Klient po odebraniu wysyła go na standardowe wyjście.

Usługa wersalików:
  Klient wysyła ciąg znaków.
  Serwer zamienia małe litery na duże i odsyła zmieniony ciąg z powrotem.
  Klient po odebraniu wysyła go na standardowe wyjście.

Usługa czasu:
  Po odebraniu takiego zlecenia serwer wysyła do klienta datę i godzinę w postaci łańcucha znaków.
  Klient po odebraniu informacji wysyła ją na standardowe wyjście.

Nakaz zakończenia:
  Po odebraniu takiego zlecenia serwer zakończy działanie, jak tylko opróżni
  się kolejka zleceń (zostaną wykonane wszystkie pozostałe zlecenia).
  Klient nie czeka na odpowiedź.

 */


// obecna kolejka :
int queueDesc;

// lista klientow :
static int clients[MAXCLIENTS];

void init_clients() {
    for (int i = 0; i < MAXCLIENTS; i++) {
        clients[i] = -1;
    }
}

int get_id() {
    for (int i = 0; i < MAXCLIENTS; i++) {
        if (clients[i] == -1) {
            return i;
        }
    }
    return -1;
}

void handler(int sig) {
    exit(0);
}


void clean() {
    for (int i = 0; i < MAXCLIENTS; i++) {
        if (clients[i] != -1) {
            mq_close(clients[i]);
        }
    }
    mq_close(queueDesc);
    mq_unlink("/server");
}


int main(int argc, char *argv[]) {

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 1;
    char message[MAX_SIZE + 1];

    // open queue :
    queueDesc = mq_open("/server", O_CREAT | O_RDONLY, S_IRUSR | S_IWUSR, &attr);
    if (queueDesc == -1) {
        perror("open queue");
        exit(1);
    }

    signal(SIGINT, handler);
    atexit(clean);
    init_clients();

    printf("SERVER WORKS\n");
    while (1) {
        if (mq_receive(queueDesc, message, MAX_SIZE, 0) >= 0) {
            printf("From client : %s\n",message);
            long type = message[0];
            switch (type) {
                // nowy klient :
                case NEW_CLIENT: {
                    int new_id = get_id();
                    int cl_q = -1;
                    // create new queue :
                    if (new_id != -1) {
                        cl_q = mq_open(message + 1, O_WRONLY, 0, &attr);
                        clients[new_id] = cl_q;
                        if (cl_q == -1) {
                            new_id = -1;
                        }
                    }
                    printf("SERVER : new client with id = %d\n",cl_q);
                    // resend queue id to client :
                    message[0] = SERACCLIENT;
                    sprintf(message + 1, "%d", new_id);
                    if (mq_send(cl_q, message, MAX_SIZE, 0)) {
                        printf("SERVER : send error");
                        exit(1);
                    }
                    break;
                }
                case GET_ID: {
                    int clientId;
                    sscanf(message + 1, "%d", &clientId);
                    sprintf(message + 1, "%d", 123);
                    if (mq_send(clients[clientId], message, MAX_SIZE, 0) < 0) {
                        printf("SERVER : send error");
                    }
                    break;
                }

                // echo :
                case ECHO : {
                    printf("Echo\n");
                    int mode;
                    int clientId;
                    int c;
                    //char buffer[MAX_SIZE];
                    // odbierz od klienta :
                    sscanf(message, "%d %d %d", &mode, &clientId, &c);

                    // odeslij tekst z buffera :
                    sprintf(message, "%d", 123);
                    if (mq_send(clients[clientId], message, MAX_SIZE, 0) < 0) {
                        printf("SERVER : send error");
                    }
                    break;
                }

                // echo + to upper case
                case TO_UPPER_CASE : {
                    printf("To upper case\n");
                    break;
                }

                // time :
                case GET_TIME : {
                    printf("Gettime\n");
                    break;
                }

                // exit :
                case EXIT : {
                    printf("Close client\n");
                    break;
                }

                default: {
                    break;
                }
                 /*
                case CLRESP: {
                    int clientid;
                    int number;
                    int isprime;
                    sscanf(message + 1, "%d %d %d", &clientid, &number, &isprime);
                    if (isprime) {
                        printf("Liczba pierwsza: %d klient %d\n", number, clientid);
                    }
                    break;
                }

                case CLSCLNT: {
                    int client_id;
                    sscanf(message + 1, "%d", &client_id);
                    mq_close(clients[client_id]);
                    clients[client_id] = -1;
                    break;
                }
                default:
                    break;
                    */
            }
        }
    }
}

