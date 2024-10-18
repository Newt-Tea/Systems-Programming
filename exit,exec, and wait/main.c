#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

int main() {
    pid_t pid;
    int status;
    int randomNumber;

    pid = fork();
    if (pid <0) {
        perror("Fork Failed");
        exit(1);
    }

    // If Child
    if (pid == 0) {
        char *args[] = {"./myRand", NULL};
        execvp(args[0], args);
    } 
    // If Parent
    else {
        // Wait for the child process to finish
        wait(&status);
        if (WIFEXITED(status)) {
            randomNumber = WEXITSTATUS(status); // Get the exit status number
        } else {
            printf("Child process didnt exit properly.\n");
            exit(1);
        }
    }

    // Preparing the file name
    char filePre[] = "data";
    char filePost[] = ".dat";
    char tempName[50] = "";
    char numStr[10];

    // Cchange the random number to a string
    int i = 0;
    int temp = randomNumber;
    if (temp == 0) {
        numStr[i++] = '0';
    }
    while (temp > 0) {
        numStr[i++] = (temp % 10) + '0'; // Pull the last digit from the number and convert it
        temp /= 10;
    }
    numStr[i] = '\0';

    // Reverse the string
    for (int j = 0; j < i / 2; j++) {
        char tempChar = numStr[j];
        numStr[j] = numStr[i - j - 1];
        numStr[i - j - 1] = tempChar;
    }

    // Put the parts together for the file name
    strcat(tempName, filePre);
    strcat(tempName, numStr);
    strcat(tempName, filePost);
    int fd = open(tempName, O_RDONLY);
    if (fd <= 0) {
        perror("Failed to open file");
        exit(1);
    }

    // Get the values from the file
    int num;
    int sum = 0;
    for (i = 0; i < 60; i++) {
        if (read(fd, &num, sizeof(int)) != sizeof(int)) {
            perror("Failed to read file");
            close(fd);  
            exit(1);
        }
        printf("Value %d: %d\n", i + 1, num);
        sum += num;
    }

    // Compute the average
    double avg = sum /60.0;
    printf("Average: %f\n", avg);


    close(fd);
    if (unlink(tempName) == -1) {
        perror("Failed to unlink the file");
        exit(1);
    }
    return 0;
}
