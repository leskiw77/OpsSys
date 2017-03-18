CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

all : prog

prog  : main.o
	$(CC) -o prog main.o

main.o : main.c
	$(CC) -c main.c $(CFLAGS) -o main.o

run : prog
	./prog ./testfile

clean:
	rm -f main.o

.PHONY: clean run