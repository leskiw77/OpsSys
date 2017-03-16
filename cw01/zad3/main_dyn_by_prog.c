#include "phonebook_dyn.h"
#include <sys/times.h>
#include <time.h>
#include <sys/resource.h>
#include <dlfcn.h>

void timeValuesPrint(struct rusage *start, struct rusage *end, long sRealTime, int times) {
    long userTime = (end->ru_utime.tv_usec)-(start->ru_utime.tv_usec);
    long sysTime  = (end->ru_stime.tv_usec)-(start->ru_stime.tv_usec);
    printf("real : %ld , user : %ld , sys : %ld  [us]\n\n",sRealTime/times,userTime/times,sysTime/times);
}

int main() {
    srand(time(NULL));
    void* pbHandle = dlopen ("./libphonebook_dyn.so", RTLD_LAZY);
    if (!pbHandle) {
        printf ("Blad ladowania biblioteki dynamicznej\n");
        exit(1);
    }

    typedef list * (*createList)();
    typedef listNode * (*createListNode)(person *);
    typedef void (*deleteNode)(listNode **);
    typedef int (*equalNodes)(listNode *, listNode *);
    typedef void (*addElem)(list *, listNode *);
    typedef void (*removeElem)(list *, listNode *);
    typedef void (*printList)(list *);
    typedef listNode * (*deleteList)(list *);
    typedef listNode * (*findNode)(list *l, person *p, int (*f)(person *, person *));
    typedef listNode * (*findNodeBySurname)(list *, char *);
    typedef listNode * (*findNodeByEmail)(list *, char *);
    typedef listNode * (*findNodeByBirthDate)(list *, date *);
    typedef listNode * (*findNodeByPhoneNumber)(list *, char *);
    typedef void (*sortListBySurname)(list *l);
    typedef void (*sortListByEmail)(list *l);
    typedef void (*sortListByBirthDate)(list *l);
    typedef void (*sortListByPhoneNumber)(list *l);

    createList l_createList = (createList)dlsym(pbHandle, "createList");
    createListNode l_createListNode = (createListNode)dlsym(pbHandle, "createListNode");
    deleteNode l_deleteNode = (deleteNode)dlsym(pbHandle, "deleteNode");
    addElem l_addElem = (addElem)dlsym(pbHandle, "addElem");
    removeElem l_removeElem = (removeElem)dlsym(pbHandle, "removeElem");
    printList l_printList = (printList)dlsym(pbHandle, "printList");
    deleteList l_deleteList = (deleteList)dlsym(pbHandle, "deleteList");
    findNode l_findNode = (findNode)dlsym(pbHandle, "findNode");
    findNodeBySurname l_findNodeBySurname = (findNodeBySurname)dlsym(pbHandle, "findNodeBySurname");
    findNodeByEmail l_findNodeByEmail = (findNodeByEmail)dlsym(pbHandle, "findNodeByEmail");
    findNodeByBirthDate l_findNodeByBirthDate = (findNodeByBirthDate)dlsym(pbHandle, "findNodeByBirthDate");
    findNodeByPhoneNumber l_findNodeByPhoneNumber = (findNodeByPhoneNumber)dlsym(pbHandle, "findNodeByPhoneNumber");
    sortListBySurname l_sortListBySurname = (sortListBySurname)dlsym(pbHandle, "sortListBySurname");
    sortListByEmail l_sortListByEmail = (sortListByEmail)dlsym(pbHandle, "sortListByEmail");
    sortListByBirthDate l_sortListByBirthDate = (sortListByBirthDate)dlsym(pbHandle, "sortListByBirthDate");
    sortListByPhoneNumber l_sortListByPhoneNumber = (sortListByPhoneNumber)dlsym(pbHandle, "sortListByPhoneNumber");

    typedef person * (*createPerson)(char *, char *, date *, char *, char *, char *);
    createPerson l_createPerson = (createPerson)dlsym(pbHandle, "createPerson");

    typedef date * (*createDate)(short, short, short);
    createDate l_createDate = (createDate)dlsym(pbHandle, "createDate");

    typedef listNode * (*randomListNode)();
    randomListNode l_randomListNode = (randomListNode)dlsym(pbHandle, "randomListNode");

    int times;
    struct rusage start,end;
    clock_t startRT, endRT;

    listNode ** tab = (listNode**)malloc(1000*sizeof(listNode*));

    person * p1 = l_createPerson("The","Lowest",l_createDate(1,1,1990),
                               "mail@pl","000000000","Cracow");
    person * p2 = l_createPerson("The","Greatest",l_createDate(9,9,1991),
                               "mail@pl","999999999","Cracow");
    listNode * theLowestPhoneNumber = l_createListNode(p1);
    listNode * theGreatestPhoneNumber = l_createListNode(p2);
    for(int i=0;i<998;i++) {
        tab[i] = l_randomListNode();
    }
    tab[998] = theLowestPhoneNumber;
    tab[999] = theGreatestPhoneNumber;

    printf("List\n###################################\n");
    list * contactList;

    printf("Init list contact book\n");
    times=10000000;
    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        contactList=l_createList();
        l_deleteList(contactList);
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);


    printf("Add node to list\n");
    contactList=l_createList();
    times=1000;
    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        l_addElem(contactList,tab[i]);
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);


    printf("Sort list\n");
    times=10;
    startRT = clock();
    getrusage(RUSAGE_SELF,&start);

    for(int i=0;i<times;i++){
        l_sortListBySurname(contactList);
        l_sortListByEmail(contactList);
        l_sortListByBirthDate(contactList);
        l_sortListByPhoneNumber(contactList);
    }

    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,4*times);


    printf("Find optimistic\n");

    l_sortListByPhoneNumber(contactList);
    times=1000;
    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        l_findNodeByPhoneNumber(contactList,"000000000");
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);

    printf("Find pesimistic\n");

    l_sortListByPhoneNumber(contactList);
    times=1000;
    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        l_findNodeByPhoneNumber(contactList,"999999999");
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);


    times=1;
    l_sortListByPhoneNumber(contactList);

    printf("Delete optimistic\n");

    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        l_removeElem(contactList, theLowestPhoneNumber);
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);

    printf("Delete pesimistic\n");

    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        l_removeElem(contactList, theGreatestPhoneNumber);
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);


    dlclose (pbHandle);
    return 0;
}


