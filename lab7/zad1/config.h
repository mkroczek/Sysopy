#ifndef CONFIG_H
#define CONFIG_H

#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <time.h>
#include <string.h>

#define BAKE_SIZE 2
#define TABLE_SIZE 3
#define N_COOKS 4
#define N_DELIVERERS 3

#define BAKE_SHM_ID 1
#define TABLE_SHM_ID 2
#define BAKE_SEM_ID 3
#define TABLE_SEM_ID 4

int get_bake_shm();
int get_table_shm();
int get_bake_sem();
int get_table_sem();

int create_bake_shm();
int create_table_shm();
int create_bake_sem();
int create_table_sem();

void intialize_bake(int* bake);
void initialize_table(int* table);

int free_index_bake(int* bake);
int free_index_table(int* table);

int n_pizzas_bake(int* bake);
int n_pizzas_table(int* bake);

int first_pizza_id(int* table);

union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

#endif