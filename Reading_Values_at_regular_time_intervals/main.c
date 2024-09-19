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
    //max size of one doubles can be read at a time
    const unsigned int bufferSize = 1*sizeof(double);
    //open files for reading
    
    int fdAngl = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/angl.dat", O_RDONLY, 0644),"Failed to read file");
    int fdRota = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/rota.dat", O_RDONLY, 0644),"Failed to read file");
    int fdAccl = checkError(open("/home/hhargrove/Documents/GitHub/Systems-Programming/Reading_and_Writing_Data2/values/accl.dat", O_RDONLY, 0644),"Failed to read file");

    //An array to store the double
    double buffer[1];

    int pos = 0;
    double x;
    double y;
    double z;


    while((0 != checkError(read(fdAccl,buffer,bufferSize),"Failed to read")) && (0 != checkError(read(fdAngl,buffer,bufferSize),"Failed to read")) && 0 != checkError(read(fdRota,buffer,bufferSize),"Failed to read")){

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
    close(fdAccl);
    close(fdAngl);
    close(fdRota);

    
    return 0;
}