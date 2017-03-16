CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall

all : libphonebook_stat.a libphonebook_dyn.so

##########################################################################
# CREATING STATIC LIBRARY :

libphonebook_stat.a  : person.o list.o tree.o generator.o
	ar rcsv libphonebook_stat.a person.o list.o tree.o generator.o

generator.o : generator.h person.h list.h tree.h  generator.c
	$(CC) -c generator.c $(CFLAGS) -o generator.o

list.o : person.h list.h list.c
	$(CC) -c list.c $(CFLAGS) -o list.o

tree.o : person.h tree.h tree.c
	$(CC) -c tree.c $(CFLAGS) -o tree.o

person.o : person.h person.c
	$(CC) -c person.c $(CFLAGS) -o person.o

##########################################################################
# CREATING DYNAMIC LIBRARY :

libphonebook_dyn.so  : person_d.o list_d.o tree_d.o generator_d.o
	gcc -shared person_d.o list_d.o tree_d.o generator_d.o -o libphonebook_dyn.so

generator_d.o : generator.h person.h list.h tree.h  generator.c
	gcc -c -fPIC generator.c -o generator_d.o

list_d.o : person.h list.h list.c
	gcc -c -fPIC list.c -o list_d.o

tree_d.o : person.h tree.h tree.c
	gcc -c -fPIC tree.c -o tree_d.o

person_d.o : person.h person.c
	gcc -c -fPIC person.c -o person_d.o


##########################################################################

clean:
	rm -f main.o list.o tree.o generator.o person.o list_d.o tree_d.o generator_d.o person_d.o

.PHONY: clean