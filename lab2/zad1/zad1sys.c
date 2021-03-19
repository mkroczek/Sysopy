#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>

int getline_sys(char** line, size_t* length, int descriptor){
    //returns number of read characters
    char* buf = (char*) calloc(256, sizeof(char));
    char* ptr;
    char c;
    size_t current_buffer_size = 256;
    *length = 0;
    size_t i = 0;

    while(i < current_buffer_size){
        while(i < current_buffer_size && read(descriptor, &c, 1) == 1){
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

void merge_and_print_sys(char* filename1, char* filename2){

    int file1 = open(filename1, O_RDONLY);
    int file2 = open(filename2, O_RDONLY);

    char* line1 = NULL;
    char* line2 = NULL;
    size_t line1_len = 0;
    size_t line2_len = 0;

    int l1 = getline_sys(&line1, &line1_len, file1);
    int l2 = getline_sys(&line2, &line2_len, file2);

    while (l1 != 0 || l2 != 0){
        if (l1 != 0){
            printf(line1);
            if(line1[strlen(line1)-1] != 10) printf("%c",10); //last line from file needs \n to add
            l1 = getline_sys(&line1, &line1_len, file1);
        }
        if (l2 != 0){
            printf(line2);
            if(line2[strlen(line2)-1] != 10) printf("%c",10);
            l2 = getline_sys(&line2, &line2_len, file2);
        }
    }

    free(line1);
    free(line2);
    close(file1);
    close(file2);

}

int main(int argc, char** argv){
    char* filename1;
    char* filename2;

    filename1= argv[1];
    filename2= argv[2];

    merge_and_print_sys(filename1, filename2);

    return 0;

}