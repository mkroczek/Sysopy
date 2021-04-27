#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int prepare_pizza(){
    int pizza_type = rand()%10;
    //prepare
    printf("(%d %ld) Przygotowuje pizze: %d\n", getpid(), time(NULL), pizza_type);
    sleep(rand()%2 + 1);
    return pizza_type;
}

void place(int pizza){
    printf("(%d %ld) Dodalem pizze: %d. Liczba pizz w piecu: m\n", getpid(), time(NULL), pizza);
}

void bake(){
    sleep(rand()%2 + 4);
}

void issue(int pizza){
    printf("(%d %ld) Wyjmuje pizze: %d. Liczba pizz w piecu: m. Liczba pizz na stole: k\n", getpid(), time(NULL), pizza);
}

int main(int argc, char** argv){
    srand(time(NULL));
    int pizza = prepare_pizza();
    place(pizza);
    bake();
    issue(pizza);

    return 0;
}