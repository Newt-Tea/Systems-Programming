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
    //max size of nine integers can be read at a time
    const unsigned int bufferSize = 9*sizeof(double);

    if(mkdir("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values",S_IRWXG) != 0){
        
    }
    
    int fdR = checkError(open("data.dat",O_RDONLY, 0644),"Failed to open");

    //Create dir and files to write
    //checkError(mkdir(linkPath,S_I));
    int fdAccl = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/accl.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644),"Failed to create/write to file");
    int fdAngl = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/angl.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644),"Failed to create/write to file");
    int fdRota = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/rota.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644),"Failed to create/write to file");

    //An array to store the three integers of each type
    unsigned int buffer[bufferSize];

    int pos = 0;
    int x;
    int y;
    int z;


    while(0 != checkError(read(fdR,buffer,bufferSize),"Failed to read")){

        // write new values to new file
        for(int k = 0; k < 9; k+=3){
            //total iteration value

            pos = k;
            x = buffer[pos];
            y = buffer [pos + 1];
            z = buffer[pos + 2];


            if(k==0){
                write(fdAccl, &x);
                write(fdAccl, &y);
                write(fdAccl, &z);
            }

            if(k==3){
                write(fdAccl, &x);
                write(fdAccl, &y);
                write(fdAccl, &z);
            }

            if(k==6){
                write(fdAccl, &x);
                write(fdAccl, &y);
                write(fdAccl, &z);
            }

        }
    }

    close(fdR);
    close(fdAccl);
    close(fdAngl);
    close(fdRota);

    
    return 0;
}