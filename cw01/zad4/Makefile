CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall $(opt) -g

all=hello

main: main.c generator.o list.o tree.o person.o
	$(CC) main.c generator.o list.o tree.o person.o $(CFLAGS) -o main.out

generator.o : generator.h person.h list.h tree.h  generator.c
	$(CC) -c generator.c $(CFLAGS) -o generator.o

list.o : person.h list.h list.c
	$(CC) -c list.c $(CFLAGS) -o list.o

tree.o : person.h tree.h tree.c
	$(CC) -c tree.c $(CFLAGS) -o tree.o

person.o : person.h person.c
	$(CC) -c person.c $(CFLAGS) -o person.o

##########################################################################

clean:
	rm -f main.o list.o tree.o generator.o person.o list_d.o tree_d.o generator_d.o person_d.o

.PHONY: clean run