#include <stdio.h>

#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int k,n,counter;
int received = 0;
pid_t * pidArray;


void bt_sighandler(int sig, siginfo_t *info, void *secret) {

    sigset_t set;

    sigset_t base_mask, waiting_mask;

    sigemptyset (&base_mask);
    sigaddset (&base_mask, SIGUSR1);

    sigprocmask (SIG_SETMASK, &base_mask, NULL);

    sigpending (&waiting_mask);

    while (sigismember (&waiting_mask, SIGUSR1)) {
        counter++;

        printf("pid=%d signal=%d\n",info->si_pid,info->si_signo);
        printf("counder %d\n",counter);
    }

    printf("pid=%d signal=%d\n",info->si_pid,info->si_signo);
    printf("counder %d\n",counter);
    /*
    if(counter == k){

        for (int i = 0; i < k-1; ++i) {
            printf("send %d\n",pidArray[i]);
            kill(pidArray[i],SIGCONT);
        }
        kill(info->si_pid,SIGCONT);

    }
    else if(counter == n-1){
        kill(info->si_pid,SIGCONT);
        printf("to juz koniec\n");
        exit(17);
    }
    else if(counter>k){
        printf("Bedziemy słac jedno\n");
        kill(info->si_pid,SIGCONT);

    }
    else{
        printf("agregujemy\n");
        pidArray[counter-1]=info->si_pid;

    }
    printf("\n");
*/
}


void childP(int sig)
{
    printf("Pid dziecka %d\n",getpid());

}

long childProcess(){
    printf("%d\n",getppid());
    signal(SIGCONT,childP);
    sleep(3);
    clock_t start = clock();
    kill(getppid(),SIGUSR1);
    pause();
    clock_t end = clock();
    printf("kontunuacja %ld \n",(long)(end-start));
    //kill(getppid(),SIGRTMIN + rand()%(SIGRTMAX-SIGRTMIN));
    return (long)(end-start);
}

int main()
{
    srand(time(NULL));
    counter=0;

    pid_t pid;
    n=10;k=5;
    pidArray=(pid_t *)malloc(n*sizeof(pid_t));
    int i;

    struct sigaction sa;

    sa.sa_sigaction = (void *)bt_sighandler;
    sigemptyset (&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &sa, NULL);


    for (i = 0; i < n; ++i) {
        if ((pid = fork()) < 0) {
            printf("tutut");
            perror("fork");
            abort();
        } else if (pid == 0) {
            sleep(1);
            printf("tak\n");
            childProcess();
            pause();
            printf("dziecko\n");
            return 1;
        }
    }
    printf("Dupa\n");

    while(1){
        //printf("%d %d %d\n",counter,k,n);
        continue;
    }

    return 0;
}