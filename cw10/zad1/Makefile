CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

all : client server

client  :
	$(CC) -o client client.c -lpthread -lrt

server  :
	$(CC) -o server server.c -lpthread -lrt

clean:
	rm -f gen client server

.PHONY: clean run