CC = gcc -Wall -pthread
P1 = server client

all: clean $(P1) 

clean:
	rm -f $(P1)

$(P1):
	$(CC) $@.c -o $@
