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

void fulfill_file(int input, int output){
    char* line = NULL;
    size_t length = 0;

    while (getline_sys(&line, &length, input) != 0){
        while (strlen(line) > 50){
            write(output, line, 50);
            write(output, "\n", 1);
            line += 50;
        }
        write(output, line, strlen(line));
    }
    free(line);
    
}

int main(int argc, char** argv){

    if (argc != 3){
        perror("Wrong number of arguments, must be 4!");
        return -1;
    }

    int input = open(argv[1], O_RDONLY);
    int output = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
    

    fulfill_file(input, output);

    close(input);
    close(output);

    return 0;
}