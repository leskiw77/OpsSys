#ifndef LAB1_PERSON_H
#define LAB1_PERSON_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct date {
    short month;
    short day;
    short year;
} date;

typedef struct person {
    char *name;
    char *surname;
    date *birth_date;
    char *email;
    char *phone_number;
    char *address;
} person;

date *createDate(short day, short month, short year);

int dateComparison(person *, person *);

int nameComparison(person *, person *);

int emailComparison(person *, person *);

int phoneComparison(person *, person *);

int equalPeople(person *, person *);

person *createPerson(char *name, char *surname, date *birth_date, char *email_address,
                     char *phone_number, char *address);

void deletePerson(person *);

void printPerson(person *);

#endif //LAB1_PERSON_H
