#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>

int take(){
    // take pizza from table

    printf("(%d %ld) Pobieram pizze: %d Liczba pizz na stole: %d\n", getpid(), time(NULL), 1, 2);
    return 0;
}

void drive(){
    sleep(rand()%2 + 4);
}

void deliver(int pizza){
    printf("(%d %ld) Dostarczam pizze: n\n", getpid(), time(NULL));
}

int main(int argc, char** argv){
    srand(time(NULL));
    int pizza = take();
    drive();
    deliver(pizza);
    drive();

    return 0;
}