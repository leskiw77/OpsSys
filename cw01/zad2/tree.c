#include "tree.h"

tree * createTree(){
    tree * tmp =(tree *)malloc(sizeof(tree));
    tmp ->root =NULL;
    return tmp;
}

treeNode * createTreeNode(person * p)
{
    if(p==NULL){
        printf("Null pointer argument");
        return NULL;
    }
    treeNode * temp = (treeNode *)malloc(sizeof(treeNode));
    temp->per = p;
    temp-> left = temp->right = NULL;
    return temp;
}

void insert(treeNode **leaf, treeNode * add,int (*f)(person *, person *))
{
    if( *leaf == NULL){
        *leaf = add;
    }

    else if (f(add->per,(*leaf)->per) < 0){
        insert(&(*leaf)->left,add,f);
    }
    else{
        insert(&(*leaf)->right,add,f);
    }
}


void treeInsert(tree * t, treeNode * add){

    if(t==NULL || add == NULL){
        printf("Null pointer argument");
        return;
    }

    if(t->root == NULL){
        t->root = add;
    } else{
        insert(&(t->root),add,nameComparison);
    }
}


void printTreeNodes(treeNode * root)
{
    if (root != NULL)
    {
        printTreeNodes(root->left);
        printPerson(root->per);
        printTreeNodes(root->right);
    }
}

void printTree(tree * t){

    if(t == NULL){
        printf("Null pointer passed");
        return;
    }

    printTreeNodes(t->root);
}



treeNode * treeSearch(treeNode * leaf, person *p, int (*f)(person *, person *)){
    if(leaf == NULL)
        return NULL;

    if(f(p,leaf->per) == 0){
        return leaf;
    }
    else if (f(p,leaf->per) < 0){
        return treeSearch(leaf->left,p,f);
    }
    else{
        return treeSearch(leaf->right,p,f);
    }
}

treeNode * findTreeNodeBySurname(tree * t, char * name){
    if(t==NULL){
        printf("Tree is null");
        return NULL;
    }
    person * tmp = createPerson(NULL,name,NULL,NULL,NULL,NULL);
    treeNode * ret = treeSearch(t->root,tmp,nameComparison);


    deletePerson(tmp);
    return ret;
}

treeNode * findTreeNodeByEmail(tree * t, char * mail){
    if(t==NULL){
        printf("Tree is null");
        return NULL;
    }
    person * tmp = createPerson(NULL,NULL,NULL,mail,NULL,NULL);
    treeNode * ret = treeSearch(t->root,tmp,emailComparison);
    deletePerson(tmp);
    return ret;
}

treeNode * findTreeNodeByPhoneNumber(tree * t, char * number){
    if(t==NULL){
        printf("Tree is null");
        return NULL;
    }
    person * tmp = createPerson(NULL,NULL,NULL,NULL,number,NULL);
    treeNode * ret = treeSearch(t->root,tmp,phoneComparison);
    deletePerson(tmp);
    return ret;
}

treeNode * findTreeNodeByBirthDate(tree * t, date * d) {
    if(t==NULL){
        printf("Tree is null");
        return NULL;
    }
    person * tmp = createPerson(NULL,NULL,d,NULL,NULL,NULL);
    treeNode *ret = treeSearch(t->root, tmp, dateComparison);
    deletePerson(tmp);
    return ret;
}

void treeInsertWithComparator(tree * t, treeNode * add, int (*f)(person *, person *)){
    if(t->root == NULL){
        t->root = add;
    } else{
        insert(&t->root,add,f);
    }
}

void recSort(tree *t, treeNode * leaf, int (*f)(person *, person *)){
    if(leaf == NULL){
        return;
    }

    recSort(t,leaf->left,f);
    recSort(t,leaf->right,f);

    //printPerson(leaf->per);
    leaf->left=leaf->right=NULL;

    treeInsertWithComparator(t,leaf,f);
}

void sortTreeBySurname(tree ** t) {
    if(*t==NULL){
        printf("Tree is null");
        return;
    }
    tree * t2 = createTree();
    recSort(t2,(*t)->root,nameComparison);
    free(*t);
    *t = t2;
}

void sortTreeByEmail(tree ** t) {
    if(*t==NULL){
        printf("Tree is null");
        return;
    }
    tree * t2 = createTree();
    recSort(t2,(*t)->root,emailComparison);
    free(*t);
    *t = t2;
}

void sortTreeByBirthDate(tree ** t){
    if(*t==NULL){
        printf("Tree is null");
        return;
    }
    tree * t2 = createTree();
    recSort(t2,(*t)->root,dateComparison);
    free(*t);
    *t = t2;
}

void sortTreeByPhoneNumber(tree ** t) {
    if(*t==NULL){
        printf("Tree is null");
        return;
    }
    tree * t2 = createTree();
    recSort(t2,(*t)->root,phoneComparison);
    free(*t);
    *t = t2;
}

void recursiveAddChildrenExceptDeleted(treeNode ** core, treeNode * leaf, treeNode * toDel){
    if(leaf == NULL){
        return;
    }

    recursiveAddChildrenExceptDeleted(core,leaf->right,toDel);
    recursiveAddChildrenExceptDeleted(core,leaf->left,toDel);

    if(leaf != toDel){
        leaf->left=leaf->right=NULL;
        insert(core, leaf,nameComparison);
    }
}

void deleteTreeNode(tree * t, treeNode ** toDel){
    if(t==NULL || *toDel == NULL){
        printf("Null pointer argument");
        return;
    }
    treeNode * tmp = t->root;
    t->root=NULL;
    recursiveAddChildrenExceptDeleted(&(t->root),tmp,*toDel);

    deletePerson((*toDel)->per);
    free(*toDel);
    *toDel=NULL;
}

void recursiveDel(treeNode ** node){
    if(*node == NULL){
        return;
    }

    recursiveDel(&((*node)->right));
    recursiveDel(&((*node)->left));

    deletePerson((*node)->per);
    free(*node);
    *node=NULL;
}

void deleteTree(tree ** t){
    if(*t==NULL){
        printf("Tree is null");
        return;
    }

    recursiveDel(&((*t)->root));

    free(*t);
    *t = NULL;
}