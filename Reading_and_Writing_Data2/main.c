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
    //max size of nine doubles can be read at a time
    const unsigned int bufferSize = 9*sizeof(double);
    //Create dir and files to write
    
    if(mkdir("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values",S_IRWXU) != 0){
        printf("%s", "Dir exists continuing");
    }
    else{printf("%s", "Created dir");}
    
    int fdR = checkError(open("data.dat",O_RDONLY, 0644),"Failed to open");


    int fdAccl = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/accl.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644),"Failed to create/write to file");
    int fdAngl = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/angl.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644),"Failed to create/write to file");
    int fdRota = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/rota.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644),"Failed to create/write to file");

    //An array to store the three integers of each type
    double buffer[9];

    int pos = 0;
    double x;
    double y;
    double z;


    while(0 != checkError(read(fdR,buffer,bufferSize),"Failed to read")){

        // write new values to new file
        for(int k = 0; k < 9; k+=3){
            //total iteration value

            pos = k;
            x = buffer[pos];
            y = buffer [pos + 1];
            z = buffer[pos + 2];


            if(k==0){
                write(fdAccl, &x,sizeof(double));
                write(fdAccl, &y,sizeof(double));
                write(fdAccl, &z,sizeof(double));
            }

            if(k==3){
                write(fdAngl, &x,sizeof(double));
                write(fdAngl, &y,sizeof(double));
                write(fdAngl, &z,sizeof(double));
            }

            if(k==6){
                write(fdRota, &x,sizeof(double));
                write(fdRota, &y,sizeof(double));
                write(fdRota, &z,sizeof(double));
            }

        }
    }

    close(fdR);
    close(fdAccl);
    close(fdAngl);
    close(fdRota);

    
    return 0;
}