#include <stdio.h>
#include <stdlib.h>
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

void fulfill_file(FILE* input, FILE* output, char* string1, char* string2){
    char* line = NULL;
    size_t length = 0;
    char* ptr = NULL;

    while (getline_lib(&line, &length, input) != 0){
        ptr = strstr(line, string1);
        while (ptr){
            fwrite(line, sizeof(char), (ptr-line)/sizeof(char), output);
            fwrite(string2, sizeof(char), strlen(string2), output);
            line = ptr+strlen(string1);
            ptr = strstr(line, string1);
        }
        fwrite(line, sizeof(char), strlen(line), output);
    }
    free(line);
    
}

int main(int argc, char** argv){

    if (argc != 5){
        perror("Wrong number of arguments, must be 4!");
        return -1;
    }

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");
    
    char* string1 = argv[3];
    char* string2 = argv[4];

    fulfill_file(input, output, string1, string2);

    fclose(input);
    fclose(output);

    return 0;
}