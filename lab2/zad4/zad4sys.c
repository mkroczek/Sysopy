#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

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

void fulfill_file(int input, int output, char* string1, char* string2){
    char* line = NULL;
    size_t length = 0;
    char* ptr = NULL;

    while (getline_sys(&line, &length, input) != 0){
        ptr = strstr(line, string1);
        while (ptr){
            write(output, line, (ptr-line)/sizeof(char));
            write(output, string2, strlen(string2));
            line = ptr+strlen(string1);
            ptr = strstr(line, string1);
        }
        write(output, line, strlen(line));
    }
    free(line);
    
}

int main(int argc, char** argv){

    if (argc != 5){
        perror("Wrong number of arguments, must be 4!");
        return -1;
    }

    int input = open(argv[1], O_RDONLY);
    int output = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
    
    char* string1 = argv[3];
    char* string2 = argv[4];

    fulfill_file(input, output, string1, string2);

    close(input);
    close(output);

    return 0;
}