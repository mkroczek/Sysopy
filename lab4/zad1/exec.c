#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_PASSED SIGUSR2

int main(int argc, char** argv){
    int sig;
    int ppid;

    sig = atoi(argv[2]);
    ppid = atoi(argv[3]);

    if (strcmp(argv[1], "ignore") == 0){
        raise(sig);
        kill(ppid, TEST_PASSED);

    }
    else if (strcmp(argv[1], "mask") == 0){
        raise(sig);
        kill(ppid, TEST_PASSED);
    }
    else if (strcmp(argv[1], "pending") == 0){
        sigset_t mask;
        sigpending(&mask);
        if (sigismember(&mask, sig)) kill(ppid, TEST_PASSED);

    }

    return 0;
}