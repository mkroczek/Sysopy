#include "zad1.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

int** input_image;
int** output_image;
int W,H,M;
pthread_t* threads; 

void* negative_numbers(void* arg){
    range* scale_range = (range*) arg;
    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            if (input_image[i][j] >= scale_range->from && input_image[i][j] <= scale_range->to){
                output_image[i][j] = 255-input_image[i][j];
            }
        }
    }
    return NULL;

}

void* negative_block(void* arg){
    range* scale_range = (range*) arg;
    for (int i = 0; i < H; i++){
        for (int j = scale_range->from; j <= scale_range->to; j++){
            output_image[i][j] = 255-input_image[i][j];
        }
    }
    return NULL;

}

int main(int argc, char** argv){
    int nThreads;
    int type;
    char* input_filename;
    char* output_filename;
    char* id = (char*) calloc(50, sizeof(char));
    char trash[100];
    
    if (argc != 5){
        perror("Invalid number of arguments!");
        exit(0);
    }

    //get number of threads
    nThreads = atoi(argv[1]);
    threads = (pthread_t*) calloc(nThreads, sizeof(pthread_t));
    
    //get execute type
    if(strcmp(argv[2], "numbers") == 0){
        type = NUMBERS;
    }
    else if (strcmp(argv[2], "block") == 0){
        type = BLOCK;
    }
    else{
        perror("Type must be numbers or block!");
        exit(0);
    }

    //get input and output file
    input_filename = argv[3];
    output_filename = argv[4];

    //get information about image
    FILE* input_file = fopen(input_filename, "r");
    fscanf(input_file, "%s", id);
    while (fscanf(input_file, "%d %d", &W, &H) < 2){
        fscanf(input_file, "%s", trash);
    }
    fscanf(input_file, "%d", &M);

    //initialize input and output image arrays
    input_image = (int**) calloc(H, sizeof(int*)); 
    output_image = (int**) calloc(H, sizeof(int*));
    for (int i = 0; i < H; i++){
        input_image[i] = (int*) calloc(W, sizeof(int));
    }
    for (int i = 0; i < H; i++){
        output_image[i] = (int*) calloc(W, sizeof(int));
    }

    //read image
    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            fscanf(input_file,"%d", &input_image[i][j]);
        }
    }

    fclose(input_file);

    if (type == NUMBERS){
        //split interval into threads
        int range_size = (M+1)/nThreads;
        //take ceil
        if (range_size*nThreads < M+1) ++range_size;
        int last_size = (M+1)%range_size;
        int n_full = nThreads;
        if (last_size != 0){
            n_full--;
            range* rng = (range*) calloc(1, sizeof(range));
            rng->from = range_size*n_full;
            rng->to = rng->from+last_size-1;
            // printf("Tutaj wywolam watek z przedzialem %d -> %d\n", rng->from, rng->to);
            pthread_create(&threads[nThreads-1], NULL, negative_numbers, rng);
        }
        for (int i = 0; i < n_full; i++){
            range* rng = (range*) calloc(1, sizeof(range));
            rng->from = i*range_size;
            rng->to = (i+1)*range_size-1;
            // printf("Tutaj wywolam watek z przedzialem %d -> %d\n", rng->from, rng->to);
            pthread_create(&threads[i], NULL, negative_numbers, rng);
        }
    }
    else if (type == BLOCK){
        for (int k = 1; k < nThreads+1; k++){
            range* rng = (range*) calloc(1, sizeof(range));
            //take ceil
            int res = W/nThreads;
            if (res*nThreads < W) ++res;
            rng->from = (k-1)*res;
            rng->to = k*res-1;
            // printf("Tutaj wywolam watek z przedzialem %d -> %d\n", rng->from, rng->to);
            pthread_create(&threads[k-1], NULL, negative_block, rng);
        }
    }

    //wait for threads to finish
    for (int i = 0; i < nThreads; i++){
        pthread_join(threads[i], NULL);
    }

    //save result in output file
    FILE* output_file = fopen(output_filename, "w");
    fprintf(output_file, "%s\n%d %d\n%d\n", id, W, H, 255);
    for (int i = 0; i < H; i++){
        for (int j = 0; j < W; j++){
            fprintf(output_file,"%d ", output_image[i][j]);
        }
        if (i < H-1)
            fprintf(output_file,"\n");
    }

    //free allocated memory
    for (int i = 0; i < H; i++){
        free(input_image[i]);
        free(output_image[i]);
    }
    free(input_image);
    free(output_image);
    free(threads);

    return 0;
}