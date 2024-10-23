#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// Global Variables
int guess[2];
int cmp[2];
int rdy[2];
pthread_mutex_t mtx[2];
pthread_cond_t cnd[2];
int player1_win = 0;
int player2_win = 0;

void* player1_behavior(void* arg) {
    int min = 0, max = 100;
    while (1) {
        // Wait for the referee to signal readiness for new game
        pthread_mutex_lock(&mtx[0]);
        while (!rdy[0]) {
            pthread_cond_wait(&cnd[0], &mtx[0]);
        }
        pthread_mutex_unlock(&mtx[0]);

        // Guess loop
        while (1) {
            int guess_value = (min + max) / 2;  // Player 1's strategy
            guess[0] = guess_value;

            // Signal the referee with the guess
            pthread_mutex_lock(&mtx[0]);
            rdy[0] = 0;  // Reset readiness
            pthread_cond_signal(&cnd[0]);
            pthread_mutex_unlock(&mtx[0]);

            // Wait for the referee's response
            pthread_mutex_lock(&mtx[0]);
            while (rdy[0] != 1) {
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
        // Reset readiness for next game
        pthread_mutex_lock(&mtx[0]);
        rdy[0] = 0;
        pthread_mutex_unlock(&mtx[0]);
    }
    return NULL;
}

void* player2_behavior(void* arg) {
    int min = 0, max = 100;
    srand(time(NULL) + 1);
    while (1) {
        // Wait for the referee to signal readiness for new game
        pthread_mutex_lock(&mtx[1]);
        while (!rdy[1]) {
            pthread_cond_wait(&cnd[1], &mtx[1]);
        }
        pthread_mutex_unlock(&mtx[1]);

        // Guess loop
        while (1) {
            int guess_value = rand() % (max - min + 1) + min;  // Player 2's strategy
            guess[1] = guess_value;

            // Signal the referee with the guess
            pthread_mutex_lock(&mtx[1]);
            rdy[1] = 0;  // Reset readiness
            pthread_cond_signal(&cnd[1]);
            pthread_mutex_unlock(&mtx[1]);

            // Wait for the referee's response
            pthread_mutex_lock(&mtx[1]);
            while (rdy[1] != 1) {
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
        // Reset readiness for next game
        pthread_mutex_lock(&mtx[1]);
        rdy[1] = 0;
        pthread_mutex_unlock(&mtx[1]);
    }
    return NULL;
}

void* referee_behavior(void* arg) {
    for (int game_number = 1; game_number <= 10; game_number++) {
        // Generate the target number
        int target = rand() % 100 + 1;

        // Signal players to start
        pthread_mutex_lock(&mtx[0]);
        pthread_mutex_lock(&mtx[1]);
        rdy[0] = 1;
        rdy[1] = 1;
        pthread_cond_signal(&cnd[0]);
        pthread_cond_signal(&cnd[1]);
        pthread_mutex_unlock(&mtx[0]);
        pthread_mutex_unlock(&mtx[1]);

        printf("Game %d: Target Number = %d\n", game_number, target);

        while (1) {
            // Wait for Player 1's guess
            pthread_mutex_lock(&mtx[0]);
            while (rdy[0] == 1) {
                pthread_cond_wait(&cnd[0], &mtx[0]);
            }

            // Wait for Player 2's guess
            pthread_mutex_lock(&mtx[1]);
            while (rdy[1] == 1) {
                pthread_cond_wait(&cnd[1], &mtx[1]);
            }

            // Compare guesses to the target
            cmp[0] = guess[0] - target;
            cmp[1] = guess[1] - target;

            // Determine winners
            if (cmp[0] == 0) {
                printf("Player 1 wins this game!\n");
                player1_win++;
                pthread_mutex_unlock(&mtx[0]);
                pthread_mutex_unlock(&mtx[1]);
                break;
            }
            if (cmp[1] == 0) {
                printf("Player 2 wins this game!\n");
                player2_win++;
                pthread_mutex_unlock(&mtx[0]);
                pthread_mutex_unlock(&mtx[1]);
                break;
            }

            // Signal players with feedback
            rdy[0] = 1;  // Ready for feedback
            pthread_cond_signal(&cnd[0]);
            pthread_mutex_unlock(&mtx[0]);

            rdy[1] = 1;  // Ready for feedback
            pthread_cond_signal(&cnd[1]);
            pthread_mutex_unlock(&mtx[1]);
        }
    }
    return NULL;
}

int main() {
    srand(time(NULL));

    // Initialize mutexes and condition variables
    for (int i = 0; i < 2; i++) {
        pthread_mutex_init(&mtx[i], NULL);
        pthread_cond_init(&cnd[i], NULL);
    }

    // Create threads
    pthread_t player1_thread, player2_thread, referee_thread;
    pthread_create(&player1_thread, NULL, player1_behavior, NULL);
    pthread_create(&player2_thread, NULL, player2_behavior, NULL);
    pthread_create(&referee_thread, NULL, referee_behavior, NULL);

    // Wait for the referee thread to finish
    pthread_join(referee_thread, NULL);

    // Clean up
    for (int i = 0; i < 2; i++) {
        pthread_mutex_destroy(&mtx[i]);
        pthread_cond_destroy(&cnd[i]);
    }

    printf("Final results: Player 1 wins: %d, Player 2 wins: %d\n", player1_win, player2_win);
    return 0;
}