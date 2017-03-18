#ifndef OPSSYS_LOCKS_H
#define OPSSYS_LOCKS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>

bool setLock(int fd, int byteNumber, char type);
bool resetLock(int fd, int byteNumber);
bool printLocks(int fd);

#endif //OPSSYS_LOCKS_H
