#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int counter = 0;
int L;

void incrementCounter(int signum) {
    counter++;
}

void reactSIGUSR2(int signum) {
    printf("\nParent : %d x SIGUSR1 send to child\n",L);
    printf("Parent :  1 x SIGUSR2 send to child\n");
    printf("\nParent : %d x SIGUSR1 received from child\n",counter);
    printf("Parent :  1 x SIGUSR2 received from child\n");
    exit(0);
}

void reactRT2(int signum) {
    printf("\nParent : %d x SIGRTMIN send to child\n",L);
    printf("\nParent : %d x SIGRTMIN received from child\n",counter);
    exit(0);
}

void sendNSignalsWithKillAndWait(int N, int signal1, int signal2) {
    pid_t child;

    // ? :
    sigset_t newSet, oldSet;
    sigaddset(&newSet, signal1);
    sigaddset(&newSet, signal2);
    sigprocmask(SIG_BLOCK, &newSet, &oldSet);

    // fork :
    child = fork();
    if (child == 0) {
        if (execl("./child", "child", NULL) == -1) {
            printf("Parent : execl error\n");
            exit(1);
        }
    } else if (child == -1) {
        printf("Parent : fork error\n");
        exit(1);
    }

    //wyslanie L sygnalow SIGNAL1 do child'a :
    for (int i = 0; i < N; i++) {
        if ((kill(child, signal1)) != 0) {
            printf("\nParent : kill error\n");
            exit(1);
        }
    }

    // wyslanie signal2 do child'a :
    if ((kill(child, signal2)) != 0) {
        printf("\nParent : kill error\n");
        exit(1);
    }

    while (1) {
        sigsuspend(&oldSet);
    }
}

int main(int argc, char **argv) {
    pid_t child;

    if (argc != 3) {
        printf("Wrong arguments\n");
        exit(1);
    }
    L = atoi(argv[1]);
    int type = atoi(argv[2]);

    struct sigaction sAct1;
    sAct1.sa_flags = 0;
    struct sigaction sAct2;
    sAct2.sa_flags = 0;

    if(type==1) {
        // typ1 : SIGUSR1, SIGUSR2 za pomocą funkcji kill

        // ustawienie sigaction :
        sAct1.sa_handler = incrementCounter;
        sigaddset(&(sAct1.sa_mask), SIGUSR1);

        sAct2.sa_handler = reactSIGUSR2;
        sigaddset(&(sAct2.sa_mask), SIGUSR2);

        if (sigaction(SIGUSR1, &sAct1, NULL) == -1 || sigaction(SIGUSR2, &sAct2, NULL) == -1) {
            printf("Parent : sigaction error\n");
            exit(1);
        }
        sendNSignalsWithKillAndWait(L,SIGUSR1,SIGUSR2);
    } else if(type==2) {
        // typ2 : SIGUSR1, SIGUSR2 za pomocą funkcji sigqueue

    } else if(type==3) {
        // typ3 : wybrane 2 sygnały czasu rzeczywistego za pomocą kill

        sAct1.sa_handler = incrementCounter;
        sigaddset(&(sAct1.sa_mask),SIGRTMIN+1);

        sAct2.sa_handler = reactRT2;
        sigaddset(&(sAct2.sa_mask),SIGRTMIN);

        if(sigaction(SIGRTMIN, &sAct1, NULL) == -1 || sigaction(SIGRTMIN+1, &sAct2, NULL) == -1) {
            printf("Blad sigaction (parent)\n");
            exit(1);
        }

        sigset_t newSet, oldSet;
        sigaddset(&newSet, SIGRTMIN); sigaddset(&newSet, SIGRTMIN+1);
        sigprocmask(SIG_BLOCK, &newSet, &oldSet);

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

        for(int i = 0; i < L; i++) {
            if((kill(child, SIGRTMIN)) != 0) {
                printf("Błąd funkcji kill (parent)\n");
                exit(1);
            }
        }
        if((kill(child, SIGRTMIN+1)) != 0) {
            printf("Błąd funkcji kill (parent)\n");
            exit(1);
        }
        
        while(1) sigsuspend(&oldSet);

    } else {
        printf("Wrong arguments\n");
        exit(1);
    }
    return 0;
}
