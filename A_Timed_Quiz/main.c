#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/time.h>



int checkError (int val, const char *msg)
{
  if (val == -1)
    {
      perror (msg);
      exit (EXIT_FAILURE);
    }
  return val;
}

//Reads until it finds a newline or end of file changes the buffer pointer given to the string read on the line without the \n char
int readLine(int fd, char *buffer, unsigned int buf_size) {
  unsigned int bytesRead = 0;
  char c;
  int i = 0;

  // Read one character (c) at a time until a newline or EOF is encountered
  while ((bytesRead = checkError(read(fd, &c, 1), "Failed to read")) > 0 && i < buf_size - 1) {
    if (c == '\n') {
      break;  // Stop reading at newline
    }
    buffer[i++] = c;
  }
  buffer[i] = '\0';
  if(bytesRead == 0 && i ==0){
    return -1;
  }
  else{return i;}
}

//Variable to determine timer state
int expired = 0;
//Signal handler for timer expiration
int timerHandler(int sig){
  if(sig == SIGALRM){
    expired = 1;
  }
}



int main(){
  //max size of one string can be read at a time
  const unsigned int bufferSize = 256;
  
  //open files for reading
  int fdQ = checkError(open("quest.txt", O_RDONLY),"Failed to read file");
  int fdA = checkError(open("ans.txt", O_RDONLY),"Failed to read file");

  //an array to store the questions/answers
  char bufQ[bufferSize];
  char bufA[bufferSize];
  
  //Tracks the amount of answers read
  double answersRead = 0;

  //Tracks question number
  unsigned int qNum = 1;

  //User choice to begin quiz
  char begin = 'p';

  //User Answer
  char ans[bufferSize];

  //Variables for storing correct/incorrect answers
  unsigned int right = 0;
  unsigned int wrong = 0;

  //Bool for time expiration
  unsigned expired = 0;



  //Introduce the user to the quiz and it's rules
  printf("%s", "You are about to begin a timed test.\nYou will have 15 seconds per question.\nWhen the 15 seconds is up you will be moved to the next question.\nYou may exit at any time by using Control+C.\nWould you like to continue(Y/n):\n");

  while (begin != 'y'||begin !='Y'||begin !='n'||begin !='N')
  {
      checkError(scanf(" %c", &begin),"Error Recieving Input");
    if (begin == 'N' || begin == 'n'){
      printf("%s", "Exiting Timed Quiz Program\n");
      return 0;
    }
    else if(begin == 'Y' || begin == 'y'){
      break;
    }
    else{
      printf("%s", "Invalid response. Please try again.\n");
    }
  }

  //Create timer
  struct itimerval it;

  //Setup sigaction
  struct sigaction sa;
  sa.sa_handler = timerHandler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  //Register the handler
  checkError(sigaction(SIGALRM, &sa, NULL), "Failed to set sigaction");


  //Checks that the file is not empty then writes the question to the terminal and asks for an input
  while (1) {
    //reset timer for each question
    expired = 0;
    it.it_value.tv_sec = 15; // Initial timer value
    it.it_value.tv_usec = 0;
    it.it_interval.tv_sec = 0; // No interval
    it.it_interval.tv_usec = 0;
    checkError(setitimer(ITIMER_REAL, &it, NULL), "Failed to set timer");

    
    // Read a question into the question buffer
    if (readLine(fdQ, bufQ, bufferSize) < 0) {
        break;  // Exit the loop if no more questions
    }
    
    // Read an answer into the answer buffer
    if (readLine(fdA, bufA, bufferSize) < 0) {
        printf("%s", "Answer: No corresponding answer found.\n");
        break;  // Exit if no more answers
    }
    
    // Print the question and recieve input
    printf("Question %d: %s", qNum, bufQ);
    scanf("%255s", ans);

    //Compare input to known answer in bufA
    if(expired){
      wrong += 1;
      printf("%s", "Time has expired for this question... continuing\n");
    }
    else if(strcmp(ans, bufA) == 0){
      right += 1;
    } 
    else{wrong += 1;}

    answersRead += 1;
    qNum += 1;

    // Reset the timer after processing the question
      it.it_value.tv_sec = 0; // Stop the timer
      it.it_value.tv_usec = 0;
      checkError(setitimer(ITIMER_REAL, &it, NULL), "Failed to reset timer");
  }


  //Calculate and output the results
  if(answersRead == 0){ //Disallows division by 0
    printf("%s", "One of the Q/A files was empty and did not allow reading of questions");
  }

  double percentage = (answersRead - wrong) / answersRead * 100;
  printf("You got %d answers right.\n", right);
  printf("You got %d answers wrong.\n", wrong);
  printf("You got %.2f %% correct.", percentage);


  close(fdQ);
  close(fdA);
  return 0;
}