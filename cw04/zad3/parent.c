#include <stdio.h>
#include <stdlib.h> //atoi
#include <unistd.h> //fork
#include <signal.h>

//-------------------------------
int counter = 0;
int N;
void handler1(int);
void handler2(int);
//-------------------------------

int main(int argc, char **argv) {
    int i;
    pid_t child;

    if(argc != 2){
        printf("Niepoprawna ilość parametrów wywołania!\n");
        exit(1);
    }
    N = atoi(argv[1]);

    struct sigaction sa1, sa2;
    sa1.sa_handler = handler1;
    sa1.sa_flags = 0;
    sigaddset(&(sa1.sa_mask),SIGRTMIN+1);
    //podczas obsługi sygnału SIGRTMIN
    //ignorujemy sygnał SIGRTMIN+1

    sa2.sa_handler = handler2;
    sa2.sa_flags = 0;
    sigaddset(&(sa2.sa_mask),SIGRTMIN);
    //analogicznie

    if(sigaction(SIGRTMIN, &sa1, NULL) == -1 || sigaction(SIGRTMIN+1, &sa2, NULL) == -1) {
        printf("Blad sigaction (parent)\n");
        exit(1);
    }

    sigset_t new, old;
    sigaddset(&new, SIGRTMIN);
    sigaddset(&new, SIGRTMIN+1);
    sigprocmask(SIG_BLOCK, &new, &old);

    child = fork();
    if (child == -1) {
        printf("Błąd funkcji fork! (parent)\n");
        exit(1);
    } else if(child == 0) {
        if(execl("./child", "child", NULL) == -1){
            printf("Błąd funkcji execl!\n");
            exit(1);
        }
    }

    for(i = 0; i < N; i++) {
        if((kill(child, SIGRTMIN)) != 0) {
            printf("Błąd funkcji kill (parent)\n");
            exit(1);
        }
    }
    if((kill(child, SIGRTMIN+1)) != 0) {
        printf("Błąd funkcji kill (parent)\n");
        exit(1);
    }
    while(1) sigsuspend(&old);

    return 0;
}

void handler1(int signum){
    counter++;
}

void handler2(int signum){
    printf("\n%d sygnałów SIGUSR1 zostało wysłanych do child\n"
                   "%d sygnałów SIGUSR1 zostało odebranych przez parent\n",
           N, counter);
    exit(0);
}