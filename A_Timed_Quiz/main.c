#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>



int checkError (int val, const char *msg)
{
  if (val == -1)
    {
      perror (msg);
      exit (EXIT_FAILURE);
    }
  return val;
}
//Reads until it finds a newline or end of file
int readLine(int fd, char *buffer, unsigned int buf_size) {
  unsigned int bytesRead = 0;
  char c;
  unsigned int i = 0;

  // Read one character at a time until a newline or EOF is encountered
  while ((bytesRead = checkError(read(fd, &c, 1), "Failed to read")) > 0 && i < buf_size - 1) {
    buffer[i++] = c;
    if (c == '\n') {
      break;  // Stop reading at newline
    }
  }
  buffer[i] = '\0';
  if(bytesRead == 0 && i ==0){
    return -1;
  }
  else{return i;}
}



int main(){
    //max size of three doubles can be read at a time
    const unsigned int bufferSize = 256;
    
    //open files for reading
    int fdQ = checkError(open("quest.txt", O_RDONLY),"Failed to read file");
    int fdA = checkError(open("ans.txt", O_RDONLY),"Failed to read file");

    //an array to store the questions/answers
    char bufQ[bufferSize];
    char bufA[bufferSize];
  

    const struct timespec req  = {1,0};
    struct timespec rem = {NULL, NULL};

    //Checks that the file is not empty then writes the question to the terminal and asks for an input
       while (1) {
        // Read a question into the question buffer
        if (readLine(fdQ, bufQ, bufferSize) == -1) {
            break;  // Exit the loop if no more questions
        }
        
        // Read an answer into the answer buffer
        if (readLine(fdA, bufA, bufferSize) == -1) {
            printf("Answer: No corresponding answer found.\n");
            break;  // Exit if no more answers
        }
        
        // Print the question and answer
        printf("Question: %s", bufQ);  // Already includes a newline
        printf("Answer: %s", bufA);      // Already includes a newline
    }
    close(fdQ);
    close(fdA);

    
    return 0;
}