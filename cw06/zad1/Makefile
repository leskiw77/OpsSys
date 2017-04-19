CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

all : server client

server  : server.o
	$(CC) -o server server.o

server.o : config.h server.c
	$(CC) -c server.c $(CFLAGS) -o server.o

client  : client.o
	$(CC) -o client client.o

client.o : config.h client.c
	$(CC) -c client.c $(CFLAGS) -o client.o

clean:
	rm -f client.o server.o

.PHONY: clean run