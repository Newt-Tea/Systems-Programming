#ifndef BST_H
#define BST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node
{
  char *value;
  struct Node *left;
  struct Node *right;
};

void freeNode(struct Node *node);

struct BST
{
  struct Node *root;
};


struct BST *makeBST();
void freeBST(struct BST *bst);


// Definitions
int insert(struct BST *bst, const char *value);
int search(const struct BST *bst, const char *value);
void postOrder(const struct BST *bst);
void printValue(const char *value);


#endif
