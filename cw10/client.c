#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include <signal.h>
#include <sys/un.h>
#include "structs.h"

#define NETWORK 0
#define LOCAL 1

int fd = -1;
char name[MAX_NAME_LENGTH];
char* address;
char* path;
int port;
int type;

char toString(Operator o);
void calculate(Operation *o);
void exit_handle(int s);

int main(int argc, char** argv) {
    // PARSE ARGUMENTS :
    if(argc < 4) {
        printf("Wrong arguments.\n args:\n ./client name network/local address [port]\n");
        exit(1);
    }
    strcpy(name, argv[1]);

    if(strcmp(argv[2], "network") == 0) {
        type = NETWORK;
        address = malloc(strlen(argv[3]) + 1);
        strcpy(address, argv[3]);
        port = atoi(argv[4]);
    } else if(strcmp(argv[2], "local") == 0) {
        type = LOCAL;
        path = malloc(strlen(argv[3]) + 1);
        strcpy(path, argv[3]);
    } else {
        printf("Wrong arguments.\nUsage:\n./client name network/local address [port]\n");
        exit(1);
    }

	signal(SIGINT, exit_handle);
    char buffer[1024];
	Operation oper;

	if(type == NETWORK) {
        // SET INTERNET CONNECTION:
		struct sockaddr_in addr;
		fd = socket(AF_INET, SOCK_STREAM, 0);
    	if(fd < 0) {
        	printf("Error while creating socket.\n");
        	exit(1);
    	} 
	
    	addr.sin_family = AF_INET;
    	addr.sin_port = htons(port); 

    	if(inet_pton(AF_INET, address, &addr.sin_addr) <= 0) {
        	printf("Error while transforming addres.\n");
        	exit(1);
    	} 
		if(connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) < 0) {
      	 	printf("Error while connecting to sever.\n");
			exit(1);
    	}
	}

	else {
        // SET UNIX CONNECTION :
		struct sockaddr_un addr;
		fd = socket(AF_UNIX, SOCK_STREAM, 0);
		if(fd < 0) {
        	printf("Error while creating socket.\n");
        	exit(1);
    	}
		addr.sun_family = AF_UNIX;
		strcpy(addr.sun_path, path);
		if(connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) < 0) {
			printf("Error while connecting to sever.\n");
			exit(1);
    	}
	}

    // SEND REGISTRATION REQUEST :
	oper.op = NAME;
	strcpy(oper.name, name);
	write(fd, (void*)&oper, sizeof(Operation));

    // REGISTRATION RESPONSE :
	read(fd, buffer, sizeof(Operation));
	oper = *(Operation*)buffer;
	if(oper.op == DENY) {
		printf("Access to server denied.\n"); exit(1);
	}
	else {
		printf("Access to server received.\n");
	}

    // WORKING LOOP FOR SINGLE UNIT OF THE CLUSTER :
	while(1) {
		read(fd, buffer, sizeof(Operation));
		oper = *(Operation*)buffer;
		strcpy(oper.name, name);
        // RECEIVED MATH REQUEST :
		if(oper.op == ADD || oper.op == MULTIPLY || oper.op == SUBTRACT || oper.op == DIVIDE) {
			calculate(&oper);
			printf("%d%c%d calculated.\n", oper.arg1, toString(oper.op), oper.arg2);
		}
		write(fd, (void*)&oper, sizeof(Operation));
	}
    return 0;
}

char toString(Operator o) {
    if(o == ADD) return '+';
    if(o == SUBTRACT) return '-';
    if(o == MULTIPLY) return '*';
    if(o == DIVIDE) return '/';
    return '\0';
}

void calculate(Operation *o) {
    if (o->op == ADD) {
        o->result = o->arg1 + o->arg2;
    } else if (o->op == SUBTRACT) {
        o->result = o->arg1 - o->arg2;
    } else if (o->op == MULTIPLY) {
        o->result = o->arg1 * o->arg2;
    } else if (o->op == DIVIDE) {
        o->result = o->arg1 / o->arg2;
    }
}

// SEND EXIT REQUEST TO SERVER AND EXIT PROCESS :
void exit_handle(int s) {
    Operation o;
    o.op = EXIT;
    write(fd, (void*)&o, sizeof(o));
    shutdown(fd, SHUT_RDWR);
    exit(0);
}