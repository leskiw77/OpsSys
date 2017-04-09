#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "main.h"

static const int MAX_CMD_COUNT = 20;
static const int MAX_ARGV_LEN = 5;
static const char *PIPE_DELIMITER = "|";
static const char *WSPACE_DELIMITER = " \n\t";
static const char *ERROR_INVALID_PIPE= "Entered incorrect combination of commands and pipe signs";

int main(int argc, char *argv[]){
    char *line = NULL;
    size_t size = 0;
    printf("Pipe interpreter\n");
    while(1){
        // read line from stdin :
        printf("Command > \n");
        ssize_t charsRead = getline(&line, &size, stdin);
        if(charsRead != -1){
            processLine(line, charsRead);
        }
        else{
            printf("Finished reading lines from stdin\n");
            if(line != NULL)
                free(line);
            exit(0);
        }
    }
    return 0;
}

void processLine(char *line, ssize_t length){
    char **lineArray = calloc(MAX_CMD_COUNT, sizeof(char *));
    Command **cmdArray = calloc(MAX_CMD_COUNT, sizeof(Command*));
    char *command;
    int cmdCount = 1;

    lineArray[0] = strtok(line, PIPE_DELIMITER);
    while((command = strtok(NULL, PIPE_DELIMITER)) != NULL && cmdCount < MAX_CMD_COUNT){
        lineArray[cmdCount++] = command;
    }

    for(int i = 0; i < cmdCount; i++){
        cmdArray[i] = parseCommand(lineArray[i]);
    }

    int pipeLine[2];
    int in = STDIN_FILENO;
    for(int i = 0; i < cmdCount -1; i++){
        pipe(pipeLine);
        executeCmd(i, in, pipeLine[TARGET], cmdArray[i]);
        close(pipeLine[TARGET]);
        in = pipeLine[SOURCE];
    }

    pid_t last = fork();
    if(last == 0){
        if(in != STDIN_FILENO){
            dup2(in, STDIN_FILENO);
            close(in);
        }
        execvp(cmdArray[cmdCount-1]->cmd, cmdArray[cmdCount-1]->argv);
    }
    else{
        int status;
        wait(&status);
    }

    for(int i = 0; i < cmdCount; i++)
        Command_delete(cmdArray[i]);
    free(cmdArray);
    free(lineArray);
}

void executeCmd(int index, int in, int out, Command *command){
    assert(command != NULL);
    pid_t pid = fork();
    if(pid == 0){
        if(index == 0){
            if(out != STDOUT_FILENO){
                dup2(out, STDOUT_FILENO);
                close(out);
            }
        }
        else{
            if(in != STDIN_FILENO) {
                dup2(in, STDIN_FILENO);
                close(in);
            }
            if(out != STDOUT_FILENO){
                dup2(out, STDOUT_FILENO);
                close(out);
            }
        }
        if(execvp(command->cmd, command->argv) == -1){
            fprintf(stderr, "Could not execute command '%s': %s\n", command->cmd, strerror(errno));
            exit(1);
        }

    }
    else{
        int status;
        wait(&status);
    }
}

Command *parseCommand(char *line){
    Command *cmd = Command_new();
    cmd->cmd = strtok(line, WSPACE_DELIMITER);
    if(cmd->cmd == NULL){
        fprintf(stderr, "%s\n", ERROR_INVALID_PIPE);
        exit(1);
    }

    int argCount = 1;
    char *arg;
    cmd->argv = calloc(MAX_ARGV_LEN, sizeof(char *));
    cmd->argv[0] = cmd->cmd;
    while((arg = strtok(NULL, WSPACE_DELIMITER)) != NULL && argCount < MAX_ARGV_LEN){
        cmd->argv[argCount++] = arg;
    }

    return cmd;
}

Command *Command_new(char *line){
    Command *newCommand = malloc(sizeof(Command));
    if(newCommand == NULL){
        fprintf(stderr, "Couldn't allocate memory for new command: %s\n", strerror(errno));
        exit(1);
    }
    return newCommand;
}

void Command_delete(Command *cmd){
    assert(cmd != NULL);
    free(cmd->argv);
    free(cmd);
}