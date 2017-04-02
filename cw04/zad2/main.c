#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

int k,n;
int counter=0;
int finished = 0;
pid_t * activePidArr;
pid_t * allPidArr;



void finalSygnal(int sig, siginfo_t *info, void *secret) {
    finished++;
    printf("Finished process pid=%d signal=%d value=%d\n",info->si_pid,info->si_signo,info->si_value.sival_int);
    if(finished == n){
        exit(0);
    }

}

void abortChildrenProcesses(int sig){
    for (int i = 0; i < n; ++i) {
        kill(allPidArr[i],SIGABRT);
    }
    exit(2);
}
void income_signal_counter(int sig, siginfo_t *info, void *secret) {

    counter++;

    printf("Parent received signal=%d from process pid=%d \n",info->si_signo,info->si_pid);

    if(counter == k){

        for (int i = 0; i < k-1; ++i) {
            kill(activePidArr[i],SIGCONT);
        }
        kill(info->si_pid,SIGCONT);

    }
    else if(counter>k){
        kill(info->si_pid,SIGCONT);

    }
    else{
        activePidArr[counter-1]=info->si_pid;

    }
    printf("\n");

}

void childCont(int sig)
{
    printf("Process %d continued\n",getpid());
}

int childProcess(){

    signal(SIGCONT,childCont);

    //working
    FILE * randFile = fopen("/dev/random", "r");
    unsigned int randNum;
    fread(&randNum, sizeof(unsigned int), 1, randFile);
    fclose(randFile);
    //sleep(randNum%10);

    clock_t start = clock();
    kill(getppid(),SIGUSR1);
    pause();
    clock_t end = clock();

    union sigval value;
    value.sival_int = end - start;

    sigqueue(getppid(), SIGRTMIN + randNum%(SIGRTMAX-SIGRTMIN), value);
    return (int)(end-start);
}

int main(int argc, char **argv)
{

    if (argc != 3) {
        printf("Wrong arguments\n");
        exit(1);
    }

    n = atoi(argv[1]);
    k = atoi(argv[2]);

    if(n<=0 || k<=0 || k>n){
        printf("Wrong arguments\n");
        exit(1);
    }

    activePidArr=(pid_t *)malloc(n*sizeof(pid_t));
    allPidArr=(pid_t *)malloc(n*sizeof(pid_t));
    int i;


    struct sigaction sAct1;
    sAct1.sa_sigaction = income_signal_counter;
    sAct1.sa_flags = SA_SIGINFO;

    struct sigaction sAct2;
    sAct2.sa_sigaction = finalSygnal;
    sAct2.sa_flags = SA_SIGINFO;

    for (int i = SIGRTMIN; i <= SIGRTMAX ; ++i) {
        if(sigaction(i, &sAct2, NULL) == -1){
            printf("Parent : sigaction error\n");
            exit(1);
        }
    }

    if (sigaction(SIGUSR1, &sAct1, NULL) == -1 ) {
        printf("Parent : sigaction error\n");
        exit(1);
    }

    signal(SIGINT,abortChildrenProcesses);




    for (i = 0; i < n; ++i) {
        if ((allPidArr[i] = fork()) < 0) {
            perror("fork");
            abort();
        } else if (allPidArr[i] == 0) {
            return childProcess();
        }
    }


    while (1) {
        ;
    }

    return 0;
}