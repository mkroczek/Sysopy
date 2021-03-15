#include <stdio.h>

#ifndef __MERGELIB_H__
#define __MERGELIB_H__

struct files_pair{
    char* first_file;
    char* second_file;
};

struct sequence{
    struct files_pair** pairs;
    int size;
};

struct file_info{
    FILE* file;
    int no_lines;
};

struct block{
    char** rows;
    int total_size;
    int current_size;
};

struct main_table{
    struct block** blocks;
    int size;
};

struct main_table* create_table(int size);
struct file_info* merge_pair(struct files_pair* files);
struct block* create_block(struct file_info* temp_file);
int add_block(struct main_table* table, struct block* blk);
int merge_and_add(struct main_table* table, struct files_pair* pair);
void merge_files(struct main_table* table, struct sequence* seq);
int number_of_rows(struct block* blk);
int rows_in_block(struct main_table* table, int index);
void remove_block(struct main_table* table, int index);
void remove_row_from_block(struct main_table* table, int block_index, int row);
void print_block(struct main_table* table, int index);
void print_all(struct main_table* table);
struct sequence* create_sequence(int size);
void free_main_table(struct main_table* table);

#endif