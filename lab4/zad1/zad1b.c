#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define TEST_PASSED SIGUSR2

void test_handler(int signum){
    // write(STDOUT_FILENO, "Now I am handling signal\n", 26);
    printf("Now I am handling signal\n");
}

void handlerUSR2(int signum){
    // write(STDOUT_FILENO, "Test succeded\n", 15);
    printf("Test succeded\n");
}

void test_ignore(int sig){
    signal(sig, SIG_IGN);
    raise(sig);
    char* sig_str = (char*) calloc(2, sizeof(char));
    char* ppid = (char*) calloc(10, sizeof(char));
    if (fork() == 0){
        sprintf(sig_str, "%d", sig);
        sprintf(ppid, "%d", getppid());
        execlp("./exec", "./exec", "ignore", sig_str, ppid, NULL);
    }
    wait(NULL);
    signal(sig, SIG_DFL); 
    raise(TEST_PASSED);
    free(sig_str);
    free(ppid);
}

void test_mask(int sig){
    char* sig_str = (char*) calloc(2, sizeof(char));
    char* ppid = (char*) calloc(10, sizeof(char));
    sigset_t oldmask, blockmask;

    sigemptyset(&blockmask);
    sigaddset(&blockmask, sig);
    if (sigprocmask(SIG_SETMASK, &blockmask, &oldmask) == -1){
        perror("Nie udalo sie zablokowac sygnalu\n");
        exit(1);
    }
    raise(sig); //only parent process
    if (fork() == 0){
        sprintf(sig_str, "%d", sig);
        sprintf(ppid, "%d", getppid());
        execlp("./exec", "./exec", "mask", sig_str, ppid, NULL);
    }
    wait(NULL);
    raise(TEST_PASSED);
    free(sig_str);
    free(ppid);

}

void test_pending(int sig){
    char* sig_str = (char*) calloc(2, sizeof(char));
    char* ppid = (char*) calloc(10, sizeof(char));
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
        sprintf(sig_str, "%d", sig);
        sprintf(ppid, "%d", getppid());
        execlp("./exec", "./exec", "pending", sig_str, ppid, NULL);
    }
    wait(NULL);
    free(sig_str);
    free(ppid);

}

int main(int argc, char** argv){

    signal(SIGUSR2, handlerUSR2);

    for(int i = 1; i< argc; i++){
        if (strcmp(argv[i], "ignore") == 0){
            i ++;
            int sig = atoi(argv[i]);
            test_ignore(sig);

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