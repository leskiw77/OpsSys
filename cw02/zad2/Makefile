CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

all : prog

prog  : printerStat.o printerNftw.o main.o
	$(CC) -o prog printerStat.o printerNftw.o main.o

main.o : printerStat.h printerNftw.h main.c
	$(CC) -c main.c $(CFLAGS) -o main.o

printerNftw.o : printerNftw.c printerNftw.h
	$(CC) -c printerNftw.c $(CFLAGS) -o printerNftw.o

printerStat.o : printerStat.c printerStat.h
	$(CC) -c printerStat.c $(CFLAGS) -o printerStat.o

run : prog
	./prog ./test 2000 nftw

clean:
	rm -f main.o printerStat.o printerNftw.o

.PHONY: clean run