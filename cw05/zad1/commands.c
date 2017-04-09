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
    Command **cmdArray = calloc(MAX_CMDS, sizeof(Command *));
    for (int i = 0; i < cmdSize; i++) {
        cmdArray[i] = getParsedCommand(words[i]);
    }

    // ? :
    int p[2];
    int in = STDIN_FILENO;
    for (int i = 0; i < cmdSize - 1; i++) {
        pipe(p);
        executeCmd(i, in, p[PIPE_IN], cmdArray[i]);
        close(p[PIPE_IN]);
        in = p[PIPE_OUT];
    }

    pid_t last = fork();
    if (last == 0) {
        if (in != STDIN_FILENO) {
            dup2(in, STDIN_FILENO);
            close(in);
        }
        execvp(cmdArray[cmdSize - 1]->cmd, cmdArray[cmdSize - 1]->argv);
    } else {
        int status;
        wait(&status);
    }

    for (int i = 0; i < cmdSize; i++)
        deleteCommand(cmdArray[i]);
    free(cmdArray);
    free(words);
}

// ? :
void executeCmd(int index, int in, int out, Command *command) {
    if(command==NULL) {
        fprintf(stderr,"Command cannot be NULL!!");
        exit(1);
    }
    pid_t pid = fork();
    if (pid == 0) {
        // proces potomny :
        if (index == 0) {
            if (out != STDOUT_FILENO) {
                dup2(out, STDOUT_FILENO);
                close(out);
            }
        } else {
            if (in != STDIN_FILENO) {
                dup2(in, STDIN_FILENO);
                close(in);
            }
            if (out != STDOUT_FILENO) {
                dup2(out, STDOUT_FILENO);
                close(out);
            }
        }
        if (execvp(command->cmd, command->argv) == -1) {
            fprintf(stderr, "Error while executing : %s\n", command->cmd);
            exit(1);
        }
    } else {
        // proces macierzysty :
        int status;
        wait(&status);
    }
}

//
Command *getParsedCommand(char *line) {
    Command *result = (Command*)malloc(sizeof(Command));
    if (result == NULL) {
        fprintf(stderr, "Error while allocating memory");
        exit(1);
    }

    // get command :
    result->cmd = strtok(line," \n\t");
    if (result->cmd == NULL) {
        fprintf(stderr, "%s\n","Error : wrong statement");
        exit(1);
    }

    // get args :
    result->argv = calloc(MAX_ARGV_LEN, sizeof(char *));
    result->argv[0] = result->cmd;
    int argCount = 1;
    char *arg;
    while ((arg = strtok(NULL," \n\t")) != NULL && argCount < MAX_ARGV_LEN) {
        result->argv[argCount] = arg;
        argCount++;
    }
    return result;
}

void deleteCommand(Command *cmd) {
    if(cmd != NULL) {
        free(cmd->argv);
        free(cmd);
    }
}
