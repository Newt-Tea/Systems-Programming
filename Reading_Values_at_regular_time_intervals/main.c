#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include<string.h>



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
    
    int fdAngl = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/angl.dat", O_RDONLY, 0644),"Failed to read file");
    int fdRota = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/rota.dat", O_RDONLY, 0644),"Failed to read file");
    int fdAccl = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/accl.dat", O_RDONLY, 0644),"Failed to read file");

    //An array to store the three double
    double buffer[3];

    int pos = 0;
    double yaw = 0.0;
    double pitch = 0.0;
    double roll = 0.0;
    int setCount = 1;


    while(0 != checkError(read(fdAngl,buffer,bufferSize),"Failed to read")){

        // print the values of angl and wait a second after each information set

            pos = 0;
            roll = buffer[pos];
            pitch = buffer [pos + 1];
            yaw = buffer[pos + 2];

            printf("%s","Data Set: ");
            printf("%i", setCount);

            if((roll >= -20.0) && (roll <= 20.0))
                printf("\n%s","The Roll value is in range\n");
            else{ printf("%s", "The Roll value is not in range\n");}


        setCount += 1;
    }
    close(fdAccl);
    close(fdAngl);
    close(fdRota);

    
    return 0;
}