CC=gcc
CFLAGS= -ansi -pedantic -std=gnu99 -Wall

all : barber breeder

barber  : barber.o fifo.o
	$(CC) -o barber barber.o fifo.o helpers.o -lrt

barber.o : general.h barber.c
	$(CC) -c barber.c $(CFLAGS) -o barber.o

breeder  : breeder.o fifo.o
	$(CC) -o breeder breeder.o fifo.o helpers.o -lrt

breeder.o : general.h breeder.c
	$(CC) -c breeder.c $(CFLAGS) -o breeder.o

fifo.o : Fifo.h Fifo.c
	$(CC) -c Fifo.c $(CFLAGS) -o fifo.o

clean:
	rm -f fifo.o barber.o breeder.o

.PHONY: clean run