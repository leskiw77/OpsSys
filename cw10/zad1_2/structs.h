#ifndef UTILS_H
#define UTILS_H

#define MAX_CLIENTS 32
#define MAX_NAME_LENGTH 32

typedef enum operator {
	ADD = 1,
	SUBTRACT = 2,
	MULTIPLY = 3,
	DIVIDE = 4,
	PING = 5,
	NAME = 6,
	EXIT = 7,
	ACCEPT = 8,
	DENY = 9
} operator;

typedef struct operation {
	int arg1;
	int arg2;
	int arg3;
	char name[MAX_NAME_LENGTH];
	operator op;
} operation;


typedef struct client {
	char name[MAX_NAME_LENGTH];
	int fd;
} client;


#endif
