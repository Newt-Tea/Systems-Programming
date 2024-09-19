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



int main(){
    //max size of three doubles can be read at a time
    const unsigned int bufferSize = 3*sizeof(double);
    //open files for reading
    
    int fdAngl = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/angl.dat", O_RDONLY),"Failed to read file");

    //An array to store the three double
    double buffer[3];

    double yaw = 0.0;
    double pitch = 0.0;
    double roll = 0.0;
    int setCount = 1;

    const struct timespec req  = {1,0};
    struct timespec rem = {NULL, NULL};


    while(0 != checkError(read(fdAngl,buffer,bufferSize),"Failed to read")){

        // print the values of angl and wait a second after each information set

        roll = buffer[0];
        pitch = buffer [1];
        yaw = buffer[2];

        printf("%s","Data Set: ");
        printf("%i", setCount);

        if((roll >= -20.0) && (roll <= 20.0))
            printf("\n%s","The Roll value is in range\n");
        else{ printf("\n%s", "The Roll value is not in range\n");}

        if((pitch >= -20.0) && (pitch <= 20.0))
            printf("%s","The Pitch value is in range\n");
        else{ printf("%s", "The Pitch value is not in range\n");}

        if((yaw >= -20.0) && (yaw <= 20.0))
            printf("%s","The Yaw value is in range\n\n");
        else{ printf("%s", "The Yaw value is not in range\n\n");}

        nanosleep(&req,&rem);

        setCount += 1;
    }
    close(fdAngl);

    
    return 0;
}