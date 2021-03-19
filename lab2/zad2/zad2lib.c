#include <stdio.h>
#include <stdlib.h>

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

int contains_chr(char character, char* buffer, int length){
    for (int i = 0; i < length && buffer[i] != 10; i+=1){
        if(buffer[i] == character) return 1;
    }
    return 0;
}

void print_containing(char character, char* filename){
    FILE* file = fopen(filename, "r");
    char* line = NULL;
    size_t line_len = 0;
    while(getline_lib(&line, &line_len, file) != 0){
        if (contains_chr(character, line, line_len) == 1) printf(line);
    }

    free(file);
}

int main(int argc, char** argv){
    char character;
    char* filename;

    character = argv[1][0];
    filename = argv[2];

    print_containing(character, filename);

    return 0;
}