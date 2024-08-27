#include "Program-1_C++-vs-C\BST.h"


int insertNode(struct Node ** node, const char *value)
{
  if (node == NULL)
    {
      node = new Node(value);
      return 1;
    }
  if (value < node->value) { return insert(node->left, value);}
  if (value > node->value) { return insert(node->right,value);}
  return 0;
}

int insert(struct BST *bst, const char *s){
  if (strcmp((*node)->value,s) < 0){
    return insertNode(&((*node)->left),s);
  }
}


int BST::search(Node *node, const std::string &value)
{
  if (node == nullptr)
    {
      return 0;
    }
  if (value < node->value) { return search(node->left,value);}
  if (value > node->value) { return search(node->right,value);}
  return 1;
}
bool BST::remove(Node *&node, const std::string &value)
{
  return 1;
}

void BST::postOrder(Node *node, std::ostream &os)
{
  if (node == nullptr) return;
  postOrder(node->left,os);
  postOrder(node->right,os);
  os << node->value << " ";
}
void BST::dealloc(Node *&node)
{
  if (node == nullptr) return;
  dealloc(node->left);
  dealloc(node->right);
  delete node;
  node = nullptr;
}

void BST::copy(Node *node, BST *bstCp)
{
  if (node == nullptr) return;
  bstCp->insert(node->value);
  copy(node->left,bstCp);
  copy(node->right,bstCp);
}
