#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

char* n_signals;
int catcher_pid;
char* sending_mode;
int endend = 0;

void action(int sig){
    if (sig == 10){
        char* c_pid = (char*) calloc(10, sizeof(char));
        char* ppid = (char*) calloc(10, sizeof(char));
        if (fork() == 0){
            sprintf(c_pid, "%d", catcher_pid);
            sprintf(ppid, "%d", getppid());
            execlp("./sender", "./sender", c_pid, n_signals, sending_mode, ppid, NULL);
        }
        free(c_pid);
        free(ppid);
    }
    else if(sig == 12){
        endend = 1;
    }
}

int main(int argc, char** argv){

    n_signals = argv[1];
    sending_mode = argv[2];
    char* ppid = (char*) calloc(10, sizeof(char));
    signal(SIGUSR1, action);
    signal(SIGUSR2, action);

    if ((catcher_pid = fork()) == 0){
        sprintf(ppid, "%d", getppid());
        execlp("./catcher", "./catcher", ppid, sending_mode, NULL);
    }

    while(1){
        if (endend == 1){
            free(ppid);
            printf("Main program finished execution.\n");
            return 0;
        }
    }
}