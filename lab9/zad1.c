#include "zad1.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

int n_trips = 0;
int n_waiting_elves = 0;
int n_back_reindeers = 0;

pthread_mutex_t variables_mutex;

pthread_cond_t elves_cond;
pthread_cond_t reindeers_cond;
pthread_cond_t santa_cond;

int rand_time(int from, int to){
    return rand()%(to-from+1)+from;
}

void* work_elve(void* arg){
    while(1){
        sleep(rand_time(2, 5));
        pthread_mutex_lock(&variables_mutex);
        if (n_waiting_elves >= 3){
            while (n_waiting_elves > 0){
                // wait until all waiting elves will come back
                printf("Elf: czeka na powrot elfow, %ld\n", pthread_self());
                pthread_cond_wait(&elves_cond, &variables_mutex);
            }
        }
        
        n_waiting_elves += 1;
        printf("Elf: czeka %d elfow na Mikolaja, %ld\n", n_waiting_elves, pthread_self());
        if (n_waiting_elves == 3){
            printf("Elf: wybudzam Mikolaja, %ld\n", pthread_self());
            pthread_cond_signal(&santa_cond);
        }
        while (n_waiting_elves > 0){
            pthread_cond_wait(&elves_cond, &variables_mutex);
        }
        printf("Elf: MIkolaj rozwiazal moj problem, %ld\n", pthread_self());
        printf("Elf: Wracam do pracy, %ld\n", pthread_self());
        pthread_mutex_unlock(&variables_mutex);
    }
    return NULL;
}

void* work_reindeer(void* arg){
    while(1){
        sleep(rand_time(5,10));
        pthread_mutex_lock(&variables_mutex);
        n_back_reindeers += 1;
        printf("Renifer: czeka %d reniferow na Mikolaja, %ld\n", n_back_reindeers, pthread_self());
        if (n_back_reindeers == N_REINDEERS){
            printf("Renifer: wybudzam Mikolaja, %ld\n", pthread_self());
            pthread_cond_signal(&santa_cond);
        }
        while(n_back_reindeers > 0){
            pthread_cond_wait(&reindeers_cond, &variables_mutex);
        }
        printf("Lece na wakacje, %ld\n", pthread_self());

        pthread_mutex_unlock(&variables_mutex);
    }
    return NULL;
}

void* work_santa(void* arg){
    while(n_trips < SANTA_TRIPS){
        pthread_mutex_lock(&variables_mutex);

        while (n_back_reindeers < N_REINDEERS && n_waiting_elves != 3){
            pthread_cond_wait(&santa_cond, &variables_mutex);
        }
        if (n_back_reindeers == N_REINDEERS){
            printf("Mikolaj: dostarczam zabawki, %ld\n", pthread_self());
            sleep(3);
            n_back_reindeers = 0;
            pthread_cond_broadcast(&reindeers_cond);
            n_trips += 1;
        }
        else if (n_waiting_elves == 3){
            printf("Mikolaj: rozwiazuje problemy elfow %ld\n", pthread_self());
            sleep(2);
            n_waiting_elves = 0;
            pthread_cond_broadcast(&elves_cond);
        }
        printf("Mikolaj: zasypiam, %ld\n", pthread_self());

        pthread_mutex_unlock(&variables_mutex);
    }
    exit(0);
    return NULL;
}

int main(int argc, char** argv){
    srand(time(NULL));
    int n_threads = N_ELVES + N_REINDEERS + 1;
    pthread_t threads[n_threads];
    pthread_mutex_init(&variables_mutex, NULL);
    pthread_cond_init(&elves_cond, NULL);
    pthread_cond_init(&reindeers_cond, NULL);
    pthread_cond_init(&santa_cond, NULL);

    for (int i = 0; i < n_threads; i++){
        if (i < N_ELVES){
            pthread_create(&threads[i], NULL, &work_elve, NULL);
        }
        else if (i >= N_ELVES && i < N_ELVES + N_REINDEERS){
            pthread_create(&threads[i], NULL, &work_reindeer, NULL);
        }
        else
            pthread_create(&threads[i], NULL, &work_santa, NULL);
    }

    for(int i = 0; i < n_threads; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&variables_mutex);
    pthread_cond_destroy(&elves_cond);
    pthread_cond_destroy(&reindeers_cond);
    pthread_cond_destroy(&santa_cond);
    return 0;
}