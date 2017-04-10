#include "commands.h"

void executeLine(char *line, int length) {
    // split to words array :
    int cmdSize = 1;
    char *command;
    char **words = calloc(MAX_CMDS, sizeof(char *));
    words[0] = strtok(line, "|");
    while ((command = strtok(NULL, "|")) != NULL && cmdSize < MAX_CMDS) {
        words[cmdSize] = command;
        cmdSize++;
    }

    // parse commands (get command and argv) :
    CmdWithArgs **commands = calloc(MAX_CMDS, sizeof(CmdWithArgs *));
    for (int i = 0; i < cmdSize; i++) {
        commands[i] = getParsedCommand(words[i]);
    }

    int fd[2];
    int input;//= STDIN_FILENO; // read from stdin

    // execute n-1 commands :
    for (int i = 0; i < cmdSize ; i++) {
        pipe(fd);
        execCommand(commands[i], input, fd[PIPE_IN]); // exec and send result data to pipe
        close(fd[PIPE_IN]);
        input = fd[PIPE_OUT]; // then read from previously filled pipe
    }

    // execute last command :
    pid_t proc = fork();
    if (proc == 0) {  // child :
        dup2(input, STDIN_FILENO);
        //close(input);
        // execute :
        execvp(commands[cmdSize - 1]->cmd, commands[cmdSize - 1]->argv);
    } else {
        // parent :
        int status;
        wait(&status);
    }

    if (commands != NULL) {
        for (int i = 0; i < cmdSize; i++) {
            if (commands[i] != NULL) {
                free(commands[i]);
            }
        }
    }
    free(commands);
    free(words);
}

void execCommand(CmdWithArgs *command, int input, int output) {
    if (command == NULL) {
        fprintf(stderr, "CmdWithArgs cannot be NULL");
        exit(1);
    }
    pid_t pid = fork();
    if (pid == 0) {
        // child :

        // set pipes :
        dup2(input, STDIN_FILENO); //close(input);
        dup2(output, STDOUT_FILENO); //close(output);

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

CmdWithArgs *getParsedCommand(char *statement) {
    CmdWithArgs *result = (CmdWithArgs *) malloc(sizeof(CmdWithArgs));
    if (result == NULL) {
        fprintf(stderr, "Error while allocating memory");
        exit(1);
    }

    // parse and set command :
    result->cmd = strtok(statement, " \n\t");
    if (result->cmd == NULL) {
        fprintf(stderr, "%s\n", "Error because of wrong statement");
        exit(1);
    }

    // parse and set args :
    result->argv = calloc(MAX_ARGV_LEN, sizeof(char *));
    result->argv[0] = result->cmd;
    int i = 1;
    char *arg;
    while ((arg = strtok(NULL, " \n\t")) != NULL && i < MAX_ARGV_LEN) {
        result->argv[i] = arg;
        i++;
    }
    return result;
}
