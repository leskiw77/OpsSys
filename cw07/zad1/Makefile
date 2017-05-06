CC=gcc
CFLAGS= -ansi -pedantic -std=gnu99 -Wall

all : barber breeder

barber  : barber.o fifo.o helpers.o
	$(CC) -o barber barber.o fifo.o helpers.o -lrt

barber.o : hairdresser.h helpers.o barber.c
	$(CC) -c barber.c $(CFLAGS) -o barber.o

breeder  : breeder.o fifo.o helpers.o
	$(CC) -o breeder breeder.o fifo.o helpers.o -lrt

breeder.o : hairdresser.h helpers.o breeder.c
	$(CC) -c breeder.c $(CFLAGS) -o breeder.o

helpers.o : helpers.h helpers.c
	$(CC) -c helpers.c $(CFLAGS) -o helpers.o

fifo.o : Fifo.h Fifo.c
	$(CC) -c Fifo.c $(CFLAGS) -o fifo.o

clean:
	rm -f helpers.o fifo.o barber.o breeder.o

.PHONY: clean run