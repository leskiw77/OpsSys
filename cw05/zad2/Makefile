CC = gcc -Wall
P1 = master
P2 = slave

all: clean compile

compile:
	$(CC) $(P1).c -o $(P1)
	$(CC) $(P2).c complex.c -lm -o $(P2)

clean:
	rm -f $(P1) $(P2)

run:
	./$(P1)
