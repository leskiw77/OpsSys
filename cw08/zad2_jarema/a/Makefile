CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

all : gen main1 main2 main3

main1  :
	$(CC) -o main1 main1.c -lpthread

main2  :
	$(CC) -o main2 main2.c -lpthread

main3  :
	$(CC) -o main3 main3.c -lpthread

main4  :
	$(CC) -o main3 main4.c -lpthread

main5  :
	$(CC) -o main3 main5.c -lpthread

clean:
	rm -f gen main1 main2 main3 main4 main5

.PHONY: clean run