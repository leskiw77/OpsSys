CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

all : main

main  :
	$(CC) -o main main.c -lpthread

clean:
	rm -f gen main

.PHONY: clean run