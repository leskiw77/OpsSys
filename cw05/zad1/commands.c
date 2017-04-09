#include "commands.h"

void executeLine(char *line, int length) {
    // split to words array :
    int cmdSize = 1;
    char *command;
    char **words = calloc(MAX_CMDS,sizeof(char *));
    words[0] = strtok(line, "|");
    while ((command = strtok(NULL, "|")) != NULL && cmdSize < MAX_CMDS) {
        words[cmdSize] = command;
        cmdSize++;
    }

    // parse commands (get command and argv) :
    Command **commands = calloc(MAX_CMDS, sizeof(Command *));
    for (int i = 0; i < cmdSize; i++) {
        commands[i] = getParsedCommand(words[i]);
    }

    int fd[2];
    int input = STDIN_FILENO; // read from stdin

    // execute n-1 commands :
    for (int i = 0; i < cmdSize - 1; i++) {
        pipe(fd);
        execCommandInProcess(i, input, fd[PIPE_IN], commands[i]); // exec and send result data to pipe
        close(fd[PIPE_IN]);
        input = fd[PIPE_OUT]; // then read from previously filled pipe
    }

    // execute last command :
    pid_t proc = fork();
    if (proc == 0) {
        // child :
        if (input != STDIN_FILENO) {
            dup2(input, STDIN_FILENO);
            close(input);
        }
        // execute :
        execvp(commands[cmdSize - 1]->cmd, commands[cmdSize - 1]->argv);
    } else {
        // parent :
        int status;
        wait(&status);
    }

    for (int i = 0; i < cmdSize; i++)
        deleteCommand(commands[i]);
    free(commands);
    free(words);
}

// TO DO : check it :
void execCommandInProcess(int index, int input, int output, Command *command) {
    if(command==NULL) {
        fprintf(stderr,"Command cannot be NULL");
        exit(1);
    }
    pid_t pid = fork();
    if (pid == 0) {
        // child :

        // set pipes :
        if (index == 0) {
            // if first one , set output as stdout :
            if (output != STDOUT_FILENO) {
                dup2(output, STDOUT_FILENO);
                close(output);
            }
        } else {
            if (input != STDIN_FILENO) {
                dup2(input,STDIN_FILENO);
                close(input);
            }
            if (output != STDOUT_FILENO) {
                dup2(output, STDOUT_FILENO);
                close(output);
            }
        }
        // execute command with args:
        if (execvp(command->cmd, command->argv) == -1) {
            fprintf(stderr, "Error while executing : %s\n", command->cmd);
            exit(1);
        }
    } else {
        // parent :
        int status;
        wait(&status);
    }
}

Command *getParsedCommand(char *statement) {
    Command *result = (Command*)malloc(sizeof(Command));
    if (result == NULL) {
        fprintf(stderr, "Error while allocating memory");
        exit(1);
    }

    // parse and set command :
    result->cmd = strtok(statement," \n\t");
    if (result->cmd == NULL) {
        fprintf(stderr, "%s\n","Error because of wrong statement");
        exit(1);
    }

    // parse and set args :
    result->argv = calloc(MAX_ARGV_LEN, sizeof(char *));
    result->argv[0] = result->cmd;
    int i = 1;
    char *arg;
    while ((arg = strtok(NULL," \n\t")) != NULL && i < MAX_ARGV_LEN) {
        result->argv[i] = arg;
        i++;
    }
    return result;
}

void deleteCommand(Command *cmd) {
    if(cmd != NULL) {
        free(cmd->argv);
        free(cmd);
    }
}
