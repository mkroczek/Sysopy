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

int is_quarter(int number){
    if (number == 0) return 1;
    for (int i = 1; i*i <= number; i += 1){
        if (i*i == number) return 1;
    }
    return 0;
}

int get_tens(int number){
    return (number/10)%10;
}

void fulfill_files(int data, int a, int b, int c){
    int even = 0;
    char* line = NULL;
    size_t length = 0;
    int number;
    char* number_str = (char*) calloc(12, sizeof(char));
    while (getline_sys(&line, &length, data) != 0){
        number = atoi(line);
        if (number%2 == 0) even += 1;
        if (get_tens(number) == 0 || get_tens(number) == 7) write(b, line, strlen(line));
        if (is_quarter(number) == 1) write(c, line, strlen(line));
    }
    sprintf(number_str, "%d", even);
    write(a, number_str, strlen(number_str));
    free(number_str);
}

int main(int argc, char** argv){
    int data = open("dane.txt", O_RDONLY);
    int a = open("a.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
    int b = open("b.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
    int c = open("c.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);

    fulfill_files(data, a, b, c);

    close(data);
    close(a);
    close(b);
    close(c);

    return 0;
}