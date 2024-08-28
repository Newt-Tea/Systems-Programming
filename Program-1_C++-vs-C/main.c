#include "BST.h"
#include <string.h>

int main()
{
  // Creating the BST
  struct BST *store = makeBST();
  //Sanity
  if (store == NULL) {
    printf("Couldn't create BST.\n");
  }

  printf("Please enter a series of words to store in a BST\n" );
  printf("Type 'STOP' to end the series and output the BST in postorder\n" );

//create an array with a predefined size so scanf knows when to terminate
char word[100];
  while (1)
    {
      printf("Word = ");
      scanf("%s",word);
      //Checks if the word given is the same as the value of STOP if so stop the loop
      if (strcmp(word, "STOP") == 0) break;
      //Sanity
      if (insert(store, word) != 1) {
            printf("Failed to insert '%s'.\n", word);
        }
    }

  printf("The BST output in postorder is \n");
postOrder(store);
  printf("\n");

//Deallocation
 free(store);
  return 1;
}
