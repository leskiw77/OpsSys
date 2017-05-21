CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

all : main1 main2

main1  :
	$(CC) -o main1 main1.c -lpthread -lrt

main2  :
	$(CC) -o main2 main2.c -lpthread -lrt

clean:
	rm -f gen main1 main2

.PHONY: clean run