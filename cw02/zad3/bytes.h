#ifndef OPSSYS_BYTES_H
#define OPSSYS_BYTES_H
#include "locks.h"

bool writeByte(int fd, int byteNumber, char byteValue);
bool readByte(int fd, int byteNumber, char *value);

#endif //OPSSYS_BYTES_H
