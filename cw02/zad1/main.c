#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/times.h>
#include <time.h>
#include <sys/resource.h>

#include "library.h"
#include "system.h"


void timeValuesToFile(struct rusage *, struct rusage *, char ** );

int main(int argc, char **argv) {
    srand(time(NULL));

    clock_t startCLK = clock();

    if(argc != 6){
        printf("Invalid amount of arguments");
        return 1;
    }

    int records = atoi(argv[4]);
    int bytes = atoi(argv[5]);


    if(records <= 0 || bytes <= 0){
        printf("Invalid numeric argumentent");
        return 2;
    }


    struct rusage start, end;
    getrusage(RUSAGE_SELF, &start);



    if(strcmp(argv[1],"sys") == 0){

        if(strcmp(argv[2],"generate") == 0){
            sysGenerate(argv[3],records,bytes);
        } else if(strcmp(argv[2],"shuffle") == 0){
            sysShuffle(argv[3],records,bytes);
        } else if(strcmp(argv[2],"sort") == 0){
            sysSort(argv[3],records,bytes);
        } else{
            printf("Invalid second argument. Expected generate, shuffle or sort instead %s",argv[2]);
        }
    }

    else if(strcmp(argv[1],"lib") == 0){

        if(strcmp(argv[2],"generate") == 0){
            libGenerate(argv[3],records,bytes);
        } else if(strcmp(argv[2],"shuffle") == 0){
            libShuffle(argv[3],records,bytes);
        } else if(strcmp(argv[2],"sort") == 0){
            libSort(argv[3],records,bytes);
        } else{
            printf("Invalid second argument. Expected generate, shuffle or sort instead %s",argv[2]);
        }
    } else{
        printf("Invalid first argument. Expected sys or lib instead %s",argv[1]);
        return 3;
    }

    getrusage(RUSAGE_SELF, &end);
    timeValuesToFile(&start, &end,argv);

    printf("%ld\n", clock() - startCLK);
    return 0;
}


void timeValuesToFile(struct rusage *start, struct rusage *end, char **argv){

    FILE *results;
    if ((results = fopen("results", "a+")) == NULL) {
        printf("Blad otwierania pliku\n");
        exit(1);
    }


    long userTime = (end->ru_utime.tv_usec)-(start->ru_utime.tv_usec);
    long sysTime  = (end->ru_stime.tv_usec)-(start->ru_stime.tv_usec);

    printf("%s %s records:%s bytes:%s\nuser : %ld , sys : %ld  [us]\n\n",argv[1],argv[2],argv[4],argv[5],userTime,sysTime);

    if(strcmp(argv[2],"generate") != 0)
        fprintf(results,"%s %s records:%s bytes:%s\nuser : %ld , sys : %ld  [us]\n\n",argv[1],argv[2],argv[4],argv[5],userTime,sysTime);

    fclose(results);
}
