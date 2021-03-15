#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include "libmerge.h"

struct operation{
    double real_time;
    double user_time;
    double sys_time;

};

struct test{
    int id;
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
    printf("ADD\n");
    printf("REAL         USER        SYSTEM\n");
    printf("%lf     %lf    %lf\n\n",operations[1].real_time, operations[1].user_time, operations[1].sys_time);
    printf("DELETE\n");
    printf("REAL         USER        SYSTEM\n");
    printf("%lf     %lf    %lf\n\n",operations[2].real_time, operations[2].user_time, operations[2].sys_time);
    printf("DELETE AND ADD\n");
    printf("REAL         USER        SYSTEM\n");
    printf("%lf     %lf    %lf\n\n",operations[3].real_time, operations[3].user_time, operations[3].sys_time);
}

void pass_test(struct test* test){
    struct main_table* table;
    printf("--------------TEST %d--------------\n", test->id);
    printf("-------files length: %s--------\n", test->files_length);

    for(int i = 0; i < test->number_of_cases; i+=1){
        struct tms usr_sys_times[8]; //to store user and system times
        clock_t real_times[8]; //to store real times
        struct operation operations[4];
        printf("-------number of blocks: %d-------\n", test->numbers_of_blocks[i]);
        free_main_table(table); //for security
        table = create_table(test->numbers_of_blocks[i]);
        struct file_info** merged_files = (struct file_info**) calloc(test->numbers_of_blocks[i], sizeof(struct file_info*)); 
        
        //MERGE
        times(&usr_sys_times[0]);
        real_times[0] = clock();
        for (int j = 0; j<test->numbers_of_blocks[i]; j+=1){
            merged_files[j] = merge_pair(test->seq->pairs[j%test->seq->size]);
        }
        times(&usr_sys_times[1]);
        real_times[1] = clock();

        //ADD
        times(&usr_sys_times[2]);
        real_times[2] = clock();
        for (int j = 0; j<test->numbers_of_blocks[i]; j+=1){
            struct block* blk = create_block(merged_files[j]);
            add_block(table, blk);
        }
        times(&usr_sys_times[3]);
        real_times[3] = clock();

        //DELETE
        times(&usr_sys_times[4]);
        real_times[4] = clock();
        for (int j = 0; j<test->numbers_of_blocks[i]; j+=1){
            remove_block(table, j);
        }
        times(&usr_sys_times[5]);
        real_times[5] = clock();
        
        //ADD AND DELETE
        times(&usr_sys_times[6]);
        real_times[6] = clock();
        for (int j = 0; j<100; j+=1){
            struct block* blk = create_block(merged_files[j%test->numbers_of_blocks[i]]);
            add_block(table, blk);
            remove_block(table, 0);
        }
        times(&usr_sys_times[7]);
        real_times[7] = clock();

        for(int j = 0; j < 4; j+=1){
            operations[j].real_time = (double)(real_times[2*j+1]-real_times[2*j])/CLOCKS_PER_SEC;
            operations[j].user_time = (double)(usr_sys_times[2*j+1].tms_utime-usr_sys_times[2*j].tms_utime)/sysconf(_SC_CLK_TCK);
            operations[j].sys_time = (double)(usr_sys_times[2*j+1].tms_stime-usr_sys_times[2*j].tms_stime)/sysconf(_SC_CLK_TCK);
        }
        write_stats(operations);

        for(int j = 0; j<test->numbers_of_blocks[i]; j+=1){
            fclose(merged_files[j]->file);
            free(merged_files[j]);
        }
        free(merged_files);
        free_main_table(table);
        table = NULL;

    }
}

struct test* create_test(int id, char* length, int number_of_cases, int* numbers_of_blocks, struct sequence* seq){
    struct test* result = (struct test*) calloc(1, sizeof(struct test));
    result->id = id;
    result->files_length = length;
    result->number_of_cases = number_of_cases;
    result->numbers_of_blocks = numbers_of_blocks;
    result->seq = seq;
    return result;
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

int main(int argc, char** argv){
    struct main_table* table;
    struct sequence* seq = NULL;

    //testing from terminal
    for(int i = 1; i < argc; i+=1){

        if (strcmp(argv[i], "create_table") == 0){
            i += 1;
            int size = atoi(argv[i]);
            if (size <= 0) return -1;
            table = create_table(size);
        }
        else if (strcmp(argv[i], "merge_files") == 0){
            i += 1;
            int not_null_id = 0;
            seq = create_sequence(table->size);
            for (int j = 0; j < seq->size; j+=1, i+=1){
                struct files_pair* pair = decode_files_pair(argv[i]);
                if (pair) {
                    seq->pairs[not_null_id] = pair;
                    not_null_id += 1;
                }
            }
            merge_files(table, seq);
            i-=1;
        }
        else if(strcmp(argv[i], "remove_block") == 0){
            i += 1;
            int id = atoi(argv[i]);
            remove_block(table, id);
        }
        else if(strcmp(argv[i], "remove_row") == 0){
            i += 1;
            int block = atoi(argv[i]);
            i += 1;
            int row = atoi(argv[i]);
            remove_row_from_block(table, block, row);
        }
        else if(strcmp(argv[i], "print_block") == 0){
            i += 1;
            int id = atoi(argv[i]);
            print_block(table, id);
        }
        
    }

    struct files_pair** short_pairs = (struct files_pair**) calloc(3, sizeof(struct files_pair*));
    struct files_pair** medium_pairs = (struct files_pair**) calloc(3, sizeof(struct files_pair*));
    struct files_pair** long_pairs = (struct files_pair**) calloc(3, sizeof(struct files_pair*));

    short_pairs[0] = create_pair("AS.txt", "BS.txt");
    short_pairs[1] = create_pair("AS.txt", "CS.txt");
    short_pairs[2] = create_pair("CS.txt", "BS.txt");

    medium_pairs[0] = create_pair("AM.txt", "BM.txt");
    medium_pairs[1] = create_pair("AM.txt", "CM.txt");
    medium_pairs[2] = create_pair("CM.txt", "BM.txt");

    long_pairs[0] = create_pair("AL.txt", "BL.txt");
    long_pairs[1] = create_pair("AL.txt", "CL.txt");
    long_pairs[2] = create_pair("CL.txt", "BL.txt");

    struct sequence* short_seq = create_seq(3,short_pairs);
    struct sequence* medium_seq = create_seq(3,medium_pairs);
    struct sequence* long_seq = create_seq(3,long_pairs);

    int cases[] = {10, 500, 1000};

    struct test* test_short = create_test(1,"short", 3, cases, short_seq);
    struct test* test_medium = create_test(2,"medium", 3, cases, medium_seq);
    struct test* test_long = create_test(3,"long", 3, cases, long_seq);

    pass_test(test_short);
    pass_test(test_medium);
    pass_test(test_long);

    return 0;
}