#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

//-------------------------------
int counter = 0;
void handler1(int);
void handler2(int);
//-------------------------------

int main(int argc, char **argv) {
    struct sigaction sa1, sa2;
    sa1.sa_handler = handler1;
    sa1.sa_flags = 0;
    sigaddset(&(sa1.sa_mask),SIGRTMIN+1);
    sa2.sa_handler = handler2;
    sa2.sa_flags = 0;
    sigaddset(&(sa2.sa_mask),SIGRTMIN);
    if(sigaction(SIGRTMIN, &sa1, NULL) == -1 || sigaction(SIGRTMIN+1, &sa2, NULL) == -1) {
        printf("Blad sigaction (child)\n");
        exit(1);
    }
    sigset_t empty;
    sigemptyset(&empty);
    while(1)
        sigsuspend(&empty);
    return 0;
}

void handler1(int signum){
    counter++;
}

void handler2(int signum){
    pid_t ppid = getppid();
    while(counter) {
        if((kill(ppid, SIGRTMIN)) != 0){
            printf("Błąd funkcji kill (child)!\n");
            exit(1);
        }
        counter--;
    }
    if(kill(ppid, SIGRTMIN+1) != 0){
        printf("Błąd funkcji kill (child)");
        exit(1);
    }
    exit(0);
}