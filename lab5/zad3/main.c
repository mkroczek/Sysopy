#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>
#include <string.h>

int getline_lib(char** line, size_t* length, FILE* file){
    //returns number of read characters
    char* buf = (char*) calloc(256, sizeof(char));
    char* ptr;
    char c;
    size_t current_buffer_size = 256;
    *length = 0;
    size_t i = 0;

    while(i < current_buffer_size){
        while(i < current_buffer_size && fread(&c, sizeof(char), 1, file) == 1){
            buf[i] = c;
            i += 1;
            if (c == '\n') break;
        }
        if (i == current_buffer_size){
            current_buffer_size *= 2;
            ptr = (char*) realloc(buf, current_buffer_size);
            buf = ptr;
        }
        else{
            break;
        }
    }

    *length = i;
    *line = buf;
    return i;
    
}

void validate_output(char* output, char** resources, int n_resources){
    FILE* output_file = fopen(output, "r");
    if (output_file == NULL){
        printf("Couldn't open output file\n");
    }
    char* line = NULL;
    size_t line_len = 0;
    char* line_resources = NULL;
    size_t line_resources_len = 0;

    // int l = getline_lib(&line, &line_len, output_file);
    for (int i = 0; i < n_resources; i++){
        FILE* resources_file = fopen(resources[i], "r");
        getline_lib(&line_resources, &line_resources_len, resources_file);
        int row = 0;
        getline_lib(&line, &line_len, output_file);
        int correct = 0;
        while (row <= i)
        {
            if (row != i){
                free(line);
                getline_lib(&line, &line_len, output_file);
            }
            else if (strncmp(line, line_resources, strlen(line)-1) == 0)
                correct = 1;
            row ++;
        }
        if (correct)
            printf("Output file contains contents of %s in line %d\n", resources[i], i);
        else
            printf("Output file doesn't contain contents of %s in line %d\n", resources[i], i);
        rewind(output_file);
        free(line_resources);
        fclose(resources_file);
        printf("Everything good at the end of validate_output\n");
    }
    if (output_file == NULL){
        printf("Couldn't open output file\n");
    }
    fclose(output_file);
}

char**create_customer(char* prog, char* pipe, char* output_path, char* N){
    char** customer = (char**) calloc(5, sizeof(char*));
    customer[0] = prog;
    customer[1] = pipe;
    customer[2] = output_path;
    customer[3] = N;
    customer[4] = NULL;
    return customer;
}

char** create_producer(char* prog, char* pipe, int i, char* resources_path, char* N){
    char** producer = (char**) calloc(6, sizeof(char*));
    char* index = (char*)calloc(6, sizeof(char));
    sprintf(index, "%d", i);
    producer[0] = prog;
    producer[1] = pipe;
    producer[2] = index;
    producer[3] = resources_path;
    producer[4] = N;
    producer[5] = NULL;
    return producer;
}

void test(int n_consumers, int n_producers, char* N, char** resources_directions, char* output_path){
    char*** programs = (char***) calloc(n_consumers+n_producers, sizeof(char**));
    for (int i = 0; i < n_consumers; i++){
        programs[i] = create_customer("./consumer", "./test_fifo", output_path, N);
    }
    for (int i = 0; i < n_producers; i++){
        programs[i+n_consumers] = create_producer("./producer", "./test_fifo", i, resources_directions[i], N);
    }
    for (int i = 0; i < n_consumers+n_producers; i++){
        if (fork() == 0){
            for(int j = 0; j < 5; j++){
            }
            if(execvp(programs[i][0], programs[i]) == -1){
                exit(1);
            }
        }
    }
    for(int i = 0; i < n_producers+n_consumers; i++){
        wait(NULL);
    }
}

int main(int argc, char** argv){

    char* output_save = "./customer_output.txt";
    char N[] = "15";
    char* resources_directions[] = {"./resources/1.txt", "./resources/a.txt", "./resources/2.txt", "./resources/3.txt", "./resources/b.txt"};

    // int consumers[] = {1, 5};
    // int producers[] = {5, 1};
    int consumers[] = {5};
    int producers[] = {5};
    char buf_size[5];
    sprintf(buf_size, "%d", PIPE_BUF + 100);
    char* N_s[] = {"5", "100", buf_size};

    for (int c = 0; c < 2; c++){
        for (int p = 0; p < 2; p++){
            for (int n = 0; n < 3; n++){
                mkfifo("test_fifo", S_IRUSR | S_IWUSR | S_IFIFO);
                remove(output_save);
                test(consumers[c], producers[p], N_s[n], resources_directions, output_save);
                printf("\nResults for %d customers, %d producers and N = %s\n", consumers[c], producers[p], N_s[n]);
                validate_output(output_save, resources_directions, producers[p]);
                remove("test_fifo");

            }
        }
    }

    return 0;
}