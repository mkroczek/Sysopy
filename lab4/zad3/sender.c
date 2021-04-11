#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int n_signals;
int caught_signals = 0;
int catcher_pid;

void handler_usr1(int sig){
    if (sig == SIGUSR1){
        caught_signals += 1;
    }
}

void handler_usr2(int sig){
    if (sig == SIGUSR2){
        printf("Caught %d signals, but should get %d\n", caught_signals, n_signals);
        exit(1);
    }
}

void send_kill(){
    for (int i = 1; i <= n_signals; i++){
        kill(catcher_pid, SIGUSR1);
    }
    kill(catcher_pid, SIGUSR2);
}

void send_sigqueue(){
    
}

void send_sigrt(){
    
}

int main(int argc, char** argv){

    catcher_pid = atoi(argv[1]);
    n_signals = atoi(argv[2]);
    char* sending_mode = argv[3];

    printf("Catcher pid in sender = %d\n", catcher_pid);

    signal(SIGUSR1, handler_usr1);
    signal(SIGUSR2, handler_usr2);

    if(strcmp(sending_mode, "kill") == 0){
        send_kill();
    }
    else if(strcmp(sending_mode, "sigqueue") == 0){
        send_sigqueue();
    }
    else if(strcmp(sending_mode, "sigrt") == 0){
        send_sigrt();
    }

    printf("I am sender and I have to send %d signals\n", n_signals);
    
    return 0;
}