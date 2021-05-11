#ifndef CONFIG_H
#define CONFIG_H

#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <sys/time.h>

#define BAKE_SIZE 2
#define TABLE_SIZE 3
#define N_COOKS 4
#define N_DELIVERERS 3

#define BAKE_SHM "/bake_shm"
#define TABLE_SHM "/table_shm"
#define BAKE_SEM "/bake_sem"
#define TABLE_SEM "/table_sem"

int get_bake_shm();
int get_table_shm();
sem_t* get_bake_sem();
sem_t* get_table_sem();

sem_t* create_bake_sem();
sem_t* create_table_sem();

void intialize_bake(int* bake);
void initialize_table(int* table);

int free_index_bake(int* bake);
int free_index_table(int* table);

int n_pizzas_bake(int* bake);
int n_pizzas_table(int* bake);

int first_pizza_id(int* table);

void* attach_bake();
void* attach_table();

void detach_bake(void* add);
void detach_table(void* add);

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

long long current_timestamp();

#endif