#include <stdlib.h>
#include <stdio.h>
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

void merge_and_print_lib(char* filename1, char* filename2){

    FILE* file1 = fopen(filename1, "r");
    if(file1 == NULL){
        perror("Couldn't open first file");
    }
    FILE* file2 = fopen(filename2, "r");
    if(file2 == NULL){
        perror("Couldn't open second file");
    }

    char* line1 = NULL;
    char* line2 = NULL;
    size_t line1_len = 0;
    size_t line2_len = 0;

    int l1 = getline_lib(&line1, &line1_len, file1);
    int l2 = getline_lib(&line2, &line2_len, file2);

    while (l1 != 0 || l2 != 0){
        if (l1 != 0){
            printf(line1);
            if(line1[strlen(line1)-1] != 10) printf("%c",10); //last line from file needs \n to add
            l1 = getline_lib(&line1, &line1_len, file1);
        }
        if (l2 != 0){
            printf(line2);
            if(line2[strlen(line2)-1] != 10) printf("%c",10);
            l2 = getline_lib(&line2, &line2_len, file2);
        }
    }

    free(line1);
    free(line2);
    fclose(file1);
    fclose(file2);

}

int main(int argc, char** argv){
    char* filename1;
    char* filename2;

    filename1= argv[1];
    filename2= argv[2];

    merge_and_print_lib(filename1, filename2);

    return 0;

}