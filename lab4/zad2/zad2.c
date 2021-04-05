#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <stdlib.h>
#include <string.h>

void print_info(siginfo_t *info){
    // printf("PID = %d\n", getpid());
    printf("Signal number: %d, has been sent by process with PID: %d\n", info->si_signo, info->si_pid);

}

void sigsegv_handler(int sig, siginfo_t *info, void *context){
    printf("------TEST SIGSEGV------\n");
    print_info(info);
    printf("Segmentation fauld was caused by adress %p\n", info->si_addr);
    raise(SIGKILL);

}

void sigint_handler(int sig, siginfo_t *info, void *context){
    printf("------TEST SIGINT------\n");
    print_info(info);
    printf("Real user ID of sending process = %d\n", info->si_uid);

}

void sigchld_handler(int sig, siginfo_t *info, void *context){
    printf("------TEST SIGCHLD------\n");
    print_info(info);
    printf("Child process exited with value %d\n", info->si_status);

}

void standard_handler(int sig){
    printf("Currently I am handling process %d\n", sig);

}

void test_sigsegv(){
    if (fork() == 0){
        int* ptr = NULL;
        struct sigaction action;
        action.sa_sigaction = sigsegv_handler;
        action.sa_flags = SA_SIGINFO;
        sigemptyset(&action.sa_mask);
        sigaction(SIGSEGV, &action, NULL);
        *ptr = 40;
    }
    wait(NULL);

}

void test_sigint(){
    struct sigaction action;
    action.sa_sigaction = sigint_handler;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    raise(SIGINT);

}

void test_sigchld(){
    struct sigaction action;
    action.sa_sigaction = sigchld_handler;
    action.sa_flags = SA_SIGINFO;
    sigemptyset(&action.sa_mask);
    sigaction(SIGCHLD, &action, NULL);
    if (fork() == 0){
        exit(10);
    }
    wait(NULL);

}

void test_resethand(){
    struct sigaction action;
    action.sa_handler = standard_handler;
    action.sa_flags = SA_RESETHAND;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);
    sigaction(SIGCHLD, &action, NULL);
    if (fork() == 0){
        raise(SIGINT);
        raise(SIGINT);
    }
    wait(NULL);

}

void test_nocldstop(){
    struct sigaction action;
    action.sa_handler = standard_handler;
    action.sa_flags = SA_NOCLDSTOP;
    sigemptyset(&action.sa_mask);
    sigaction(SIGCHLD, &action, NULL);
    if (fork() == 0){
        raise(SIGSTOP);
    }

}

int main(int argc, char** argv){
    
    if (strcmp(argv[1], "SIGINFO") == 0){
        printf("--------------SIGINFO TEST---------------\n");
        //scenario 1
        test_sigsegv();
        //scenario 2
        test_sigint();
        //scenario 3
        test_sigchld();
    }
    else if (strcmp(argv[1], "RESETHAND") == 0){
        printf("--------------RESETHAND TEST---------------\n");
        test_resethand();
    }
    else if (strcmp(argv[1], "NOCLDSTOP") == 0){
        printf("--------------NOCLDSTOP TEST---------------\n");
        test_nocldstop();
    }
    printf("\n");

    return 0;
}