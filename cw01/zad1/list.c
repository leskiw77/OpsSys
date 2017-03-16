#include "list.h"

list *createList() {
    list *result = (list *) malloc(sizeof(list));
    result->head = NULL;
    return result;
}

listNode *createListNode(person *p) {
    listNode *n = (listNode *) malloc(sizeof(listNode));
    n->per = p;
    n->next = n->prev = NULL;
    return n;
}

int equalNodes(listNode *n1, listNode *n2) {
    return equalPeople(n1->per, n2->per);
}

void deleteNode(listNode **n) {
    deletePerson((*n)->per);
    free(*n);
    *n = NULL;
}

void addElem(list *list1, listNode *newNode) {

    if (list1 == NULL || newNode == NULL) {
        printf("Null pointer passed");
        return;
    }

    listNode *iter = list1->head;
    if (list1->head != NULL) {
        while (iter->next != NULL) {
            iter = iter->next;
        }
        iter->next = newNode;
        newNode->prev = iter;
        newNode->next = NULL;

    } else {
        list1->head = newNode;
        newNode->next = NULL;
        newNode->prev = NULL;
    }
}

void removeElem(list *list1, listNode *nodeRemove) {
    if (list1 == NULL || nodeRemove == NULL) {
        printf("Null pointer passed");
        return;
    }

    listNode *iter = list1->head;
    while (iter != NULL && nodeRemove != iter) {
        iter = iter->next;
    }

    if (iter != NULL) {
        if (iter->prev == NULL) {
            list1->head = iter->next;
            if (list1->head != NULL) {
                list1->head->prev = NULL;
            }
        } else {
            iter->prev->next = iter->next;
            if (iter->next != NULL) {
                iter->next->prev = iter->prev;
            }
        }
        deleteNode(&iter);
    }

    return;
}

void printList(list *l) {

    if (l == NULL) {
        return;
    }

    listNode *iter = l->head;

    while (iter != NULL) {
        printPerson(iter->per);
        iter = iter->next;
    }
}

listNode *deleteList(list *l) {
    if (l == NULL) {
        return NULL;
    }

    listNode *iter = l->head;

    while (iter != NULL) {
        listNode *nxt = iter->next;
        deleteNode(&iter);
        iter = nxt;
    }

    free(l);
    return NULL;
}

listNode *findNode(list *l, person *p, int (*f)(person *, person *)) {
    if (l == NULL || p == NULL) {
        return NULL;
    }

    listNode *iter = l->head;

    while (iter != NULL) {
        if (f(p, iter->per) == 0) {
            return iter;
        }
        iter = iter->next;
    }
    return NULL;
}

listNode *findNodeBySurname(list *l, char *name) {
    person *tmp = createPerson(NULL, name, NULL, NULL, NULL, NULL);
    listNode *ret = findNode(l, tmp, nameComparison);
    deletePerson(tmp);
    return ret;
}

listNode *findNodeByEmail(list *l, char *mail) {
    person *tmp = createPerson(NULL, NULL, NULL, mail, NULL, NULL);
    listNode *ret = findNode(l, tmp, emailComparison);
    deletePerson(tmp);
    return ret;
}

listNode *findNodeByPhoneNumber(list *l, char *number) {
    person *tmp = createPerson(NULL, NULL, NULL, NULL, number, NULL);
    listNode *ret = findNode(l, tmp, phoneComparison);
    deletePerson(tmp);
    return ret;
}

listNode *findNodeByBirthDate(list *l, date *d) {
    person *tmp = createPerson(NULL, NULL, d, NULL, NULL, NULL);
    listNode *ret = findNode(l, tmp, dateComparison);
    deletePerson(tmp);
    return ret;
}

listNode *addNode(listNode *n, listNode *add, int (*f)(person *, person *))//n and add not NULL
{
    //add first element
    if (f(add->per, n->per) <= 0) {
        n->prev = add;
        add->next = n;
        return add;
    }
    listNode *back = n;
    listNode *front = n->next;

    while (front != NULL && f(add->per, front->per) > 0) {
        back = front;
        front = front->next;
    }
    add->next = front;
    add->prev = back;
    back->next = add;
    if (front != NULL)
        front->prev = add;

    return n;
}

listNode *sort(listNode *n, int (*f)(person *, person *)) {
    if (n == NULL)
        return NULL;

    listNode *ret = n;
    n = n->next;
    ret->next = ret->prev = NULL;

    while (n != NULL) {
        listNode *tmp = n;
        n = n->next;
        tmp->next = tmp->prev = NULL;
        ret = addNode(ret, tmp, f);
    }
    return ret;
}

void sortList(list *l, int (*f)(person *, person *)) {
    l->head = sort(l->head, f);
}

void sortListBySurname(list *l) {
    sortList(l, nameComparison);
}

void sortListByEmail(list *l) {
    sortList(l, emailComparison);
}

void sortListByBirthDate(list *l) {
    sortList(l, dateComparison);
}

void sortListByPhoneNumber(list *l) {
    sortList(l, phoneComparison);
}