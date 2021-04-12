#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int n_signals;
int caught_signals = 0;
int catcher_pid;
int ppid;

void handler_usr1(int sig){
    if (sig == SIGUSR1){
        printf("Sender caught signal\n");
        caught_signals += 1;
    }
}

void handler_usr2(int sig){
    if (sig == SIGUSR2){
        printf("Caught %d signals, but should get %d\n", caught_signals, n_signals);
        kill(ppid, SIGUSR2);
        exit(1);
    }
}

void start_catching(){
    printf("Start catching started\n");
    sigset_t block_set, allowed_set;

    sigfillset(&block_set);
    sigfillset(&allowed_set);
    sigdelset(&allowed_set, SIGUSR1);
    sigdelset(&allowed_set, SIGUSR2);

    sigprocmask(SIG_SETMASK, &block_set, NULL);

    while(1){
        sigsuspend(&allowed_set);
    }
}

void send_kill(){
    for (int i = 1; i <= n_signals; i++){
        kill(catcher_pid, SIGUSR1);
    }
    printf("Now I should start catching\n");
    kill(catcher_pid, SIGUSR2);
    start_catching();
}

void send_sigqueue(){
    
}

void send_sigrt(){
    
}

int main(int argc, char** argv){

    catcher_pid = atoi(argv[1]);
    n_signals = atoi(argv[2]);
    char* sending_mode = argv[3];
    ppid = atoi(argv[4]);

    signal(SIGUSR2, handler_usr2);
    signal(SIGUSR1, handler_usr1);

    if(strcmp(sending_mode, "kill") == 0){
        send_kill();
    }
    else if(strcmp(sending_mode, "sigqueue") == 0){
        send_sigqueue();
    }
    else if(strcmp(sending_mode, "sigrt") == 0){
        send_sigrt();
    }
    
    return 0;
}