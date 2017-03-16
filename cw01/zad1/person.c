#include "person.h"

date *createDate(short day, short month, short year) {
    date *date1 = (date *) malloc(sizeof(date));
    date1->day = day;
    date1->month = month;
    date1->year = year;
    return date1;
}

int myDateComparison(date date1, date date2) {
    if (date1.year != date2.year)
        return date1.year - date2.year;
    if (date1.month != date2.month)
        return date1.month - date2.month;
    return date1.day - date2.day;
}

int dateComparison(person *p1, person *p2) {
    return myDateComparison(*(p1->birth_date), *(p2->birth_date));
}

int nameComparison(person *p1, person *p2) {
    return strcmp(p1->surname, p2->surname);
}

int emailComparison(person *p1, person *p2) {
    return strcmp(p1->email, p2->email);
}

int phoneComparison(person *p1, person *p2) {
    return strcmp(p1->phone_number, p2->phone_number);
}

int equalPeople(person *p1, person *p2) {

    if (nameComparison(p1, p2) != 0)
        return 0;

    if (dateComparison(p1, p2) != 0)
        return 0;

    if (emailComparison(p1, p2) != 0)
        return 0;

    if (phoneComparison(p1, p2) != 0)
        return 0;

    if (strcmp(p1->name, p2->name) != 0)
        return 0;

    if (strcmp(p1->address, p2->address) != 0)
        return 0;

    return 1;
}


person *createPerson(char *name, char *surname, date *birth_date, char *email, char *phone_number, char *address) {
    person *p = (person *) malloc(sizeof(person));
    p->name = name;
    p->surname = surname;
    p->birth_date = birth_date;
    p->email = email;
    p->phone_number = phone_number;
    p->address = address;
    return p;
}

void deletePerson(person *p) {
    free(p);
}

void printPerson(person *p) {
    printf("Name:            %s %s\n", p->name, p->surname);
    printf("E-mail:          %s\n", p->email);
    printf("Phone number:    %s \n", p->phone_number);
    printf("Address:         %s\n", p->address);
    printf("Date of birth:   %d.%d.%d\n\n", p->birth_date->day, p->birth_date->month, p->birth_date->year);
}