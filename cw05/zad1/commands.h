#ifndef OPSSYS_COMMANDS_H
#define OPSSYS_COMMANDS_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define PIPE_IN 1
#define PIPE_OUT 0

static const int MAX_CMDS = 20;
static const int MAX_ARGV_LEN = 5;

typedef struct Command Command;
struct Command{
    char * cmd;
    char **argv;
};

void executeLine(char*,int);
void execCommandInProcess(int,int,int,Command*);
Command *getParsedCommand(char*);
void deleteCommand(Command*);

#endif //OPSSYS_COMMANDS_H
