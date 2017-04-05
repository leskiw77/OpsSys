#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>

#define LINE_SIZE 256

// dziala jak split w javie :
char ** getCommandsFromLine(char *line, int *size) {
    char *fline = (char *) malloc((strlen(line) - 1) * sizeof(char));

    int comSize = 1;
    for(int i=0;i<strlen(line)-1;i++) {
        fline[i] = line[i];
        if(fline[i]=='|') {
            comSize++;
        }
    }

    char * pch = strtok(fline, "|");
    char ** result = (char**)malloc(comSize*sizeof(char*));
    for(int i=0;i<comSize;i++) {
        result[i] = (char*)malloc((strlen(pch)+1)* sizeof(char));
        for(int j=0;j<strlen(pch);j++) {
            result[i][j] = pch[j];
        }
        result[i][strlen(pch)] = '\0';
        pch = strtok(NULL, "|");
    }
    *size = comSize;
    free(fline);
    return result;
}

bool isFinishCommand(char * command) {
    const char FINISH_COMMAND[2] = {'q', (char) 10};
    return strcmp(command, FINISH_COMMAND) == 0;
}

void freeCommands(char ** commands, int size){
    for(int i=0;i<size;i++){
        free(commands[i]);
    }
    free(commands);
}

void sendFromFirstToSecond(FILE * p1, FILE * p2, char * command) {

}



int main(int argc, char **argv) {
    FILE *p1, *p2;

    char FINISH_COMMAND[2] = {'q', (char) 10};
    char buffer[LINE_SIZE]; // potrzebny dla fgets
    char *line;
    int numOfCommands;

    while (1) {
        printf("Hello it's pipe interpreter : \ncommand> \n");
        line = fgets(buffer, LINE_SIZE, stdin);   // czytamy ze standardowego wejścia
        if (isFinishCommand(line)) {
            break;
        }

        char ** commands = getCommandsFromLine(line, &numOfCommands);

        for (int i = 0; i < numOfCommands-1 ; i++) {
            // commands[i] to komenda do wykonania :
            p1 = popen(commands[i], "r");
            p2 = fopen("result.txt", "a");
            printf("%i) %s\n",i,commands[i]);
            while (fgets(buffer, LINE_SIZE, p1)) {
                fputs(buffer, p2);
            }
            fclose(p1);
            fclose(p2);
        }

        freeCommands(commands,numOfCommands);
        printf("\n");
    }
    return 0;
}