CC=gcc
CFLAGS= -ansi -pedantic -std=gnu99 -Wall

all : barber client

barber  : barber.o fifo.o
	$(CC) -o barber barber.o fifo.o -lrt -pthread

barber.o : general.h barber.c
	$(CC) -c barber.c $(CFLAGS) -o barber.o

client  : client.o fifo.o
	$(CC) -o client client.o fifo.o -lrt -pthread

client.o : general.h client.c
	$(CC) -c client.c $(CFLAGS) -o client.o

fifo.o : Fifo.h Fifo.c
	$(CC) -c Fifo.c $(CFLAGS) -o fifo.o

clean:
	rm -f fifo.o barber.o client.o

.PHONY: clean run