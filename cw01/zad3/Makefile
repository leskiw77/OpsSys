CC=gcc
CFLAGS= -ansi -pedantic -std=c11 -Wall $(opt)


all : libphonebook_stat.a libphonebook_dyn.so prog_sta prog_dyn prog_dyn_by_pr

##########################################################################

prog_sta  : calc.c libphonebook_stat.a
	$(CC) -o prog_sta calc.c libphonebook_stat.a


prog_dyn  : calc.c libphonebook_dyn.so
	$(CC) calc.c -o prog_dyn -L. -lphonebook_dyn

prog_dyn_by_pr: main_dyn_by_prog.c
	$(CC) main_dyn_by_prog.c -o prog_dyn_by_pr -ldl

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
	$(CC) -shared person_d.o list_d.o tree_d.o generator_d.o -o libphonebook_dyn.so

generator_d.o : generator.h person.h list.h tree.h  generator.c
	$(CC) -c -fPIC $(CFLAGS) generator.c -o generator_d.o

list_d.o : person.h list.h list.c
	$(CC) -c -fPIC $(CFLAGS) list.c -o list_d.o

tree_d.o : person.h tree.h tree.c
	$(CC) -c -fPIC $(CFLAGS) tree.c -o tree_d.o

person_d.o : person.h person.c
	$(CC) -c -fPIC $(CFLAGS) person.c -o person_d.o


gen : prog_sta prog_dyn prog_dyn_by_pr
    echo "Static:\n"
    #./prog_sta >> result.txt
    #echo "Dynamic:\n" >> result.txt
    #./prog_dyn >> result.txt



run: test_dynamic_linking test_static test_dynamic 


test_static: prog_sta
	rm -f results.txt
	echo "\nTest of static library:\n" >> results.txt
	./prog_sta >> results.txt

test_dynamic: prog_dyn
	echo "\nTest of dynamic library:\n\n" >> results.txt
	./prog_dyn >> results.txt

test_dynamic_linking: prog_dyn_by_pr
	echo "\nTest of dynamicly linked library:\n\n" >> results.txt
	./prog_dyn_by_pr >> results.txt

clean:
	rm -f *.o *.a *.so

.PHONY: clean run
