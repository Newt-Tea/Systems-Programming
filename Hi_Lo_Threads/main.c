#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#define NUM_GAMES 10

// Global variables
int guess[2];
int cmp[2];
int rdy[4];
pthread_mutex_t mtx[3];
pthread_cond_t cnd[3];
int player1_win = 0;
int player2_win = 0;

void* player1_behavior(void* arg) {
    int min = 0, max = 100;
    while (1) {
        // Wait for the referee to signal readiness
        pthread_mutex_lock(&mtx[2]);
        while (!rdy[2]) {
            pthread_cond_wait(&cnd[2], &mtx[2]);
        }
        pthread_mutex_unlock(&mtx[2]);
        rdy[2] = 0;

        // Guess loop
        while (1) {
            int guess_value = (min + max) / 2;  // Player 1's strategy
            guess[0] = guess_value;

            // Signal the referee with the guess
            pthread_mutex_lock(&mtx[0]);
            rdy[0] = 1;
            printf("Player 1 Guessed: %i\t", guess_value);
            pthread_cond_signal(&cnd[0]);
            pthread_mutex_unlock(&mtx[0]);

            // Wait for the referee's response
            pthread_mutex_lock(&mtx[0]);
            while (rdy[0] == 1) {
                pthread_cond_wait(&cnd[0], &mtx[0]);
            }
            pthread_mutex_unlock(&mtx[0]);

            // Adjust bounds based on referee's response
            if (cmp[0] < 0) {
                min = guess_value;
            } else if (cmp[0] > 0) {
                max = guess_value;
            } else {
                break;  // Correct guess
            }
        }
    }
    return NULL;
}

void* player2_behavior(void* arg) {
    int min = 0, max = 100;
    srand(time(NULL) + 1);  // Seed RNG with a different seed
    while (1) {
        // Wait for the referee to signal readiness
        pthread_mutex_lock(&mtx[2]);
        while (rdy[3] == 0) {
            pthread_cond_wait(&cnd[2], &mtx[2]);
        }
        rdy[3] = 0;
        pthread_mutex_unlock(&mtx[2]);

        // Guess loop
        while (1) {
            int guess_value = rand() % (max - min + 1) + min;  // Player 2's strategy
            guess[1] = guess_value;

            // Signal the referee with the guess
            pthread_mutex_lock(&mtx[1]);
            rdy[1] = 1;
            printf("Player 2 Guessed: %i\n", guess_value);
            pthread_cond_signal(&cnd[1]);
            pthread_mutex_unlock(&mtx[1]);

            // Wait for the referee's response
            pthread_mutex_lock(&mtx[1]);
            while (rdy[1] == 1) {
                pthread_cond_wait(&cnd[1], &mtx[1]);
            }
            pthread_mutex_unlock(&mtx[1]);

            // Adjust bounds based on referee's response
            if (cmp[1] < 0) {
                min = guess_value;
            } else if (cmp[1] > 0) {
                max = guess_value;
            } else {
                break;  // Correct guess
            }
        }
    }
    return NULL;
}

void* referee_behavior(void* arg) {
    for (int game_number = 1; game_number <= NUM_GAMES; game_number++) {
        // Generate the target number
        int target = rand() % 100 + 1;

        // Signal players to start
        pthread_mutex_lock(&mtx[2]);
        rdy[2] = 1;
        rdy[3] = 1;
        pthread_cond_broadcast(&cnd[2]);
        pthread_mutex_unlock(&mtx[2]);

        printf("Game %d: Target Number = %d\n", game_number, target);

        while (1) {
            // Wait for Player 1's guess
            pthread_mutex_lock(&mtx[0]);
            while (rdy[0] == 0) {
                pthread_cond_wait(&cnd[0], &mtx[0]);
            }
            rdy[0] = 0;
            pthread_mutex_unlock(&mtx[0]);

            // Wait for Player 2's guess
            pthread_mutex_lock(&mtx[1]);
            while (rdy[1] == 0) {
                pthread_cond_wait(&cnd[1], &mtx[1]);
            }
            rdy[1] = 0;
            pthread_mutex_unlock(&mtx[1]);

            // cmp guesses to the target
            cmp[0] = guess[0] - target;
            cmp[1] = guess[1] - target;

            // Check for winners
            if (cmp[0] == 0 || cmp[1] == 0) {
                if (cmp[0] == 0) {
                    printf("Player 1 wins this game!\n");
                    player1_win++;
                }
                if (cmp[1] == 0) {
                    printf("Player 2 wins this game!\n");
                    player2_win++;
                }
                break;
            }

            // Signal players with feedback
            pthread_mutex_lock(&mtx[0]);
            pthread_cond_signal(&cnd[0]);
            pthread_mutex_unlock(&mtx[0]);

            pthread_mutex_lock(&mtx[1]);
            pthread_cond_signal(&cnd[1]);
            pthread_mutex_unlock(&mtx[1]);
        }

        printf("Game %d: Player 1 wins: %d, Player 2 wins: %d\n", game_number, player1_win, player2_win);
    }
    return NULL;
}

int main() {
    srand(time(NULL));  // Seed the random number generator

    // Initialize mutexes and condition variables
    for (int i = 0; i < 3; i++) {
        pthread_mutex_init(&mtx[i], NULL);
        pthread_cond_init(&cnd[i], NULL);
    }

    // Initialize readiness flags
    for (int i = 0; i < 4; i++) {
        rdy[i] = 0;
    }

    // Create threads
    pthread_t player1_thread, player2_thread, referee_thread;
    pthread_create(&player1_thread, NULL, player1_behavior, NULL);
    pthread_create(&player2_thread, NULL, player2_behavior, NULL);
    pthread_create(&referee_thread, NULL, referee_behavior, NULL);

    // Wait for the referee thread to finish
    pthread_join(referee_thread, NULL);

    // Clean up
    for (int i = 0; i < 3; i++) {
        pthread_mutex_destroy(&mtx[i]);
        pthread_cond_destroy(&cnd[i]);
    }

    printf("Final results: Player 1 wins: %d, Player 2 wins: %d\n", player1_win, player2_win);
    return 0;
}