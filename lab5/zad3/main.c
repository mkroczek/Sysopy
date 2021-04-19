#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>

void prepare_output(char* path, int n_producers){
    FILE* file = fopen(path, "w");
    for (int i = 0; i < n_producers; i++){
        fputc('\n', file);
    }
    fclose(file);
}

int main(int argc, char** argv){
    mkfifo("test_fifo", S_IRUSR | S_IWUSR | S_IFIFO);

    char* ouput_save = "./customer_output.txt";

    char N[] = "15";
    char* consumer[] = {"./consumer", "./test_fifo", ouput_save, N, NULL};
    char* producer0[] = {"./producer", "./test_fifo", "0", "./resources/1.txt", N, NULL};
    char* producer1[] = {"./producer", "./test_fifo", "1", "./resources/a.txt", N, NULL};
    char* producer2[] = {"./producer", "./test_fifo", "2", "./resources/2.txt", N, NULL};
    char* producer3[] = {"./producer", "./test_fifo", "3", "./resources/3.txt", N, NULL};
    char* producer4[] = {"./producer", "./test_fifo", "4", "./resources/b.txt", N, NULL};
    char** programs[] = {consumer, producer0, producer1, producer2, producer3, producer4};

    prepare_output(ouput_save, 5);

    for (int i = 0; i < 6; i++){
        if (fork() == 0){
            printf("Prof name = %s\n", programs[i][0]);
            for(int j = 0; j < 5; j++){
                printf("Argument = %s\n", programs[i][j]);
            }
            if(execvp(programs[i][0], programs[i]) == -1){
                printf("Nie udalo sie odpalic programu.\n");
                exit(1);
            }
        }
    }
    for(int i = 0; i < 6; i++){
        wait(NULL);
    }

    return 0;
}