#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libmerge.h"

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

int main(int argc, char** argv){
    struct main_table* table;
    struct sequence* seq = NULL;

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
            int id = atoi(argv[i]);
            remove_block(table, id);
        }
        else if(strcmp(argv[i], "print_block") == 0){
            i += 1;
            int id = atoi(argv[i]);
            print_block(table, id);
        }
    }

    return 0;
}