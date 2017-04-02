#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

int counter = 0;

void resend(int signum) {
    counter++;
    pid_t ppid = getppid();
    if ((kill(ppid, SIGUSR1)) != 0) {
        printf("\nChild : kill error\n");
        exit(1);
    }
}

void resendRT(int signum) {
    counter++;
    pid_t ppid = getppid();
    if ((kill(ppid, SIGRTMIN)) != 0) {
        printf("\nChild : kill SIGRTMIN error\n");
        exit(1);
    }
}

void finish(int signum) {
    pid_t ppid = getppid();
    if (kill(ppid, SIGUSR2) != 0) {
        printf("\nChild : kill error\n");
        exit(1);
    }
    printf("\nChild : %d x SIGUSR1 received and resend to parent\n", counter);
    printf("Child : 1 x SIGUSR2 received and resend to parent\n");
    exit(0);
}

void finishRT(int signum) {
    pid_t ppid = getppid();
    if (kill(ppid, SIGRTMIN + 1) != 0) {
        printf("\nChild : kill SIGRTMIN+1 error\n");
        exit(1);
    }
    printf("\nChild : %d x SIGRTMIN received and resend to parent\n", counter);
    printf("Child : 1 x SIGRTMIN+1 received and resend to parent\n");
    exit(0);
}


int main(int argc, char **argv) {
    struct sigaction sAct1;
    sAct1.sa_handler = resend;
    sAct1.sa_flags = 0;
    sigaddset(&(sAct1.sa_mask), SIGUSR2);

    struct sigaction sAct2;
    sAct2.sa_handler = finish;
    sAct2.sa_flags = 0;
    sigaddset(&(sAct2.sa_mask), SIGUSR1);

    struct sigaction sAct3;
    sAct3.sa_handler = resendRT;
    sAct3.sa_flags = 0;
    sigaddset(&(sAct3.sa_mask), SIGRTMIN + 1);

    struct sigaction sAct4;
    sAct4.sa_handler = finishRT;
    sAct4.sa_flags = 0;
    sigaddset(&(sAct4.sa_mask), SIGRTMIN);

    // ustawienie sigactions :
    if (sigaction(SIGUSR1, &sAct1, NULL) == -1 || sigaction(SIGUSR2, &sAct2, NULL) == -1 ||
        sigaction(SIGRTMIN, &sAct3, NULL) == -1 || sigaction(SIGRTMIN + 1, &sAct4, NULL) == -1) {
        printf("\nChild : sigaction error\n");
        exit(1);
    }

    sigset_t sigset;
    sigemptyset(&sigset);
    while (1) {
        sigsuspend(&sigset);
    }
    return 0;
}
