#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

sig_atomic_t player1_ready = 0;
sig_atomic_t player2_ready = 0;
sig_atomic_t player1_win = 0;
sig_atomic_t player2_win = 0;
pid_t player1_pid;
pid_t player2_pid;

int p1Bound;
int p2Bound;


void child1_handler(int sig){
    if (sig == SIGUSR1){
        p1Bound = 1;
    }
    else if (sig == SIGUSR2){
        p1Bound = 2;
    }
    else if (sig == SIGINT){
        p1Bound = 0;
    }
    else if (sig == SIGTERM){
        exit(0);
    }
}

void child2_handler(int sig){
    if (sig == SIGUSR1){
        p2Bound = 1;
    }
    else if (sig == SIGUSR2){
        p2Bound = 2;
    }
    else if (sig == SIGINT){
        p2Bound = 0;
    }
    else if (sig == SIGTERM){
        exit(0);
    }
}


void parent_handler(int sig){
    if (sig == SIGUSR1){
        player1_ready = 1;
    }
    else if(sig ==SIGUSR2){
        player2_ready = 1;
    }
    else if(sig == SIGINT){
        kill(player1_pid, SIGTERM);
        kill(player2_pid, SIGTERM);
        exit(0);
    }
    else if(sig == SIGCHLD){
        wait(NULL); //Reaper
    }
}

void setup_signal_handler(int signum, void (*handler)(int)) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigaction(signum, &sa, NULL);
}

void write_guess_to_file(const char* filename, int guess) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Error opening guess file");
        exit(1);
    }

    char guess_str[10];  // Buffer for the guess string
    int len = 0;
    int temp_guess = guess;

    // Convert integer to string manually
    if (guess == 0) {
        guess_str[len++] = '0';
    } else {
        while (temp_guess > 0) {
            guess_str[len++] = (temp_guess % 10) + '0';  // Convert digit to character
            temp_guess /= 10;
        }
        // Reverse the string to get the correct order
        for (int i = 0; i < len / 2; i++) {
            char temp = guess_str[i];
            guess_str[i] = guess_str[len - i - 1];
            guess_str[len - i - 1] = temp;
        }
    }
    guess_str[len++] = '\n';  // Append newline
    write(fd, guess_str, len); // Write to file
    close(fd);
}

void player1_behavior() {
    setup_signal_handler(SIGUSR1, child1_handler);
    setup_signal_handler(SIGUSR2, child1_handler);
    setup_signal_handler(SIGINT, child1_handler);
    setup_signal_handler(SIGTERM, child1_handler);

    int min = 0, max = 101;
    while (1) {
        // Wait for the parent to signal readiness
        pause();

        // Loop forever - guess loop
        while (1) {
            kill(getppid(),SIGUSR1);
            int guess = (min + max) / 2;  // Player 1's strategy
            
            write_guess_to_file("player1_guess.txt", guess);
            sleep(1);  // Simulate thinking time

            kill(getppid(), SIGUSR1);  // Signal parent with guess

            // Reset flags for handling signals
            player1_ready = 0;

            // Wait for parent's response
            pause();

            // Adjust bounds based on parent response
            if (p1Bound == 1) {
                min = guess;  // Low guess
            } else if (p1Bound == 2) {
                max = guess;  // High guess
            } else {
                break;  // Parent signaled to move to next game
            }
        }
    }
}

void player2_behavior() {
    setup_signal_handler(SIGUSR1, child2_handler);
    setup_signal_handler(SIGUSR2, child2_handler);
    setup_signal_handler(SIGINT, child2_handler);
    setup_signal_handler(SIGTERM, child2_handler);
    srand(getpid());  // Seed RNG with process ID

    int min = 0, max = 101;
    while (1) {
        // Wait for the parent to signal readiness
        pause();

        // Loop forever - guess loop
        while (1) {
            kill(getppid(),SIGUSR2);
            int guess = rand() % (max - min) + min;  // Player 2's strategy

            write_guess_to_file("player2_guess.txt", guess);
            sleep(1);  // Simulate thinking time

            kill(getppid(), SIGUSR2);  // Signal parent with guess

            // Reset flags for handling signals
            player2_ready = 0;

            // Wait for parent's response
            pause();

            // Adjust bounds based on parent response
            if (p2Bound == 1) {
                min = guess;  // Low guess
            } else if (p2Bound == 2) {
                max = guess;  // High guess
            } else {
                break;  // Parent signaled to move to next game
            }
        }
    }
}

int main() {
    setup_signal_handler(SIGINT, parent_handler);
    setup_signal_handler(SIGCHLD, parent_handler);
    setup_signal_handler(SIGUSR1, parent_handler);
    setup_signal_handler(SIGUSR2, parent_handler);
    // Game loop
    for (int game_number = 1; game_number <= 10; game_number++) {
        player1_pid = fork();
        if (player1_pid == 0) {
            player1_behavior();
        }

        player2_pid = fork();
        if (player2_pid == 0) {
            player2_behavior();
        }

        // Parent (Referee) Logic
        sleep(5);  // Wait for children to set up
        kill(player1_pid, SIGUSR1);  // Signal Player 1 to start
        kill(player2_pid, SIGUSR2);  // Signal Player 2 to start

    
        // Wait for players to signal they're ready
        while (!(player1_ready && player2_ready)) {
            pause();
        }

        printf("Game %d: Player 1 wins: %d, Player 2 wins: %d\n", game_number, player1_win, player2_win);
        int target = rand() % 100 + 1;  // Random number between 1 and 100
        printf("Target Number = %d\n", target);

        while (1) {
            // Wait for players to make their guesses
            pause();

            // Read guesses from files
            int fd1 = open("player1_guess.txt", O_RDONLY);
            char guess1_str[10];
            read(fd1, guess1_str, sizeof(guess1_str) - 1);
            close(fd1);
            guess1_str[9] = '\0';  // Ensure null termination
            int guess1 = atoi(guess1_str);

            int fd2 = open("player2_guess.txt", O_RDONLY);
            char guess2_str[10];
            read(fd2, guess2_str, sizeof(guess2_str) - 1);
            close(fd2);
            guess2_str[9] = '\0';  // Ensure null termination
            int guess2 = atoi(guess2_str);

            if (guess1 == target) {
                printf("Player 1 wins this game!\n");
                player1_win++;
                break;  // Exit loop if Player 1 wins
            }
            if (guess2 == target) {
                printf("Player 2 wins this game!\n");
                player2_win++;
                break;  // Exit loop if Player 2 wins
            }

            // Provide feedback to players
            if (guess1 < target) {
                kill(player1_pid, SIGUSR1);  // Low guess
            } else if (guess1 > target) {
                kill(player1_pid, SIGUSR2);  // High guess
            }

            if (guess2 < target) {
                kill(player2_pid, SIGUSR1);  // Low guess
            } else if (guess2 > target) {
                kill(player2_pid, SIGUSR2);  // High guess
            }
        }

        // Reset for next game
        kill(player1_pid, SIGINT);
        kill(player2_pid, SIGINT);
    }

    printf("Final results: Player 1 wins: %d, Player 2 wins: %d\n", player1_win, player2_win);
    kill(player1_pid, SIGTERM);
    kill(player2_pid, SIGTERM);
    return 0;
}
