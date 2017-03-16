CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

main : main.c library.o system.o
	$(CC) main.c library.o system.o $(CFLAGS) -o main

library.o: library.c
	$(CC) -c library.c $(CFLAGS) -o library.o

system.o: system.c
	$(CC) -c system.c $(CFLAGS) -o system.o

clean:
	rm -f library.o system.o