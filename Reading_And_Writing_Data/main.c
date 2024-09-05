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
    int fd;
    char buf[3600];
    int valRd = 0;
    
    fd = checkError(open("raw.dat",O_RDONLY),"Failed to open");

    valRd = checkError(read(fd,buf,1000*sizeof(int)),"Failed to read");
    printf("%s","Read ");
    printf("%i ", valRd);
    printf("%s", "bytes, processing ");
    printf("%i ", valRd/20);
    printf("%s", "packets.\n");
// write new values to new file
// to: grader - I'm not sure how to do this without a nested loop if there's a good implementation could we go over that in class possibly
    int pos = 0;
    char low;
    char high;
    double postShift;
    double postDiv;

    double accel;
    double angular;
    double angle;

    for (int i = 0 ; i < valRd/20; i++){
        printf("%s", "Packet ");
        printf("%i", i+1);
        printf("%s", ":\n");

        for(int k = 0; k < 20; k+=2){
            //total iteration value

            if(i == 0){
                pos = k;
            }
            else if (i == 1){
                pos = k+20;
            }
            else{pos = i * k;}

            low = buf[pos];
            high = buf[pos + 1];
            postShift = ((short)high << 8)|low;
            postDiv = postShift/32768.0;


            if(k==2){
                double accell = postDiv*16*9.8;
                printf("%s","ax: ");
                printf("%f", accell);
                printf("%s","\n");
            }

            if(k==4){
                printf("%s","ay: ");

                printf("%s","\n");
            }

            if(k==6){
                printf("%s","az: ");

                printf("%s","\n");
            }

            if(k==8){
                printf("%s","wx: ");

                printf("%s","\n");
            }

            if(k==10){
                printf("%s","wy: ");
            
                printf("%s","\n");
            }

            if(k==12){
                printf("%s","wz: ");

                printf("%s","\n");
            }

            if(k==14){
                printf("%s","Roll: ");

                printf("%s","\n");
            }

            if(k==16){
                printf("%s","Pitch");

                printf("%s","\n");
            }

            if(k==18){
                printf("%s","Yaw");

                printf("%s","\n");
            }
        }
    }

    close(fd);

    
    return 0;
}