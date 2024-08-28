#include "BST.h"

// function to create a node with the given value
struct Node *makeNode(const char *value){
  struct Node* node = (struct Node *) malloc(sizeof(struct Node));
  if (node == NULL) return NULL;

  node->value = strdup(value);
  node->left = NULL;
  node->right = NULL;
  return node;
}

void freeNode(struct Node *node)
{
  if(node != NULL){
    free(node->value);
    freeNode(node->left);
    freeNode(node->right);
    free(node);
  }
}

// function to create a BST 
struct BST *makeBST()
{
    struct BST *bst = (struct BST *)malloc(sizeof(struct BST));
    if (bst == NULL) return NULL;
    bst->root = NULL;
    return bst;
}

int insert(struct BST *bst, const char *value){
  struct Node **str = &bst->root;
    while (*str != NULL) {
        int cmp = strcmp(value, (*str)->value);
        if (cmp < 0) {
            str = &(*str)->left;
        } else if (cmp > 0) {
            str = &(*str)->right;
        } else {
            return 0; // checks if the value is already in the tree
        }
    }

    *str = makeNode(value);
    return *str != NULL;
}

// function to find a value in the BST
int search(const struct BST *bst, const char *value)
{
    struct Node *current = bst->root;
    while (current != NULL) {
        int cmp = strcmp(value, current->value);
        if (cmp < 0) {
            current = current->left;
        } else if (cmp > 0) {
            current = current->right;
        } else {
            return 1;
        }
    }
    return 0; 
}
//function to find the node with the lowest value
struct Node *findMin(struct Node *node)
{
    while (node->left != NULL) {
        node = node->left;
    }
    return node;
}

// function to remove nodes values, and children
struct Node *removeNode(struct Node *node, const char *value)
{
    if (node == NULL) return NULL;

    int cmp = strcmp(value, node->value);
    if (cmp < 0) {
        node->left = removeNode(node->left, value);
    } else if (cmp > 0) {
        node->right = removeNode(node->right, value);
    } else {
        // node to be deleted found

        // for nodes with no or one child
        if (node->left == NULL) {
            struct Node *temp = node->right;
            free(node->value);
            free(node);
            return temp;
        } else if (node->right == NULL) {
            struct Node *temp = node->left;
            free(node->value);
            free(node);
            return temp;
        }

        // past this all nodes have two children or more
        //this finds the next lowest node value to remove next
        struct Node *temp = findMin(node->right);

        // duplicates the next nodes value to this node
        free(node->value);
        node->value = strdup(temp->value);

        // deletes the next Node
        node->right = removeNode(node->right, temp->value);
    }

    return node;
}

// function to remove complete bst using node deltes
int removeBST(struct BST *bst, const char *value)
{
    struct Node *oldRoot = bst->root;
    bst->root = removeNode(bst->root, value);
    return (bst->root != oldRoot); // Return 1 if the root was changed
}

// function to traverses in postOrder going down the leftmost side of the tree and moving from the lowest leaves up
void postOrder(const struct BST *bst)
{
    void postOrderNode(struct Node *node)
    {
        if (node != NULL) {
            postOrderNode(node->left);
            postOrderNode(node->right);
            printf("%s ",node->value);

        }
    }

    postOrderNode(bst->root);
}
// // used to help the postorder function
// void printValue(const char *value)
// {
//     printf("%s ", value);
// }
