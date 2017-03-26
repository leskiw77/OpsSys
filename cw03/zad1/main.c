#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_PARAMS 10
#define MAX_LENGTH 10

//Zeby działo w konsoli musisz zrobic export PATH=$PATH:. albo export PATH=$PATH:$(pwd)
int main(int argc, char ** argv) {


    if(argc != 2) {
        printf("Invalid amount of arguments\n");
        return 1;
    }

    char * pch;
    char * comands [MAX_PARAMS+1];
    comands[MAX_PARAMS]=NULL;
    for (int j = 0; j < MAX_PARAMS; ++j) {
        comands[j] = NULL;
    }


    FILE * fp = fopen(argv[1], "r");

    if (fp == NULL){
        printf("Dupa");
        return 2;
    }

    char * line = (char *)malloc(sizeof(char) * MAX_LENGTH * MAX_PARAMS); //possibly damaging
    size_t len = 0;
    pid_t pid;

    while (getline(&line, &len, fp) != -1) {
        int i=0;
        if(line[strlen(line)-1]=='\n')
            line[strlen(line)-1]='\0';

        pch = strtok (line," ");
        while (pch != NULL && i <MAX_PARAMS)
        {
            comands[i] = (char *)malloc(sizeof(char) * MAX_LENGTH);
            strcpy(comands[i],pch);
            i++;
            pch = strtok (NULL, " ");
        }

        char * tmp1 = comands[0];
        char * tmp2 = comands[1];
        switch(comands[0][0]) {

            case '#' :
                if(tmp2 == NULL){
                    unsetenv(tmp1+1);
                } else{
                    setenv(tmp1+1,tmp2,1);
                }

                break;

            default :
                //for test only
                printf("check environmental variable TestFile = %s\n\n",getenv("TestFile"));

                if ((pid = fork()) == -1) {
                    perror("fork error");
                }
                else if (pid == 0) {
                    if(execvp(comands[0],comands) != 0){
                        printf("Error occurred while running %s command.\n",comands[0]);
                        exit(1);
                    }
                } else {
                    int status;
                    if ((pid = wait(&status)) == -1)
                        perror("wait error");
                    else if (WEXITSTATUS(status) != 0) {//value returned by child
                        exit(1);
                    }
                }
        }

        for (int j = 0; j < i; ++j) {
            free(comands[j]);
            comands[j]=0;
        }

    }

    for (int j = 0; j < MAX_PARAMS; ++j) {
        free(comands[j]);
    }

    fclose(fp);
    free(line);

}

