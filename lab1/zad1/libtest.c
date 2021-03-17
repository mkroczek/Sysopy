#include <stdio.h>
#include <stdlib.h>
#include "libmerge.h"

int main(){
    struct main_table* table = create_table(1);
    struct files_pair* pair = calloc(1,sizeof(pair));
    pair->first_file = "A.txt";
    pair->second_file = "B.txt";
    printf("Index of added block is: %d\n", merge_and_add(table, pair));
    printf("%d\n",rows_in_block(table, 0));
    print_all(table);
    remove_row_from_block(table, 0, 1);
    printf("%d\n",rows_in_block(table, 0));
    // print_block(table, 0);
    // printf("\n\n");
    // remove_block(table, 0);
    // print_all(table);

}