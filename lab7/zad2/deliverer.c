#include "config.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

sem_t* table_sem;

int take(){
    int took = 0;
    int pizza = -1;

    while(took == 0){
        //get access to table
        sem_wait(table_sem);
        int table_fd = get_table_shm();
        int* table = attach_table(table_fd);
        if (n_pizzas_table(table) > 0){
            int index = first_pizza_id(table);
            pizza = table[index];
            table[index] = -1;
            int n_pizzas = n_pizzas_table(table);
            printf("(%d %lld) Pobieram pizze: %d Liczba pizz na stole: %d\n", getpid(), current_timestamp(), pizza, n_pizzas);
            took = 1;
        }
        detach_table(table);
        close(table_fd);
        //return access to table
        sem_post(table_sem);
        if (took == 0){
            sleep(1);
        }

    }

    return pizza;
}

void drive(){
    sleep(rand()%2 + 4);
}

void deliver(int pizza){
    drive();
    printf("(%d %lld) Dostarczam pizze: %d\n", getpid(), current_timestamp(), pizza);
    drive();
}

int main(int argc, char** argv){
    srand(time(NULL));

    //initialize
    table_sem = get_table_sem();

    while(1){
        int pizza = take();
        deliver(pizza);
    }

    return 0;
}