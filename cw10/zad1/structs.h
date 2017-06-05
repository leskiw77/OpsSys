#define MAX_CLIENTS 16
#define MAX_NAME_LENGTH 32

typedef enum Operator {
	ADD = 1,
	SUBTRACT = 2,
	MULTIPLY = 3,
	DIVIDE = 4,
	PING = 5,
	NAME = 6,
	EXIT = 7,
	ACCEPT = 8,
	DENY = 9
} Operator;

typedef struct Operation {
	int arg1;
	int arg2;
	int result;
	char name[MAX_NAME_LENGTH];
	Operator op;
} Operation;


typedef struct client {
	char name[MAX_NAME_LENGTH];
	int fd;
} client;
