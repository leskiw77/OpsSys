#include <stdio.h>
#include <sys/times.h>
#include <time.h>
#include <sys/resource.h>

#include "list.h"
#include "tree.h"
#include "generator.h"
#include "person.h"

void timeValuesPrint(struct rusage *start, struct rusage *end, long sRealTime, int times) {
    long userTime = (end->ru_utime.tv_usec)-(start->ru_utime.tv_usec);
    long sysTime  = (end->ru_stime.tv_usec)-(start->ru_stime.tv_usec);
    printf("real : %ld , user : %ld , sys : %ld  [us]\n\n",sRealTime/times,userTime/times,sysTime/times);
}

int main(){
    srand(time(NULL));
    struct rusage start,end;
    clock_t startRT, endRT;

    int times;

    // tworze tablice node'ow :
    listNode ** tab = (listNode**)malloc(1000*sizeof(listNode*));

    person * p1 = createPerson("The","Lowest",createDate(1,1,1990),
                               "mail@pl","000000000","Cracow");
    person * p2 = createPerson("The","Greatest",createDate(9,9,1991),
                               "mail@pl","999999999","Cracow");
    listNode * theLowestPhoneNumber = createListNode(p1);
    listNode * theGreatestPhoneNumber = createListNode(p2);
    for(int i=0;i<998;i++) {
        tab[i] = randomListNode();
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
        contactList=createList();
        deleteList(contactList);
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);


    printf("Add node to list\n");
    contactList=createList();
    times=1000;
    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        addElem(contactList,tab[i]);
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);


    printf("Sort list\n");
    times=10;
    startRT = clock();
    getrusage(RUSAGE_SELF,&start);

    for(int i=0;i<times;i++){
        sortListBySurname(contactList);
        sortListByEmail(contactList);
        sortListByBirthDate(contactList);
        sortListByPhoneNumber(contactList);
    }

    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,4*times);


    printf("Find optimistic\n");

    sortListByPhoneNumber(contactList);
    times=1000;
    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        findNodeByPhoneNumber(contactList,"000000000");
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);

    printf("Find pesimistic\n");

    sortListByPhoneNumber(contactList);
    times=1000;
    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        findNodeByPhoneNumber(contactList,"999999999");
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);


    times=1;
    sortListByPhoneNumber(contactList);

    printf("Delete optimistic\n");

    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        removeElem(contactList, theLowestPhoneNumber);
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);

    printf("Delete pesimistic\n");

    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        removeElem(contactList, theGreatestPhoneNumber);
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);


    //--------------------------------------------------------
    treeNode ** tab2 = (treeNode**)malloc(1000*sizeof(treeNode*));

    for(int i=0;i<1000;i++) {
        tab2[i] = randomTreeNode();
    }

    //--------------------------------------------------------

    printf("Tree\n###################################\n");

    printf("Init tree contact book\n");
    times=10000000;
    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        tree * t = createTree();
        deleteTree(&t);
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);

    printf("Add node to tree\n");
    tree * t = createTree();
    times=500;
    startRT = clock();
    getrusage(RUSAGE_SELF,&start);
    for(int i = 0; i<times; i++){
        treeInsert(t, tab2[i]);
    }
    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,times);


    printf("Sort tree\n");
    times=10;
    startRT = clock();
    getrusage(RUSAGE_SELF,&start);

    for(int i=0;i<times;i++){
        sortTreeBySurname(&t);
        sortTreeByEmail(&t);
        sortTreeByBirthDate(&t);
        sortTreeByPhoneNumber(&t);
    }

    getrusage(RUSAGE_SELF,&end);
    endRT = clock();
    timeValuesPrint(&start,&end,endRT-startRT,4*times);


    return 0;
}