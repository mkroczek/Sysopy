#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>

char* n_signals;
int catcher_pid;
char* sending_mode;

void action(int sig){
    printf("I got signal\n");
    char* c_pid = (char*) calloc(10, sizeof(char));
    if (sig == 10){
        if (fork() == 0){
            printf("c_pid = %d\n", catcher_pid);
            sprintf(c_pid, "%d", catcher_pid);
            execlp("./sender", "./sender", catcher_pid, n_signals, sending_mode, "kill",NULL);
        }
    }
}

int main(int argc, char** argv){

    n_signals = argv[1];
    sending_mode = argv[2];
    char* ppid = (char*) calloc(10, sizeof(char));
    // catcher_pid = (char*) calloc(10, sizeof(char));
    signal(10, action);

    printf("Main process pid = %d\n", getpid());

    if (fork() == 0){
        catcher_pid = getpid();
        sprintf(ppid, "%d", getppid());
        execlp("./catcher", "./catcher", ppid, sending_mode, NULL);
    }

    wait(NULL);
    free(ppid);

    return 0;
}