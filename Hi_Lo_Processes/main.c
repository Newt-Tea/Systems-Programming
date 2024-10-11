#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

// Flags to keep track of readiness and guess status
int player1_ready = 0;
int player2_ready = 0;
int player1_guess_ready = 0;
int player2_guess_ready = 0;
int guess_flag = 0;  // 0,Low 1,High 2,Reset

// Error handling function
int checkError(int val, const char *msg) {
    if (val == -1) {
        perror(msg);
        exit(EXIT_FAILURE);
    }
    return val;
}

// Signal handlers for players
void guess_signal_handler(int signum) {
    if (signum == SIGUSR1)
        guess_flag = 0;
    else if (signum == SIGUSR2)
        guess_flag = 1;
    else if (signum == SIGINT)
        guess_flag = 2;
}

// Parent signal handlers
void sigusr1_handler(int signum) {
    if (!player1_ready)
        player1_ready = 1;  // Player 1 is ready to start
    else
        player1_guess_ready = 1;  // Player 1 guess is ready
}

void sigusr2_handler(int signum) {
    if (!player2_ready)
        player2_ready = 1;  // Player 2 is ready to start
    else
        player2_guess_ready = 1;  // Player 2 guess is ready
}

void sigchld_handler(int signum) {
    while (waitpid(-1, NULL, WNOHANG) > 0);  // Reap child processes
}

void sigint_handler(int signum) {
    // Send SIGTERM to both children and terminate the game
    kill(0, SIGTERM);
    exit(0);
}

// Set up parent signal handlers
void setup_parent_signals() {
    struct sigaction sa;

    sa.sa_handler = sigusr1_handler;
    checkError(sigaction(SIGUSR1, &sa, NULL), "sigaction SIGUSR1");

    sa.sa_handler = sigusr2_handler;
    checkError(sigaction(SIGUSR2, &sa, NULL), "sigaction SIGUSR2");

    sa.sa_handler = sigchld_handler;
    checkError(sigaction(SIGCHLD, &sa, NULL), "sigaction SIGCHLD");

    sa.sa_handler = sigint_handler;
    checkError(sigaction(SIGINT, &sa, NULL), "sigaction SIGINT");
}

// Low-level file operations
void write_guess(const char *filename, int guess) {
    int fd = checkError(open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644), "open write_guess");

    char buffer[10];
    int len = snprintf(buffer, sizeof(buffer), "%d\n", guess);
    
    checkError(write(fd, buffer, len), "write guess");
    checkError(close(fd), "close write_guess");
}

int read_guess(const char *filename) {
    int fd = checkError(open(filename, O_RDONLY), "open read_guess");

    char buffer[10];
    int len = checkError(read(fd, buffer, sizeof(buffer) - 1), "read guess");
    buffer[len] = '\0';  // Null-terminate the string

    checkError(close(fd), "close read_guess");

    return atoi(buffer);  // Convert the string to an integer and return it
}

// Player 1 logic (binary search strategy)
void player1() {
    int min = 0, max = 101, guess;

    struct sigaction sa;
    sa.sa_handler = guess_signal_handler;
    checkError(sigaction(SIGUSR1, &sa, NULL), "sigaction SIGUSR1");
    checkError(sigaction(SIGUSR2, &sa, NULL), "sigaction SIGUSR2");
    checkError(sigaction(SIGINT, &sa, NULL), "sigaction SIGINT");

    while (1) {
        pause();  // Wait for a signal to start guessing
        min = 0;
        max = 101;

        while (1) {
            guess = (min + max) / 2;  // Binary search method for Player 1

            write_guess("player1_guess.txt", guess);  // Write the guess to the file
            checkError(kill(getpid(), SIGUSR1), "failed to kill process"); //Signal parent that process has written guess

            pause();  // Wait for a response from the parent

            if (guess_flag == 0)
                min = guess;  // Adjust the lower bound
            else if (guess_flag == 1)
                max = guess;  // Adjust the upper bound
            else if (guess_flag == 2)
                break;  // Exit loop and start a new game round
        }
    }
}

// Player 2 logic (random guessing strategy)
void player2() {
    int min = 0, max = 101, guess;

    struct sigaction sa;
    sa.sa_handler = guess_signal_handler;
    checkError(sigaction(SIGUSR1, &sa, NULL), "sigaction SIGUSR1");
    checkError(sigaction(SIGUSR2, &sa, NULL), "sigaction SIGUSR2");
    checkError(sigaction(SIGINT, &sa, NULL), "sigaction SIGINT");

    srand(getpid());  // Seed random number generator with the process ID

    while (1) {
        pause();  // Wait for a signal to start guessing

        min = 0;
        max = 101;

        while (1) {
            guess = (rand() % (max - min)) + min;  // Random guessing method for Player 2

            write_guess("player2_guess.txt", guess);  // Write guess to file
            checkError(kill(getppid(), SIGUSR2), "failed to kill process");

            pause();  // Wait for parent's response

            if (guess_flag == 0)
                min = guess;
            else if (guess_flag == 1)
                max = guess;
            else if (guess_flag == 2)
                break;
        }
    }
}

// Parent (referee) logic
void parent() {
    setup_parent_signals();  // Set up signal handlers

    int child1_pid = checkError(fork(), "fork child1");
    if (child1_pid == 0) {
        player1();  // Player 1 logic in child process
        exit(0);
    }

    int child2_pid = checkError(fork(), "fork child2");
    if (child2_pid == 0) {
        player2();  // Player 2 logic in child process
        exit(0);
    }

    int target, player1_score = 0, player2_score = 0;
    srand(time(NULL));

    for (int game = 1; game <= 10; game++) {
        target = (rand() % 100) + 1;  // Generate random target number

        printf("Game %d, target number: %d\n", game, target);

        player1_ready = 0;
        player2_ready = 0;

        // Send signals to start the game for both players
        checkError(kill(child1_pid, SIGUSR1), "Failed to kill process");
        checkError(kill(child2_pid, SIGUSR2), "Failed to kill process");

        // Wait for both players to signal they are ready
        while (!player1_ready || !player2_ready) {
            pause();  // Wait for signals from both players
        }

        // Referee loop: compare guesses, send signals to adjust bounds
        int game_won = 0;
        while (!game_won) {
            player1_guess_ready = 0;
            player2_guess_ready = 0;

            // Wait for both players to make guesses
            while (!player1_guess_ready || !player2_guess_ready) {
                pause();  // Wait for signals from players indicating guesses are ready
            }

            int guess1 = read_guess("player1_guess.txt");
            int guess2 = read_guess("player2_guess.txt");

            // Compare guesses with target and send feedback signals
            if (guess1 == target) {
                printf("Player 1 wins!\n");
                player1_score++;
                game_won = 1;
            } else if (guess2 == target) {
                printf("Player 2 wins!\n");
                player2_score++;
                game_won = 1;
            } else {
                if (guess1 < target) checkError(kill(child1_pid, SIGUSR1), "Failed to kill process");
                else checkError(kill(child1_pid, SIGUSR2), "Failed to kill process");

                if (guess2 < target) checkError(kill(child2_pid, SIGUSR1), "Failed to kill process");
                else checkError(kill(child2_pid, SIGUSR2), "Failed to kill process");
            }
        }

        // After game is won, send reset signal (SIGINT) to both players
        checkError(kill(child1_pid, SIGINT), "Failed to kill process");
        checkError(kill(child2_pid, SIGINT), "Failed to kill process");
    }

    // Final score display
    printf("Final scores: Player 1: %d, Player 2: %d\n", player1_score, player2_score);

    // Send SIGTERM to terminate both children
    checkError(kill(child1_pid, SIGTERM), "Failed to kill process");
    checkError(kill(child2_pid, SIGTERM), "Failed to kill process");

    // Reap both children
    checkError(wait(NULL), "wait for Player 1");
    checkError(wait(NULL), "wait for Player 2");
}

// Main function to start the game
int main() {
    parent();  // Start the parent (referee)
    return 0;
}
