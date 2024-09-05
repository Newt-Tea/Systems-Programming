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
    const signed char bufferSize = 20;
    
    int fdR = checkError(open("raw.dat",O_RDONLY, 0644),"Failed to open");
    int fdW = checkError(open("data.dat", O_WRONLY | O_CREAT | O_TRUNC, 0644),"Failed to create/write to file");

    signed char buffer[bufferSize];

    int pos = 0;
    signed short low;
    signed short high;
    double postShift;
    double postDiv;

    double accell;
    double angular;
    double angle;

    double *accellPTR = malloc(sizeof(double));
    double *angularPTR = malloc(sizeof(double));
    double *anglePTR = malloc(sizeof(double));
    while(0 != checkError(read(fdR,buffer,bufferSize),"Failed to read")){

        // write new values to new file
        for(int k = 2; k < 20; k+=2){
            //total iteration value

            pos = k;

            low = buffer[pos];
            high = buffer[pos + 1];
            postShift = (high << 8)|low;
            postDiv = postShift/32768.0;
            accell = postDiv*16*9.8;
            angular = postDiv*2000;
            angle = postDiv*180;  

            if(k==2){
                
                printf("%s","ax: ");
                printf("%f", accell);
                printf("%s","\n");

                write(fdW,*accellPTR);
            }

            if(k==4){
                printf("%s","ay: ");
                printf("%f", accell);
                printf("%s","\n");

                write(fdW,*accellPTR);
            }

            if(k==6){
                printf("%s","az: ");
                printf("%f", accell);
                printf("%s","\n");

                write(fdW,*accellPTR);
            }

            if(k==8){
                printf("%s","wx: ");
                printf("%f", angular);
                printf("%s","\n");

                write(fdW,*angularPTR);
            }

            if(k==10){
                printf("%s","wy: ");
                printf("%f", angular);
                printf("%s","\n");

                write(fdW,*angularPTR);
            }

            if(k==12){
                printf("%s","wz: ");
                printf("%f", angular);
                printf("%s","\n");

                write(fdW,*angularPTR);
            }

            if(k==14){
                printf("%s","Roll: ");
                printf("%f", angle);
                printf("%s","\n");

                write(fdW,*anglePTR);
            }

            if(k==16){
                printf("%s","Pitch: ");
                printf("%f", angle);
                printf("%s","\n");

                write(fdW,*anglePTR);
            }

            if(k==18){
                printf("%s","Yaw: ");
                printf("%f", angle);
                printf("%s","\n");

                write(fdW,*anglePTR);
            }
        }
    }

    close(fdR);
    close(fdW);

    
    return 0;
}