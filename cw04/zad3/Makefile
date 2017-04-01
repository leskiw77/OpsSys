CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

all : main parent child

main : main.c
	$(CC) main.c -o main

parent : parent.c
	$(CC) parent.c -o parent

child : child.c
	$(CC) child.c -o child

clean:
	rm -f main parent child