CC=gcc
CFLAGS= -ansi -pedantic -std=gnu99 -Wall

all : client server

server  : server.o helpers.o
	$(CC) -o server server.o helpers.o -lrt -lpthread

server.o : helpers.h server.c
	$(CC) -c server.c $(CFLAGS) -o server.o

client  : client.o helpers.o
	$(CC) -o client helpers.o client.o -lrt -lpthread

client.o : helpers.h client.c
	$(CC) -c client.c $(CFLAGS) -o client.o

helpers.o : helpers.h helpers.c
	$(CC) -c helpers.c $(CFLAGS) -o helpers.o

clean:
	rm -f helpers.o server.o client.o

.PHONY: clean run