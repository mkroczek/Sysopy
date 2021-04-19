#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <sys/file.h>

int main(int argc, char** argv){
    char* pipe_path;
    int row;
    char* resources_path;
    int N;
    if (argc == 5){
        pipe_path = argv[1];
        row = atoi(argv[2]);
        resources_path = argv[3];
        N = atoi(argv[4]);
    }
    else{
        exit(1);
    }

    // FILE* pipe_file = fopen(pipe_path, "w");
    int pipe_file = open(pipe_path, O_WRONLY);
    FILE* resources_file = fopen(resources_path, "r");

    flock(pipe_file, LOCK_EX);
    char buf[N];
    srand(time(NULL));

    while (fgets(buf, N, resources_file)){
        char input[N+3+strlen(argv[2])];
        sprintf(input, "#%d#%s\n", row, buf);
        // fputs(input, pipe_file);
        write(pipe_file, input, strlen(input));
        sleep((rand()%2)+1);
    }

    flock(pipe_file, LOCK_UN);

    // fclose(pipe_file);
    close(pipe_file);
    fclose(resources_file);

    return 0;
}