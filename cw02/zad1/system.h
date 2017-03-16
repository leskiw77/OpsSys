//
// Created by jarema on 3/15/17.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#include <unistd.h>
#include <fcntl.h>

#ifndef LAB2_SYSTEM_H
#define LAB2_SYSTEM_H

void sysGenerate(char * fileName, int records, int bytes);

void sysSort(char * fileName, int records, int bytes);

void sysShuffle(char * fileName, int records, int bytes);
#endif //LAB2_SYSTEM_H
