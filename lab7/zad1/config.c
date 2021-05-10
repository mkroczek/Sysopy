#include "config.h"

//GET METHODS
int get_bake_shm(){
    key_t bake_shm_key = ftok(getenv("HOME"), BAKE_SHM_ID);
    return shmget(bake_shm_key, 0, 0);
}

int get_table_shm(){
    key_t table_shm_key = ftok(getenv("HOME"), TABLE_SHM_ID);
    return shmget(table_shm_key, 0, 0);
}

int get_bake_sem(){
    key_t bake_sem_key = ftok(getenv("HOME"), BAKE_SEM_ID);
    return semget(bake_sem_key, 0, 0);
}

int get_table_sem(){
    key_t table_sem_key = ftok(getenv("HOME"), TABLE_SEM_ID);
    return semget(table_sem_key, 0, 0);
}

//CREATE METHODS
int create_bake_shm(){
    key_t bake_shm_key = ftok(getenv("HOME"), BAKE_SHM_ID);
    return shmget(bake_shm_key, BAKE_SIZE, IPC_CREAT | 0666);
}

int create_table_shm(){
    key_t table_shm_key = ftok(getenv("HOME"), TABLE_SHM_ID);
    return shmget(table_shm_key, TABLE_SIZE, IPC_CREAT | 0666);
}

int create_bake_sem(){
    key_t bake_sem_key = ftok(getenv("HOME"), BAKE_SEM_ID);
    return semget(bake_sem_key, 2, IPC_CREAT | 0666);
}

int create_table_sem(){
    key_t table_sem_key = ftok(getenv("HOME"), TABLE_SEM_ID);
    return semget(table_sem_key, 2, IPC_CREAT | 0666);
}

//USAGE METHODS
void intialize_bake(int* bake){
    for (int i = 0; i < BAKE_SIZE; i++){
        bake[i] = -1;
    }
}

void initialize_table(int* table){
    for (int i = 0; i < TABLE_SIZE; i++){
        table[i] = -1;
    }
}

int free_index_bake(int* bake){
    for (int i = 0; i < BAKE_SIZE; i++){
        if (bake[i] == -1)
            return i;
    }
    return -1;
}

int free_index_table(int* table){
    for (int i = 0; i < TABLE_SIZE; i++){
        if (table[i] == -1)
            return i;
    }
    return -1;
}

int n_pizzas_bake(int* bake){
    int counter = 0;
    for (int i = 0; i < BAKE_SIZE; i++){
        if (bake[i] != -1)
            counter++;
    }
    return counter;
}

int n_pizzas_table(int* table){
    int counter = 0;
    for (int i = 0; i < TABLE_SIZE; i++){
        if (table[i] != -1)
            counter++;
    }
    return counter;
}

int first_pizza_id(int* table){
    for (int i = 0; i < TABLE_SIZE; i++){
        if (table[i] != -1)
            return i;
    }
    return -1;
}