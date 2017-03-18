CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

all : prog

prog  : locks.o bytes.o main.o
	$(CC) -o prog locks.o bytes.o main.o

main.o : locks.h bytes.h main.c
	$(CC) -c main.c $(CFLAGS) -o main.o

bytes.o : locks.h bytes.h bytes.c
	$(CC) -c bytes.c $(CFLAGS) -o bytes.o

locks.o : locks.h locks.c
	$(CC) -c locks.c $(CFLAGS) -o locks.o

run : prog
	./prog ./testfile

clean:
	rm -f main.o bytes.o locks.o

.PHONY: clean run