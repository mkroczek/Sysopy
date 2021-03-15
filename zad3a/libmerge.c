#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libmerge.h"

struct main_table* create_table(int size){
    struct main_table* table = calloc(1, sizeof(table));
    table->blocks = (struct block**) calloc(size, sizeof(struct block*));
    table->size = size;
    return table;
}

struct file_info* merge_pair(struct files_pair* files){

    int no_lines = 0;

    FILE* first_file_ptr = fopen(files->first_file, "r");
    if(first_file_ptr == NULL) printf("Error while opening file %s", files->first_file);
    FILE* second_file_ptr = fopen(files->second_file, "r");
    if(second_file_ptr == NULL) printf("Error while opening file %s", files->second_file);
    FILE* temp_file_ptr = tmpfile();

    char* line1 = NULL;
    char* line2 = NULL;
    size_t line1_len = 0;
    size_t line2_len = 0;

    int l1 = getline(&line1, &line1_len, first_file_ptr);
    int l2 = getline(&line2, &line2_len, second_file_ptr);

    while (l1 != -1 || l2 != -1){
        if (l1 != -1){
            fputs(line1, temp_file_ptr);
            no_lines += 1;
            if(line1[strlen(line1)-1] != 10) fputc(10, temp_file_ptr);
            l1 = getline(&line1, &line1_len, first_file_ptr);
        }
        if (l2 != -1){
            fputs(line2, temp_file_ptr);
            no_lines += 1;
            if(line2[strlen(line2)-1] != 10) fputc(10, temp_file_ptr);
            l2 = getline(&line2, &line2_len, second_file_ptr);    
        }
    }

    free(line1);
    free(line2);
    fclose(first_file_ptr);
    fclose(second_file_ptr);
    rewind(temp_file_ptr);
    struct file_info* merged = (struct file_info*) calloc(1,sizeof(struct file_info));
    merged->file = temp_file_ptr;
    merged->no_lines = no_lines;

    return merged;

}

struct block* create_block(struct file_info* merged){
    FILE* tmp_file_ptr = merged->file;
    if(tmp_file_ptr == NULL){
        perror("Error while opening merged file");
        return NULL;
    }

    struct block* blk = calloc(1, sizeof(blk));
    blk->total_size = merged->no_lines;
    blk->current_size = merged->no_lines;
    blk->rows = (char**) calloc(merged->no_lines, sizeof(char*));

    char* line = NULL;
    size_t line_len = 0;

    for (int i = 0; i < merged->no_lines; i+=1){
        getline(&line, &line_len, tmp_file_ptr);
        blk->rows[i] = (char*) calloc(strlen(line), sizeof(char));
        strncpy(blk->rows[i], line, sizeof(char)*strlen(line));
    }
    
    free(line);
    tmp_file_ptr = NULL;
    return blk;
}

int add_block(struct main_table* table, struct block* blk){
    int i = 0;
    while (table->blocks[i] && i < table->size){
        i+=1;
    }
    if (i == table->size) return -1;
    table->blocks[i] = blk;

    return i;
}

int merge_and_add(struct main_table* table, struct files_pair* pair){
    struct file_info* merged = merge_pair(pair);
    struct block* blk = create_block(merged);
    return add_block(table, blk);
}

void merge_files(struct main_table* table, struct sequence* seq){
    if (seq == NULL || table == NULL) return;
    for (int i = 0; i < seq->size; i+=1){
        merge_and_add(table, seq->pairs[i]);
    }
}

int number_of_rows(struct block* blk){
    return (blk->current_size);
}

int rows_in_block(struct main_table* table, int index){
    if (index >= table->size){
        perror("Index out of main_table's bounds");
        return -1;
    }
    if (table->blocks[index] == NULL){
        return 0;
    }
    return number_of_rows(table->blocks[index]);
    
}

void remove_block(struct main_table* table, int index){
    if (index >= table->size){
        perror("Index out of main_table's bounds");
        return;
    }
    struct block* blk = table -> blocks[index];
    if(blk == NULL) return;
    for(int i = 0; i < blk->total_size; i+=1){
        free(blk->rows[i]);
    }
    free(blk);
    table -> blocks[index] = NULL;
    blk = NULL;

}

void remove_row_from_block(struct main_table* table, int block_index, int row){
    if (block_index >= table->size){
        perror("Index out of main_table's bounds");
        return;
    }
    struct block* blk = table->blocks[block_index];
    if (row >= blk->current_size){
        perror("Index out of block's bounds");
        return;
    }
    int not_null = -1;
    int i = 0;

    while(not_null != row){
        if(blk->rows[i]){
            not_null += 1;
        }
        i += 1;
    }
    i -= 1;
    free(blk->rows[i]);
    blk->rows[i] = NULL;
    blk->current_size -= 1;

}

void print_block(struct main_table* table, int index){
    struct block* blk = table->blocks[index];
    for(int i = 0; i < blk->total_size; i+=1){
        if (blk->rows[i]){
            printf("%s",blk->rows[i]);
        }
    }
}

void print_all(struct main_table* table){
    for(int i = 0; i < table->size; i+=1){
        if (table->blocks[i] != NULL){
            print_block(table, i);
        }
    }
}

struct sequence* create_sequence(int size){
    struct sequence* seq = calloc(1, sizeof(seq));
    seq->pairs = (struct files_pair**) calloc(size, sizeof(struct files_pair*));
    seq->size = size;
    return seq;
}

void free_main_table(struct main_table* table){
    if (table == NULL) return;
    for (int i = 0; i < table->size; i+=1){
        remove_block(table, i);
    }
    free(table->blocks);
    free(table);
}