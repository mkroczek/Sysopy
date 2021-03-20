#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <sys/times.h>

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

int contains_chr(char character, char* buffer, int length){
    for (int i = 0; i < length && buffer[i] != 10; i+=1){
        if(buffer[i] == character) return 1;
    }
    return 0;
}

void print_containing(char character, char* filename){
    int file = open(filename, O_RDONLY);
    char* line = NULL;
    size_t line_len = 0;
    while(getline_sys(&line, &line_len, file) != 0){
        if (contains_chr(character, line, line_len) == 1) printf(line);
    }
    close(file);

}

int main(int argc, char** argv){
    char* result[3];
    for (int i = 0; i < 3; i += 1){
        result[i] = (char*) calloc(20, sizeof(char));
    }
    int report = open("pomiar_zad_2_sys.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
    struct tms usr_sys_times[2];
    clock_t time[2];

    char character;
    char* filename;

    character = argv[1][0];
    filename = argv[2];

    times(&usr_sys_times[0]);
    time[0] = clock();
    print_containing(character, filename);
    times(&usr_sys_times[1]);
    time[1] = clock();
    sprintf(result[0],"%lfs",(double)(time[1]-time[0])/CLOCKS_PER_SEC);
    sprintf(result[1],"%lfs",(double)(usr_sys_times[1].tms_utime-usr_sys_times[0].tms_utime)/sysconf(_SC_CLK_TCK));
    sprintf(result[2],"%lfs",(double)(usr_sys_times[1].tms_stime-usr_sys_times[0].tms_stime)/sysconf(_SC_CLK_TCK));
    write(report, "----REAL----\n", 13);
    write(report, result[0], strlen(result[0]));
    write(report, "\n----USER----\n", 14);
    write(report, result[1], strlen(result[1]));
    write(report, "\n----SYS-----\n", 14);
    write(report, result[2], strlen(result[2]));

    close(report);

    for (int i = 0; i < 3; i += 1){
        free(result[i]);
    }

    return 0;
}