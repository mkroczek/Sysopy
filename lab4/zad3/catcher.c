#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

int caugth_signals = 0;
char* sending_mode;

void start_sending(pid_t sender_pid){
    for (int i = 1; i <= caugth_signals + 1; i++){
        int signal = SIGUSR1;
        if (i == caugth_signals + 1) signal = SIGUSR2;
        if(strcmp(sending_mode, "kill") == 0){
            kill(sender_pid, signal);
            printf("Catcher sent signal %d to sender\n", signal);
        }
        else if(strcmp(sending_mode, "sigqueue") == 0){
            union sigval value;
            value.sival_ptr = NULL; 
            value.sival_int = i;
            sigqueue(sender_pid, signal, value);
            printf("Catcher sent signal %d to sender\n", signal);
        }
        else if(strcmp(sending_mode, "sigrt") == 0){
            signal = SIGRTMIN+0;
            if (i == caugth_signals + 1) signal = SIGRTMIN+1;
            kill(sender_pid, signal);
            printf("Catcher sent signal %d to sender\n", signal);
        }
    }
    printf("Catcher received %d signals, and ends work\n", caugth_signals);
    exit(1);
}

void handler_usr1(int sig, siginfo_t *info, void* context){
    if (sig == SIGUSR1 || SIGRTMIN+0){
        caugth_signals += 1;
    }
}

void handler_usr2(int sig, siginfo_t *info, void* context){
    if (sig == SIGUSR2 || SIGRTMIN+1){
        start_sending(info->si_pid);
    }
}

void start_catching(int ppid){
    sigset_t block_set, allowed_set;
    struct sigaction usr2_action, usr1_action;
    int noticed_parent = 0;

    printf("Catcher PID = %d\n", getpid());

    sigfillset(&block_set);
    sigfillset(&allowed_set);
    sigdelset(&allowed_set, SIGUSR1);
    sigdelset(&allowed_set, SIGUSR2);
    sigdelset(&allowed_set, SIGRTMIN+0);
    sigdelset(&allowed_set, SIGRTMIN+1);

    sigprocmask(SIG_SETMASK, &block_set, NULL);

    usr2_action.sa_sigaction = handler_usr2;
    usr2_action.sa_flags = SA_SIGINFO;
    usr1_action.sa_sigaction = handler_usr1;
    usr1_action.sa_flags = SA_SIGINFO;

    // signal(SIGUSR1, handler_usr1);
    sigaction(SIGUSR2, &usr2_action, NULL);
    sigaction(SIGUSR1, &usr1_action, NULL);
    sigaction(SIGRTMIN+1, &usr2_action, NULL);
    sigaction(SIGRTMIN+0, &usr1_action, NULL);

    while(1){
        if (noticed_parent == 0){
            noticed_parent = 1;
            kill(ppid, SIGUSR1); //sends information to parent process, that is ready to catch signals
        }
        sigsuspend(&allowed_set);
    }
}

int main(int argc, char** argv){

    int ppid = atoi(argv[1]);
    sending_mode = argv[2];
    
    printf("Catcher pid = %d\n", getpid());
    start_catching(ppid);

    return 0;
}