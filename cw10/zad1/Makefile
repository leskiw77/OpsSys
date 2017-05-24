CC = gcc -Wall -ansi -pedantic -std=c11 -pthread
P1 = server client

all: clean $(P1) 

clean:
	rm -f $(P1)

$(P1):
	$(CC) $@.c -o $@
