CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

all : prog

prog  : commands.o main.o
	$(CC) -o prog commands.o main.o

main.o : commands.h main.c
	$(CC) -c main.c $(CFLAGS) -o main.o

commands.o : commands.c commands.h
	$(CC) -c commands.c $(CFLAGS) -o commands.o

clean:
	rm -f main.o commands.o

.PHONY: clean run