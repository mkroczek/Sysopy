#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char** argv){
    int n;
    if (argc == 2){
        n = atoi(argv[1]);
    }

    for (int i = 0; i < n; i+=1){
        if (fork() == 0){
            printf("Ten tekst pochodzi z procesu o pid = %d\n", (int)getpid());
            _exit(0);
        }
        
    }

    return 0;

}