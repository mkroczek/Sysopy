#include "config.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int bake_shm;
int table_shm;
int bake_sem;
int table_sem;
pid_t deliverers[N_DELIVERERS];
pid_t cooks[N_COOKS];

void end_job(){
    for (int i = 0; i < N_COOKS; i++){
        kill(cooks[i], SIGINT);
    }
    for (int i = 0; i < N_DELIVERERS; i++){
        kill(deliverers[i], SIGINT);
    }

    printf("I am finishing my job\n");
    //delete
    shmctl(bake_shm, IPC_RMID, NULL);
    shmctl(table_shm, IPC_RMID, NULL);
    semctl(bake_sem, 0, IPC_RMID, NULL);
    semctl(table_sem, 0, IPC_RMID, NULL);

    exit(0);
}

void sig_handler(int sig){
    if (sig == SIGINT){
        printf("Mam zakonczyc prace\n");
        end_job();
    }
}

void start_job(){
    for(int i = 0; i < N_COOKS; i++){
        cooks[i] = fork();
        if (cooks[i] == 0){
            execlp("./cook", "cook", NULL);
        }
    }
    for(int i = 0; i < N_DELIVERERS; i++){
        deliverers[i] = fork();
        if (deliverers[i] == 0){
            execlp("./deliverer", "deliverer", NULL);
        }
    }

    for(int i = 0 ; i < N_COOKS+N_DELIVERERS; i++){
        wait(NULL);
        printf("Process finished\n");
    }
}

int main(int argc, char** argv){
    signal(SIGINT, sig_handler);

    //initialize shared memory
    bake_shm = create_bake_shm();
    table_shm = create_table_shm();
    int* bake = shmat(bake_shm, NULL, 0);
    int* table = shmat(table_shm, NULL, 0);
    initialize_table(table);
    intialize_bake(bake);
    
    //detach shared memory
    shmdt(bake);
    shmdt(table);

    //initialize semaphores
    bake_sem = create_bake_sem();
    table_sem = create_table_sem();
    union semun arg;
    arg.val = 1;
    semctl(bake_sem, 0, SETVAL, arg);
    semctl(table_sem, 0, SETVAL, arg);

    start_job();
    end_job();
    return 0;
}