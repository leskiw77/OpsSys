#ifndef COMMONS_H
#define COMMONS_H

#define MAX_MSG_LENGTH 256
#define MAX_NAME_LENGTH 72
#define MAX_USERS 32

#define MODE_UNIX 0
#define MODE_INET 1
#define REQ_LOGIN 0
#define REQ_LOGOUT 1
#define REQ_SEND 2

#define PORT_NO 9044
#define SERVER_PATH "./socket"

#include <sys/resource.h>
#include <sys/sysinfo.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <pthread.h>
#include <unistd.h>
#include <syslog.h>
#include <signal.h>
#include <sys/un.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdio.h>
#include <sys/time.h>

typedef struct User {
	int mode;
	int confirmed;
	char name[MAX_NAME_LENGTH];
	int socket;
	struct timeval time;
} User;

typedef struct Message {
	char name[MAX_NAME_LENGTH];
	char content[MAX_MSG_LENGTH];
} Message;

#endif
