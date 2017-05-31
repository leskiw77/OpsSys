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

char to_string(operator o) {
	if(o == ADD) return '+';
	if(o == SUBTRACT) return '-';
	if(o == MULTIPLY) return '*';
	if(o == DIVIDE) return '/';
	return '\0';
}

void parse_args(int argc, char** argv) {
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
	}
	else if(strcmp(argv[2], "local") == 0) {
		type = LOCAL;
		path = malloc(strlen(argv[3]) + 1);
		strcpy(path, argv[3]);
	}
	else {
		printf("Wrong arguments.\nUsage:\n./client name network/local address [port]\n");
		exit(1);
	}
}

void calculate(operation *o) {
	if(o -> op == ADD) o -> arg3 = o -> arg1 + o -> arg2;
	if(o -> op == SUBTRACT) o->arg3 = o->arg1 - o->arg2;
	if(o -> op == MULTIPLY) o->arg3 = o->arg1 * o->arg2;
	if(o -> op == DIVIDE) o->arg3 = o->arg1 / o->arg2;
}

void exit_handle(int s) {
	operation o;
	o.op = EXIT;
	write(fd, (void*)&o, sizeof(o));
	shutdown(fd, SHUT_RDWR);
	exit(0);
}

int main(int argc, char** argv) {
	parse_args(argc, argv);
	signal(SIGINT, exit_handle);
    char buf[1024];
    
	operation op; 
	
	if(type == NETWORK) {
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
    
	op.op = NAME;
	strcpy(op.name, name);
	write(fd, (void*)&op, sizeof(operation)); //send name to server
	read(fd, buf, sizeof(operation));
	op = *(operation*)buf;
	if(op.op == DENY) {
		printf("Access to server denied.\n");
		exit(1);
	}
	else {
		printf("Access to server received.\n");
	}
	while(1) { //actual client work
		read(fd, buf, sizeof(operation));
		op = *(operation*)buf;
		strcpy(op.name, name);
		if(op.op == ADD || op.op == MULTIPLY || op.op == SUBTRACT || op.op == DIVIDE) {
			calculate(&op);
			printf("%d%c%d calculated.\n", op.arg1, to_string(op.op), op.arg2);
		}
		write(fd, (void*)&op, sizeof(operation));
	}
    return 0;
}
