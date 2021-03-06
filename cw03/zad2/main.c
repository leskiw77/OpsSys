#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <time.h>

#define MAX_PARAMS 20
#define MAX_LENGTH 20

//Zeby działo w konsoli musisz zrobic export PATH=$PATH:. albo export PATH=$PATH:$(pwd)
int main(int argc, char **argv) {

    printf("\nRESULTS : \n");
    if (argc != 4) {
        printf("Invalid amount of arguments\n");
        return 1;
    }

    const long long timeLimitValue =  atoll(argv[2]);
    const long long sizeLimitValue =  atoll(argv[3]) * 1024 * 1024;

    char *pch;
    char *comands[MAX_PARAMS + 1];
    comands[MAX_PARAMS] = NULL;
    for (int j = 0; j < MAX_PARAMS; ++j) {
        comands[j] = NULL;
    }

    FILE *fp = fopen(argv[1], "r");

    if (fp == NULL) {
        printf("Error while opening file");
        return 1;
    }

    char *line = (char *) malloc(sizeof(char) * MAX_LENGTH * MAX_PARAMS);
    size_t len = 0;
    pid_t pid;
    struct rusage usage;

    // dla kazdej linii z pliku wsadowego :
    while (getline(&line, &len, fp) != -1) {
        int i = 0;
        if (line[strlen(line) - 1] == '\n') {
            line[strlen(line) - 1] = '\0';
        }

        pch = strtok(line, " "); // split
        // dla kazdej komendy z linii :
        while (pch != NULL && i < MAX_PARAMS) {
            comands[i] = (char *) malloc(sizeof(char) * MAX_LENGTH);
            strcpy(comands[i], pch);
            i++;
            pch = strtok(NULL, " ");
        }

        char *tmp1 = comands[0];
        char *tmp2 = comands[1];

        // sprawdzamy czy mamy do czynienia z komenda czy operacja na zmiennej srodowiskowej :
        switch (comands[0][0]) {
            case '#' : {
                if (tmp2 == NULL) {
                    // usuwamy zmienna srodowiskowa
                    unsetenv(tmp1 + 1);
                } else {
                    // ustawiamy zmienna srodowiskowa o nazwie tmp1 (bez #) o oraz wartosci z tmp2
                    setenv(tmp1 + 1, tmp2, 1);
                }
            }
                break;

            default :
                pid = fork();
                if(pid==0) {
                    printf("\n-----------------------\n"
                                   "Command : %s \n",comands[0]);

                    struct rlimit sLimit;
                    rlim_t maxSize = (rlim_t)sizeLimitValue;
                    sLimit.rlim_cur = maxSize/2;
                    sLimit.rlim_max = maxSize;
                    if( setrlimit(RLIMIT_AS,&sLimit) == -1 ) {
                        perror("Setting limit for memeory failed");
                        exit(1);
                    }

                    struct rlimit tLimit;
                    rlim_t maxTime = (rlim_t)timeLimitValue;
                    tLimit.rlim_cur = 1;
                    tLimit.rlim_max = maxTime;
                    if( setrlimit(RLIMIT_CPU,&tLimit) == -1 ) {
                        perror("Setting limit for cpu time failed");
                        exit(1);
                    }

                    if(execvp(comands[0],comands) == -1){
                        printf("Error occurred while running %s command.\n",comands[0]);
                        exit(1);
                    }

                } else if(pid > 0) {
                    int status;
                    wait3(&status,0,&usage);
                    if (WIFEXITED(status) != 1 || WEXITSTATUS(status) != 0) {
                        fprintf(stderr,"Failed, wstatus = %i\n",status);
                        exit(1);
                    } else {
                        printf("\nTimes : system = %li , users = %li\n",
                               usage.ru_stime.tv_usec,
                               usage.ru_utime.tv_usec);
                    }
                } else {
                    // nie udalo sie utworzyc procesu potomnego :
                    perror("fork error");
                    exit(1);
                }
        }
        for (int j = 0; j < i; ++j) {
            free(comands[j]);
            comands[j] = 0;
        }
    }

    for (int j = 0; j < MAX_PARAMS; ++j) {
        free(comands[j]);
    }

    fclose(fp);
    free(line);
    printf("\n-----------------------\n");
    return 0;
}

