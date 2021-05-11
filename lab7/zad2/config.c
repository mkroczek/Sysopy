#include "config.h"

//GET METHODS
int get_bake_shm(){
    int fd = shm_open(BAKE_SHM, O_CREAT | O_RDWR, 0760);
    ftruncate(fd, BAKE_SIZE*sizeof(int));
    return fd;
}

int get_table_shm(){
    int fd = shm_open(TABLE_SHM, O_CREAT | O_RDWR, 0760);
    ftruncate(fd, TABLE_SIZE*sizeof(int));
    return fd;
}

sem_t* get_bake_sem(){
    return sem_open(BAKE_SEM, 0);
}

sem_t* get_table_sem(){
    return sem_open(TABLE_SEM, 0);
}

sem_t* create_bake_sem(){
    return sem_open(BAKE_SEM, O_CREAT, 0666, 1);
}

sem_t* create_table_sem(){
    return sem_open(TABLE_SEM, O_CREAT, 0666, 1);
}

//USAGE METHODS
void intialize_bake(int* bake){
    for(int i = 0; i < BAKE_SIZE; i++){
        bake[i] = -1;
    }
}

void initialize_table(int* table){
    for(int i = 0; i < TABLE_SIZE; i++){
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
        if (bake[i] != -1){
            counter++;
        }
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

void* attach_bake(int fd){
    return mmap(NULL, BAKE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

void* attach_table(int fd){
    return mmap(NULL, TABLE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
}

void detach_bake(void* addr){
    munmap(addr, BAKE_SIZE*sizeof(int));
}

void detach_table(void* addr){
    munmap(addr, TABLE_SIZE*sizeof(int));
}

long long current_timestamp(){
    struct timeval te;
    gettimeofday(&te, NULL);
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;

}