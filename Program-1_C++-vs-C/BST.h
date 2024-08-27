#ifndef BST_H
#define BST_H

#include <malloc.h>
#include <string.h>

struct Node
{
  std::string value;
  Node *left;
  Node *right;


};

struct Node *makeNode();
  Node(): left(nullptr), right(nullptr) {}
  Node(const std::string &value): value(value), left(nullptr), right(nullptr) {}

struct BST
{
  struct Node *root;
};

struct BST *makeBST();
BST() : root(nullptr) {}
  BST( BST &old) { old.copy(this);}
  ~BST() { dealloc(root);}

int insert(struct Node ** node, const char *value);
int insert(struct BST *bst, const char *value);

int search(struct Node ** node, const char *value);
int search(struct BST *bst, const char *value);

int remove(Node *&node, const std::string &value);
int remove(const std::string &value);

  void postOrder(Node *node, std::ostream &os);
  void dealloc(Node *&node);
  void copy(Node *node, BST *bstCp);


#endif
