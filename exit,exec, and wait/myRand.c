#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    srand(time(NULL));
    int X = rand() % 255; // Generate a random number between 0 and 255
    
    char numStr[10];
    char fileName[50];
    char filePre[] = "data";
    char filePost[] = ".dat"; 

    // Convert the random number to a string
    int i = 0;
    int temp = X;
    while (temp > 0) {
        numStr[i++] = (temp % 10) + '0'; // Take the last digit and convert to a char
        temp /= 10;
    }
    numStr[i] = '\0';

    // Reverse numStr
    for (int j = 0; j < i / 2; j++) {
        char tempChar = numStr[j];
        numStr[j] = numStr[i - j - 1];
        numStr[i - j - 1] = tempChar;
    }

    // Put the parts together for the file name string
    strcpy(fileName, filePre); 
    strcat(fileName, numStr);
    strcat(fileName, filePost);

    // Print the generated file name
    printf("Generated file name: %s\n", fileName);

    // Open the file for writing
    int fd = open(fileName, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    // Generate 60 random values and write them to the file
    int num;
    for (int i = 0; i < 60; i++) {
        num = rand() % 100; // Generate a random number between 0 and 100
        write(fd, &num, sizeof(int));
    }

    close(fd);
    exit(X);
}
