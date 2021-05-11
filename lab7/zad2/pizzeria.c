#include "config.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/sem.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

sem_t* bake_sem;
sem_t* table_sem;
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
    shm_unlink(BAKE_SHM);
    shm_unlink(TABLE_SHM);
    sem_close(bake_sem);
    sem_close(table_sem);
    sem_unlink(BAKE_SEM);
    sem_unlink(TABLE_SEM);

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
    int bake_fd = get_bake_shm();
    int table_fd = get_table_shm();
    int* bake = (int*) attach_bake(bake_fd);
    int* table = (int*) attach_table(table_fd);
    initialize_table(table);
    intialize_bake(bake);
    
    //detach shared memory
    detach_table(table);
    detach_bake(bake);
    close(bake_fd);
    close(table_fd);

    //initialize semaphores
    bake_sem = create_bake_sem();
    if (bake_sem == SEM_FAILED){
        perror("Problems with creating semaphore");
    }
    table_sem = create_table_sem();
    

    start_job();
    end_job();
    return 0;
}