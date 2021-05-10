#include "config.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int bake_sem;
int table_sem;

int prepare_pizza(){
    int pizza_type = abs(rand()*getpid()%10);
    //prepare
    printf("(%d %ld) Przygotowuje pizze: %d\n", getpid(), time(NULL), pizza_type);
    sleep(rand()%2 + 1);
    return pizza_type;
}

int place(int pizza){
    int placed = 0;
    int index = -1;

    struct sembuf sops;
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;

    struct sembuf ret_buf;
    ret_buf.sem_num = 0;
    ret_buf.sem_op = 1;
    ret_buf.sem_flg = 0;

    while(placed == 0){
        //reserve access to bake
        semop(bake_sem, &sops, 1);
        int* bake = shmat(get_bake_shm(), NULL, 0);
        if (n_pizzas_bake(bake) < BAKE_SIZE){
            index = free_index_bake(bake);
            bake[index] = pizza;
            int n_pizzas = n_pizzas_bake(bake);
            printf("(%d %ld) Dodalem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), time(NULL), pizza, n_pizzas);
            placed = 1;
        }
        shmdt(bake);
        //return access to bake
        semop(bake_sem, &ret_buf, 1);
        if (placed == 0){
            sleep(1);
        }

    }

    return index;
}

void bake_pizza(){
    sleep(rand()%2 + 4);
}

void issue(int index){
    int placed = 0;

    struct sembuf sops;
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;

    struct sembuf ret_buf;
    ret_buf.sem_num = 0;
    ret_buf.sem_op = 1;
    ret_buf.sem_flg = 0;

    //get access to bake
    semop(bake_sem, &sops, 1);
    int* bake = shmat(get_bake_shm(), NULL, 0);
    int pizza = bake[index];
    bake[index] = -1;
    int n_pizzas = n_pizzas_bake(bake);
    shmdt(bake);
    
    //return access to bake
    semop(bake_sem, &ret_buf, 1);

    while(placed == 0){
        //loop until you place pizza on table
        semop(table_sem, &sops, 1);
        int* table = shmat(get_table_shm(), NULL, 0);
        if (n_pizzas_table(table) < TABLE_SIZE){
            index = free_index_table(table);
            table[index] = pizza;
            int n_pizzas_tab = n_pizzas_table(table);
            printf("(%d %ld) Wyjmuje pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(), time(NULL), pizza, n_pizzas, n_pizzas_tab);
            placed = 1;
        }
        shmdt(table);
        //return access to table
        semop(table_sem, &ret_buf, 1);
        if (placed == 0){
            sleep(1);
        }

    }
}

int main(int argc, char** argv){
    srand(time(NULL));

    //initialize
    bake_sem = get_bake_sem();
    table_sem = get_table_sem();

    while(1){
        int pizza = prepare_pizza();
        int index = place(pizza);
        bake_pizza();
        issue(index);
    }

    return 0;
}