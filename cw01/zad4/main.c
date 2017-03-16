//
// Created by jarema on 3/13/17.
//

#include "list.h"
#include "generator.h"

int main(){
    list * l;// = createList();
    addElem(l,randomListNode());
    addElem(l,randomListNode());
    addElem(l,randomListNode());
    printList(l);
    return 0;
}