#ifndef LAB1_LIST_H
#define LAB1_LIST_H

#include "person.h"
#include <stdlib.h>
#include <stdio.h>

typedef struct listNode {
    person *per;
    struct listNode *next;
    struct listNode *prev;
} listNode;

typedef struct list {
    listNode *head;
} list;

list *createList();

listNode *createListNode(person *);

void deleteNode(listNode **);

int equalNodes(listNode *, listNode *);

void addElem(list *, listNode *);

void removeElem(list *, listNode *);

void printList(list *);

listNode *deleteList(list *);

listNode *findNode(list *l, person *p, int (*f)(person *, person *));

listNode *findNodeBySurname(list *, char *);

listNode *findNodeByEmail(list *, char *);

listNode *findNodeByBirthDate(list *, date *);

listNode *findNodeByPhoneNumber(list *, char *);

void sortListBySurname(list *l);

void sortListByEmail(list *l);

void sortListByBirthDate(list *l);

void sortListByPhoneNumber(list *l);


#endif //LAB1_LIST_H
