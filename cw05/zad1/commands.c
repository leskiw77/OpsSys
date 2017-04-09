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
    int in = STDIN_FILENO;

    // execute n-1 commands :
    for (int i = 0; i < cmdSize - 1; i++) {
        pipe(fd);
        execCommandInProcess(i, in, fd[PIPE_IN], commands[i]);
        close(fd[PIPE_IN]);
        in = fd[PIPE_OUT];
    }

    // execute last command :
    pid_t last = fork();
    if (last == 0) {
        // child :
        if (in != STDIN_FILENO) {
            dup2(in, STDIN_FILENO);
            close(in);
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

void execCommandInProcess(int index, int in, int out, Command *command) {
    if(command==NULL) {
        fprintf(stderr,"Command cannot be NULL");
        exit(1);
    }
    pid_t pid = fork();
    // TO DO : check it :
    if (pid == 0) {
        // child :
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

    // get command :
    result->cmd = strtok(statement," \n\t");
    if (result->cmd == NULL) {
        fprintf(stderr, "%s\n","Error because of wrong statement");
        exit(1);
    }

    // get args :
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
