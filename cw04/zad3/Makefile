CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

all : parent child

parent : parent.c
	$(CC) parent.c -o parent

child : child.c
	$(CC) child.c -o child

clean:
	rm -f main parent child