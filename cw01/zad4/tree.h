#ifndef LAB1_TREE_H
#define LAB1_TREE_H

#include<stdio.h>
#include<stdlib.h>

#include "person.h"

typedef struct treeNode
{
    person * per;
    struct treeNode * left, *right;
} treeNode;

typedef struct tree
{
    struct treeNode * root;
}tree;

tree * createTree();

treeNode * createTreeNode(person *);

void treeInsert(tree *, treeNode *);

void printTree(tree *);

treeNode * findTreeNodeBySurname(tree *, char *);

treeNode * findTreeNodeByEmail(tree *, char *);

treeNode * findTreeNodeByPhoneNumber(tree *, char *);

treeNode * findTreeNodeByBirthDate(tree *, date *);

void sortTreeBySurname(tree **);

void sortTreeByEmail(tree **);

void sortTreeByBirthDate(tree **);

void sortTreeByPhoneNumber(tree **);

void deleteTreeNode(tree *, treeNode **);

void deleteTree(tree ** );
#endif //LAB1_TREE_H
