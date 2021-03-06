#include "config.h"
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int table_sem;

int take(){
    int took = 0;
    int pizza = -1;

    //reserve access to table
    struct sembuf sops;
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = 0;

    struct sembuf ret_buf;
    ret_buf.sem_num = 0;
    ret_buf.sem_op = 1;
    ret_buf.sem_flg = 0;

    while(took == 0){
        //get access to table
        semop(table_sem, &sops, 1);
        int* table = shmat(get_table_shm(), NULL, 0);
        if (n_pizzas_table(table) > 0){
            int index = first_pizza_id(table);
            pizza = table[index];
            table[index] = -1;
            int n_pizzas = n_pizzas_table(table);
            printf("(%d %lld) Pobieram pizze: %d Liczba pizz na stole: %d\n", getpid(), current_timestamp(), pizza, n_pizzas);
            took = 1;
        }
        shmdt(table);
        //return access to table
        semop(table_sem, &ret_buf, 1);
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