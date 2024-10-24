#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Global variables
int guess[2];
int cmp[2];
int rdy[4];
pthread_mutex_t mtx[3];
pthread_cond_t cnd[3];

void* player1(void* arg) {
    int min, max;
    while (1) {
        pthread_mutex_lock(&mtx[2]);
        while (rdy[2] == 0) {
            pthread_cond_wait(&cnd[2], &mtx[2]);
        }
        rdy[2] = 0;
        pthread_mutex_unlock(&mtx[2]);

        min = 0;
        max = 100;
        while (1) {
            guess[0] = (min + max) / 2;
            printf("Player 1 guesses: %d\n", guess[0]);

            pthread_mutex_lock(&mtx[0]);
            rdy[0] = 1;
            pthread_cond_signal(&cnd[0]);
            while (rdy[0] == 1) {
                pthread_cond_wait(&cnd[0], &mtx[0]);
            }
            pthread_mutex_unlock(&mtx[0]);

            if (cmp[0] == 0) break;
            else if (cmp[0] < 0) min = guess[0] + 1;
            else max = guess[0] - 1;
        }
    }
    return NULL;
}

void* player2(void* arg) {
    int min, max;
    srand(time(NULL) + 1); // Different seed for player 2
    while (1) {
        pthread_mutex_lock(&mtx[2]);
        while (rdy[3] == 0) {
            pthread_cond_wait(&cnd[2], &mtx[2]);
        }
        rdy[3] = 0;
        pthread_mutex_unlock(&mtx[2]);

        min = 0;
        max = 100;
        while (1) {
            guess[1] = min + rand() % (max - min + 1);
            printf("Player 2 guesses: %d\n", guess[1]);

            pthread_mutex_lock(&mtx[1]);
            rdy[1] = 1;
            pthread_cond_signal(&cnd[1]);
            while (rdy[1] == 1) {
                pthread_cond_wait(&cnd[1], &mtx[1]);
            }
            pthread_mutex_unlock(&mtx[1]);

            if (cmp[1] == 0) break;
            else if (cmp[1] < 0) min = guess[1] + 1;
            else max = guess[1] - 1;
        }
    }
    return NULL;
}

void* referee(void* arg) {
    for (int game = 0; game < 10; game++) {
        int target = 1 + rand() % 100;
        printf("Game %d: Target is %d\n", game + 1, target);

        pthread_mutex_lock(&mtx[2]);
        rdy[2] = 1;
        rdy[3] = 1;
        pthread_cond_broadcast(&cnd[2]);
        pthread_mutex_unlock(&mtx[2]);

        while (1) {
            sleep(1);

            pthread_mutex_lock(&mtx[0]);
            pthread_mutex_lock(&mtx[1]);

            cmp[0] = guess[0] - target;
            cmp[1] = guess[1] - target;

            if (cmp[0] == 0 || cmp[1] == 0) {
                cmp[0] = cmp[1] = 0;
                rdy[0] = rdy[1] = 0;
                pthread_cond_signal(&cnd[0]);
                pthread_cond_signal(&cnd[1]);
                pthread_mutex_unlock(&mtx[0]);
                pthread_mutex_unlock(&mtx[1]);
                break;
            }

            rdy[0] = 0;
            rdy[1] = 0;
            pthread_cond_signal(&cnd[0]);
            pthread_cond_signal(&cnd[1]);

            pthread_mutex_unlock(&mtx[0]);
            pthread_mutex_unlock(&mtx[1]);
        }

        printf("Game %d finished.\n", game + 1);
    }
    return NULL;
}

int main() {
    srand(time(NULL)); // Seed the random number generator

    // Initialize mutexes and condition variables
    for (int i = 0; i < 3; i++) {
        pthread_mutex_init(&mtx[i], NULL);
        pthread_cond_init(&cnd[i], NULL);
    }

    // Initialize rdy flags
    for (int i = 0; i < 4; i++) {
        rdy[i] = 0;
    }

    // Create threads
    pthread_t tid1, tid2, tidRef;
    pthread_create(&tid1, NULL, player1, NULL);
    pthread_create(&tid2, NULL, player2, NULL);
    pthread_create(&tidRef, NULL, referee, NULL);

    // Wait for the referee thread to finish
    pthread_join(tidRef, NULL);

    // Clean up
    for (int i = 0; i < 3; i++) {
        pthread_mutex_destroy(&mtx[i]);
        pthread_cond_destroy(&cnd[i]);
    }

    return 0;
}