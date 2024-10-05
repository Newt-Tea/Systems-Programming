#define _POSIX_C_SOURCE 1 // Needed for SIG_BLOCK
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

// Function to handle errors
int checkError(int val, const char *msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

// Signal handlers for parent
void sigchld_handler(int sig) {
    if (sig == SIGCHLD) {
        int status;
        pid_t pid;
        // Clean up after the child process exits
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            printf("Child process %d terminated.\n", pid);
        }
    }
}

void sigint_handler(int sig) {
    if (sig == SIGINT) {
        char response;
        printf("Exit: Are you sure (Y/n)? ");
        scanf(" %c", &response);
        if (response == 'Y' || response == 'y') {
            printf("Exiting...\n");
            exit(0);
        }
    }
}

void sigusr1_handler(int sig) {
    if (sig == SIGUSR1) {
        printf("Warning! roll outside of bounds\n");
    }
}

void sigusr2_handler(int sig) {
    if (sig == SIGUSR2) {
        printf("Warning! pitch outside of bounds\n");
    }
}

// Signal handler for child termination
void sigterm_handler(int sig) {
    if (sig == SIGTERM) {
        printf("Child received SIGTERM, exiting...\n");
        exit(0);
    }
}

void child_process() {
    // Block SIGINT in the child
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    checkError(sigprocmask(SIG_BLOCK, &mask, NULL), "Failed to block SIGINT");

    // Handle SIGTERM in the child
    struct sigaction sa;
    sa.sa_handler = sigterm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    checkError(sigaction(SIGTERM, &sa, NULL), "Failed to set SIGTERM handler");

    // Open the file "angl.dat" to read
    int fd = checkError(open("angl.dat", O_RDONLY), "Failed to open angl.dat");

    double buf[3];  // Buffer to read roll, pitch, yaw triples

    const struct timespec req = {1, 0};  // 1 second sleep
    struct timespec rem;

    // Read triples (roll, pitch, yaw) from the file
    while (checkError(read(fd, buf, sizeof(buf)), "Failed to read angl.dat") > 0) {
        double roll = buf[0];
        double pitch = buf[1];

        // Check roll and pitch bounds
        if (roll < -20.0 || roll > 20.0) {
            checkError(kill(getppid(), SIGUSR1), "Failed to send SIGUSR1");
        }
        if (pitch < -20.0 || pitch > 20.0) {
            checkError(kill(getppid(), SIGUSR2), "Failed to send SIGUSR2");
        }

        // Wait for 1 second before reading the next set
        nanosleep(&req, &rem);
    }

    // Close the file after reading
    checkError(close(fd), "Failed to close angl.dat");

    exit(0); // Ensure the child process terminates after the file is processed
}

int main() {
    pid_t childId;

    // Fork a child processes
        childId = fork();
        pid_t pid;
        pid = getpid();
        
        if (childId == -1) {  // Check if fork failed
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (childId == 0) {  // Child process
            child_process();
        }

    // Parent process
    struct sigaction sa;

    // Handle SIGCHLD to clean up after the child
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    checkError(sigaction(SIGCHLD, &sa, NULL), "Failed to set SIGCHLD handler");

    // Handle SIGINT (Ctrl+C)
    sa.sa_handler = sigint_handler;
    checkError(sigaction(SIGINT, &sa, NULL), "Failed to set SIGINT handler");

    // Handle SIGUSR1 (roll out of bounds)
    sa.sa_handler = sigusr1_handler;
    checkError(sigaction(SIGUSR1, &sa, NULL), "Failed to set SIGUSR1 handler");

    // Handle SIGUSR2 (pitch out of bounds)
    sa.sa_handler = sigusr2_handler;
    checkError(sigaction(SIGUSR2, &sa, NULL), "Failed to set SIGUSR2 handler");

    // Parent process: use waitpid in a loop to handle multiple children
    int status;
    while (waitpid(-1, &status, 0) > 0) {
        printf("Handled a child termination.\n");
    }

    // Handle potential errors in waitpid
    if (errno != ECHILD) {
        perror("waitpid");
        exit(EXIT_FAILURE);
    }

    printf("No more child processes to wait for. Exiting.\n");
    return 0;
}
