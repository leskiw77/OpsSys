#include "commons.h"
//------------------------------------------------------------------------------
typedef struct threadParams {
   int socket;
   Message msg;
} threadParams;
//------------------------------------------------------------------------------
char* CLIENT_ID;
int type = 0;	// 0 - unix, 1 - internet
//------------------------------------------------------------------------------
void clean(int);
void* run(void*);
int getInternetSocket(char*, int);
int getUnixSocket(char*);
//------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
  if(argc != 3) {
		printf("usage: %s [CLIENT_ID] [-u | -i]\n", argv[0]);
		return 0;
	}

  signal(SIGINT, clean);

	CLIENT_ID = argv[1];
  int sock;
  char buffer[MAX_NAME_LENGTH];


  if(strcmp(argv[2], "-u") == 0) {
    //c = 'u';
    type = 0;
    sock = getUnixSocket(SERVER_PATH);

  } else if (strcmp(argv[2], "-i") == 0) {
    type = 1;
    printf("\nINET> Enter server address: ");
    fgets(buffer, 99, stdin);
    buffer[strlen(buffer)] = '\0';

    // get internet socket
    sock = getInternetSocket(buffer, PORT_NO);
  } else {
    printf("Wrong socket type\n"
            "usage: [-u | -i]\n");
    exit(-1);
  }

	Message msg;
	strcpy(msg.name, CLIENT_ID);

	// create thread for receiving message
	pthread_t thread;

	threadParams* params = (threadParams*)malloc(sizeof(threadParams));
	params->socket = sock;
	params->msg = msg;

	if(pthread_create(&thread, NULL, &run, (void*)params) < 0) {
    printf("pthread_create(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  strcpy(msg.content, "Registration message");
  if(send(sock, &msg, sizeof(msg) , 0)==-1) {
    printf("send(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }
	while(1) {
    fgets(msg.content, MAX_MSG_LENGTH, stdin);
    //send the message
    if(send(sock, &msg, sizeof(msg) , 0)==-1) {
      printf("send(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
  }

  if(pthread_join(thread, NULL)) {
    printf("pthread_join(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

	close(sock);

	return 0;
}

//------------------------------------------------------------------------------
void clean(int arg) {
  char path[256] = {0};
	sprintf(path, "/tmp/s%d", getpid());
	unlink(path);

  printf("[client] clean\n");

	exit(0);
}

//------------------------------------------------------------------------------
void* run(void* args) {
	threadParams* params = (threadParams*) args;

	while(1) {
		//try to receive some data, this is a blocking call
		if(recv(params->socket, &params->msg, sizeof(params->msg), 0) == -1) {
      printf("recv(): %d: %s\n", errno, strerror(errno));
      exit(-1);
    }
		printf("<%s>: %s\n", params->msg.name, params->msg.content);
	}
	return NULL;
}

//------------------------------------------------------------------------------
int getInternetSocket(char* address, int port) {
	struct sockaddr_in sock_in;
	int sock;

	if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    printf("socket(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

	sock_in.sin_family = AF_INET;
  sock_in.sin_port = htons(port);

  sock_in.sin_addr.s_addr = inet_addr((strncmp(address, "localhost", 9) == 0) ? "127.0.0.1" : address);

  if(connect(sock, (struct sockaddr*)&sock_in, sizeof(struct sockaddr)) < 0) {
    printf("connect(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  return sock;
}

//------------------------------------------------------------------------------

int getUnixSocket(char* file) {
	struct sockaddr_un server_un;
	struct sockaddr_un sock_un;

  char path[256] = {0};
	int sock;
	sprintf(path, "/tmp/s%d", getpid());

	path[strlen(path)] = '\0';
	unlink(path);

	if((sock = socket(PF_UNIX, SOCK_STREAM, 0)) == -1) {
    printf("socket(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  sock_un.sun_family = AF_UNIX;
  strcpy(sock_un.sun_path, path);

  if(bind(sock, (struct sockaddr*)&sock_un, SUN_LEN(&sock_un)) < 0) {
    printf("bind(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  server_un.sun_family = AF_UNIX;
  strcpy(server_un.sun_path, SERVER_PATH);

  if(connect(sock, (struct sockaddr*)&server_un, SUN_LEN(&sock_un)) < 0) {
    printf("connect(): %d: %s\n", errno, strerror(errno));
    exit(-1);
  }

  return sock;
}
