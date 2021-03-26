#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include "libmerge.h"
#include <sys/types.h>

struct operation{
    double real_time;
    double user_time;
    double sys_time;

};

struct test{
    char* files_length;
    int number_of_cases;
    int* numbers_of_blocks;
    struct sequence* seq;
};

struct files_pair* decode_files_pair(char* input){
    char* file1;
    char* file2;
    int i = 0;
    while (i < strlen(input) && input[i] != 58){
        i ++;
    }
    if (i > 0 && i < strlen(input)-2){
        file1 = (char*) calloc(i, sizeof(char));
        strncpy(file1, input, i);
    } 
    else {
        return NULL;
    }
    file2 = (char*) calloc(strlen(input)-i-1, sizeof(char));
    strncpy(file2, input+i+1, strlen(input)-i-1);
    if (strstr(file1, ".txt") == NULL || strstr(file2, ".txt") == NULL){
        return NULL;
    }
    struct files_pair* pair = (struct files_pair*) calloc(1, sizeof(struct files_pair));
    pair -> first_file = file1;
    pair -> second_file = file2;
    return pair;
}

void write_stats(struct operation* operations){
    printf("MERGE\n");
    printf("REAL         USER        SYSTEM\n");
    printf("%lf     %lf    %lf\n\n",operations[0].real_time, operations[0].user_time, operations[0].sys_time);
}

void pass_test(struct test* test){
    struct main_table* table;
    printf("----------TEST with fork-------------\n");
    printf("-------files length: %s--------\n", test->files_length);

    for(int i = 0; i < test->number_of_cases; i+=1){
        struct tms usr_sys_times[2]; //to store user and system times
        clock_t real_times[2]; //to store real times
        struct operation operations[1];
        printf("-------number of blocks: %d-------\n", test->numbers_of_blocks[i]);
        free_main_table(table); //for security
        table = create_table(test->numbers_of_blocks[i]);
        struct file_info** merged_files = (struct file_info**) calloc(test->numbers_of_blocks[i], sizeof(struct file_info*)); 
        
        //MERGE
        times(&usr_sys_times[0]);
        real_times[0] = clock();
        for (int j = 0; j<test->numbers_of_blocks[i]; j+=1){
            if(fork() == 0){
                merged_files[j] = merge_pair(test->seq->pairs[j%test->seq->size]);
                _exit(0);
            }
        }
        times(&usr_sys_times[1]);
        real_times[1] = clock();

        for(int j = 0; j < 1; j+=1){
            operations[j].real_time = (double)(real_times[2*j+1]-real_times[2*j])/CLOCKS_PER_SEC;
            operations[j].user_time = (double)(usr_sys_times[2*j+1].tms_utime-usr_sys_times[2*j].tms_utime)/sysconf(_SC_CLK_TCK);
            operations[j].sys_time = (double)(usr_sys_times[2*j+1].tms_stime-usr_sys_times[2*j].tms_stime)/sysconf(_SC_CLK_TCK);
        }
        write_stats(operations);
        for(int j = 0; j<test->numbers_of_blocks[i]; j+=1){
            //fclose(merged_files[j]->file);
            free(merged_files[j]);
        }
        free(merged_files);
        free_main_table(table);
        table = NULL;

    }
}

struct files_pair* create_pair(char* file1, char* file2){
    struct files_pair* pair = (struct files_pair*) calloc(1, sizeof(struct files_pair));
    pair->first_file = file1;
    pair->second_file = file2;
    return pair;
}

struct sequence* create_seq(int length, struct files_pair** pairs){
    struct sequence* seq = (struct sequence*) calloc(1, sizeof(struct sequence));
    seq->pairs = pairs;
    seq->size = length;
    return seq;
}

int is_fulfilled(struct test* test){
    if(test->files_length != NULL && test->number_of_cases > 0 && test->numbers_of_blocks != NULL && test->seq != NULL){
        return 1;
    }
    else{
        return 0;
    }
}

int main(int argc, char** argv){
    struct test* merge_test = (struct test*) calloc(1, sizeof(struct test));
    if (strcmp(argv[1], "test") == 0){
        //user must provide correct input
        for(int i = 2; i < argc; i+=1){

            if (strcmp(argv[i], "merge_files") == 0){
                i += 1;
                int no_pairs = atoi(argv[i]);
                i += 1;
                struct sequence* seq = create_sequence(no_pairs);
                int not_null_id = 0;
                for (int j = 0; j < seq->size; j+=1, i+=1){
                    //save pairs of files in sequence
                    struct files_pair* pair = decode_files_pair(argv[i]);
                    if (pair) {
                        seq->pairs[not_null_id] = pair;
                        not_null_id += 1;
                    }
                }
                merge_test->seq = seq;
                i-=1;
            }
            else if(strcmp(argv[i], "numbers_of_blocks") == 0){
                i += 1;
                int noc = atoi(argv[i]);
                merge_test->number_of_cases = noc;
                i += 1;
                int* no_blocks = (int*) calloc(noc, sizeof(int));
                for (int j = 0; j < noc; j+=1, i+=1){
                    no_blocks[j] = atoi(argv[i]);
                }
                merge_test->numbers_of_blocks = no_blocks;
                i -= 1;
            }
            else if(strcmp(argv[i], "files_length") == 0){
                i += 1;
                merge_test->files_length = argv[i];
            }
        }

        if (is_fulfilled(merge_test) == 1){
            pass_test(merge_test);
        }
        
    }
    free(merge_test);
    return 0;
}