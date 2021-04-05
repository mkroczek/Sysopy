#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TEST_PASSED SIGUSR2

void test_handler(int signum){
    write(STDOUT_FILENO, "Now I am handling signal\n", 26);
}

void handlerUSR2(int signum){
    write(STDOUT_FILENO, "Test succeded\n", 15);
}

void test_ignore(int sig){
    signal(sig, SIG_IGN);
    raise(sig);
    if (fork() == 0){
        raise(sig);
        kill(getppid(), TEST_PASSED);
        exit(1);
    }
    wait(NULL);
    signal(sig, SIG_DFL); 
    raise(TEST_PASSED);
}

void test_handle(int sig){
    signal(sig, test_handler);
    raise(sig);
    if (fork() == 0){
        raise(sig);
        kill(getppid(), TEST_PASSED);
        exit(1);
    }
    wait(NULL);
    signal(sig, SIG_DFL);
    raise(TEST_PASSED);
}

void test_mask(int sig){
    sigset_t oldmask, blockmask;
    sigemptyset(&blockmask);
    sigaddset(&blockmask, sig);
    if (sigprocmask(SIG_SETMASK, &blockmask, &oldmask) == -1){
        perror("Nie udalo sie zablokowac sygnalu\n");
        exit(1);
    }
    raise(sig); //only parent process
    if (fork() == 0){
        raise(sig);
        kill(getppid(), TEST_PASSED);
        exit(1);
    }
    wait(NULL);
    raise(TEST_PASSED);

}

void test_pending(int sig){
    sigset_t oldmask, blockmask, mask;
    sigemptyset(&blockmask);
    sigaddset(&blockmask, sig);
    if (sigprocmask(SIG_SETMASK, &blockmask, &oldmask) == -1){
        perror("Nie udalo sie zablokowac sygnalu\n");
        exit(1);
    }
    raise(sig);
    sigpending(&mask);
    if (sigismember(&mask, sig)) raise(TEST_PASSED);
    if (fork() == 0){
        sigset_t mask_fork;
        sigemptyset(&mask_fork);
        sigpending(&mask_fork);
        if (sigismember(&mask_fork, sig)) kill(getppid(), TEST_PASSED);
        exit(1);
    }
    wait(NULL);

}

int main(int argc, char** argv){

    signal(SIGUSR2, handlerUSR2);

    for(int i = 1; i< argc; i++){
        if (strcmp(argv[i], "ignore") == 0){
            i ++;
            int sig = atoi(argv[i]);
            test_ignore(sig);

        }
        else if (strcmp(argv[i], "handle") == 0){
            i ++;
            int sig = atoi(argv[i]);
            test_handle(sig);

        }
        else if (strcmp(argv[i], "mask") == 0){
            i ++;
            int sig = atoi(argv[i]);
            test_mask(sig);

        }
        else if (strcmp(argv[i], "pending") == 0){
            i ++;
            int sig = atoi(argv[i]);
            test_pending(sig);   

        }
    }

    return 0;
}