#include "BST.h"
#include <string.h>

int main()
{

  BST *store = malloc(1000);
  string word;

  printf("Please enter a series of words to store in a BST" );
  printf("Type 'STOP' to end the series and output the BST" );
  printf("In postorder");


  while (1)
    {
      printf("Word = ";
      scanf("%s",word)
      if (word == "STOP") break;
      store.insert(word);
    }

  printf("The BST ouput in postorder is ");
  store.postOrder(cout);
  printf( );


 free(store)
  return 1;
}
