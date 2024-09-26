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

//Reads until it finds a newline or end of file
//Stores the result in the passed in buffer without the \n char
//Returns chars read or -1 if the file is empty
int readLine(int fd, char *buffer, unsigned int buf_size) {
  unsigned int bytesRead = 0;
  char c;
  int i = 0;

  // Read one character (c) at a time until a newline or end of file is encountered
  while ((bytesRead = checkError(read(fd, &c, 1), "Failed to read")) > 0 && i < buf_size - 1) {
    if (c == '\n') {
      //Stop reading at newline
      break;
    }
    buffer[i++] = c;
  }
  //Add a null termination to the end of the string
  buffer[i] = '\0';
  //Checks if file was empty
  if(bytesRead == 0 && i ==0){
    return -1;
  }
  else{return i;}
}

//Global variable to determine timer state
int expired = 0;
//Signal handler for timer expiration
int timerHandler(int sig){
  if(sig == SIGALRM){
    expired = 1;
  }
}

//Global variable to determine exit state
int exitConfirmed = 0;
// Signal handler for Control+C
void exitHandler(int sig) {
  exitConfirmed = 1; // Set flag to confirm exit
}



int main(){
  //max size of one string can be read at a time
  const unsigned int bufferSize = 256;
  
  //open Question file for reading
  int fdQ = checkError(open("quest.txt", O_RDONLY),"Failed to read file");
  //Open Answer file for reading
  int fdA = checkError(open("ans.txt", O_RDONLY),"Failed to read file");

  //An array to store the questions
  char bufQ[bufferSize];
  //An array to store the answers
  char bufA[bufferSize];
  
  //Tracks the amount of answers read
  double answersRead = 0;

  //Tracks question number
  unsigned int qNum = 1;

  //User choice to begin quiz
  char begin = 'p';

  //User Answer
  char ans[bufferSize];

  //Variables for storing correct answers
  unsigned int right = 0;

  //Variable for storing incorrect answers
  unsigned int wrong = 0;

  //Introduce the user to the quiz and it's rules
  printf("%s", "You are about to begin a timed test.\nYou will have 15 seconds per question.\nWhen the 15 seconds is up you will be moved to the next question.\nYou may exit at any time by using Control+C.\nWould you like to continue(Y/n):\n");

  //input loop to start the quiz
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

  //Setup sigaction for SIGALRM
  struct sigaction sa;
  sa.sa_handler = timerHandler;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  //Register the handler
  checkError(sigaction(SIGALRM, &sa, NULL), "Failed to set sigaction");

  //setup sigaction for SIGINT and register the handler
  sa.sa_handler = exitHandler;
  checkError(sigaction(SIGINT, &sa, NULL), "Failed to set sigaction");
  


  //Checks that the file is not empty then writes the question to the terminal and asks for an input
  while (exitConfirmed != 1) {
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

    //In case they exit in the middle of a question restarts the current question
    if (exitConfirmed) {
      while(exitConfirmed){
        //Cancel Timer
        it.it_value.tv_sec = 0; // Initial timer value
        checkError(setitimer(ITIMER_REAL, &it, NULL), "Failed to set timer");
        //Confirm exit with user
        char confirm;
        printf("\nDo you want to exit the quiz? (y/n): ");
        scanf(" %c", &confirm);
        if (confirm == 'y' || confirm == 'Y') {
          return 0; // Exit the quiz if confirmed
        } 
        else {
          //Reset timer
          it.it_value.tv_sec = 15; // Initial timer value
          checkError(setitimer(ITIMER_REAL, &it, NULL), "Failed to set timer");
          printf("Question %d: %s", qNum, bufQ); // Restart the question if they choose not to exit
          exitConfirmed = 0;
          scanf("%255s", ans);
          
        }
      }
    }

    
    

    //Compare input to known answer in bufA
    if (expired) {
      printf("\n%s", "Time has expired for this question... continuing\n"); // Print expiration message
      wrong += 1; // Count as incorrect if the timer expired
    } 
    else if (strcmp(ans, bufA) == 0) {
      right += 1; // Increment right answers
    } 
    else {
      wrong += 1; // Increment wrong answers
    }

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
  else{
    double percentage = (answersRead - wrong) / answersRead * 100;
    printf("You got %d answers right.\n", right);
    printf("You got %d answers wrong.\n", wrong);
    printf("You got %.2f %% correct.", percentage);
  }

  


  close(fdQ);
  close(fdA);
  return 0;
}