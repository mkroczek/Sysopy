#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int n_signals;
int caught_signals = 0;
int catcher_pid;
int ppid;
int catching = 0;
int sending_confirmed = 0;

void handler_usr1(int sig, siginfo_t *info, void* context){
    if (!catching){
        sending_confirmed = 1;
    }
    else if (sig == SIGUSR1 || sig == SIGRTMIN + 0){
        if (info->si_value.sival_int > 0){
            printf("For now sender knows, that catcher sent %d signals\n", info->si_value.sival_int);
        }
        caught_signals += 1;
        kill(catcher_pid, SIGUSR1);
    }
}

void handler_usr2(int sig){
    if (sig == SIGUSR2 || sig == SIGRTMIN + 1){
        printf("Caught %d signals, but should get %d\n", caught_signals, n_signals);
        kill(ppid, SIGUSR2);
        exit(1);
    }
}

void start_catching(){
    sigset_t block_set, allowed_set;

    sigfillset(&block_set);
    sigfillset(&allowed_set);
    sigdelset(&allowed_set, SIGUSR1);
    sigdelset(&allowed_set, SIGUSR2);
    sigdelset(&allowed_set, SIGRTMIN+0);
    sigdelset(&allowed_set, SIGRTMIN+1);

    sigprocmask(SIG_SETMASK, &block_set, NULL);

    if (catching){
        while(1){
            sigsuspend(&allowed_set);
        }
        return;
    }
    while(!sending_confirmed){
        sigsuspend(&allowed_set);
    }
}

void send_kill(){
    for (int i = 1; i <= n_signals; i++){
        sending_confirmed = 0;
        kill(catcher_pid, SIGUSR1);
        start_catching();
    }
    catching = 1;
    kill(catcher_pid, SIGUSR2);
    start_catching();
}

void send_sigqueue(){
    union sigval value;
    value.sival_ptr = NULL;
    for (int i = 1; i <= n_signals; i++){
        sending_confirmed = 0;
        value.sival_int = i;
        sigqueue(catcher_pid, SIGUSR1, value);
        start_catching();
    }
    catching = 1;
    value.sival_int = 1;
    sigqueue(catcher_pid, SIGUSR2, value);
    start_catching();
}

void send_sigrt(){
    for (int i = 1; i <= n_signals; i++){
        sending_confirmed = 0;
        kill(catcher_pid, SIGRTMIN + 0);
        start_catching();
    }
    catching = 1;
    kill(catcher_pid, SIGRTMIN + 1);
    start_catching();
}

int main(int argc, char** argv){

    catcher_pid = atoi(argv[1]);
    n_signals = atoi(argv[2]);
    char* sending_mode = argv[3];
    ppid = atoi(argv[4]);
    struct sigaction usr1_action;

    usr1_action.sa_sigaction = handler_usr1;
    usr1_action.sa_flags = SA_SIGINFO;

    sigaction(SIGUSR1, &usr1_action, NULL);
    sigaction(SIGRTMIN+0, &usr1_action, NULL);
    signal(SIGUSR2, handler_usr2);
    signal(SIGRTMIN+1, handler_usr2);

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