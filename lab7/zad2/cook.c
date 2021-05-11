#include "config.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

sem_t* bake_sem;
sem_t* table_sem;

int prepare_pizza(){
    int pizza_type = abs(rand()*getpid()%10);
    //prepare
    printf("(%d %lld) Przygotowuje pizze: %d\n", getpid(), current_timestamp(), pizza_type);
    sleep(rand()%2 + 1);
    return pizza_type;
}

int place(int pizza){
    int placed = 0;
    int index = -1;

    while(placed == 0){
        //reserve access to bake
        sem_wait(bake_sem);
        int bake_fd = get_bake_shm();
        int* bake = (int*) attach_bake(bake_fd);
        if (n_pizzas_bake(bake) < BAKE_SIZE){
            index = free_index_bake(bake);
            bake[index] = pizza;
            int n_pizzas = n_pizzas_bake(bake);
            printf("(%d %lld) Dodalem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), current_timestamp(), pizza, n_pizzas);
            placed = 1;
        }
        detach_bake(bake);
        close(bake_fd);
        //return access to bake
        sem_post(bake_sem);
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

    //get access to bake
    sem_wait(bake_sem);
    int bake_fd = get_bake_shm();
    int* bake = (int* )attach_bake(bake_fd);
    int pizza = bake[index];
    bake[index] = -1;
    int n_pizzas = n_pizzas_bake(bake);
    detach_bake(bake);
    close(bake_fd);
    
    //return access to bake
    sem_post(bake_sem);

    while(placed == 0){
        //loop until you place pizza on table
        sem_wait(table_sem);
        int table_fd = get_table_shm();
        int* table = attach_table(table_fd);
        if (n_pizzas_table(table) < TABLE_SIZE){
            index = free_index_table(table);
            table[index] = pizza;
            int n_pizzas_tab = n_pizzas_table(table);
            printf("(%d %lld) Wyjmuje pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(), current_timestamp(), pizza, n_pizzas, n_pizzas_tab);
            placed = 1;
        }
        detach_table(table);
        close(table_fd);
        //return access to table
        sem_post(table_sem);
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