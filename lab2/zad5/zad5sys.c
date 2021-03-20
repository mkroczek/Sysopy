#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
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
    char* result[3];
    for (int i = 0; i < 3; i += 1){
        result[i] = (char*) calloc(20, sizeof(char));
    }
    int report = open("pomiar_zad_5_sys.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
    struct tms usr_sys_times[2];
    clock_t time[2];

    if (argc != 3){
        perror("Wrong number of arguments, must be 2!");
        return -1;
    }

    int input = open(argv[1], O_RDONLY);
    int output = open(argv[2], O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
    
    times(&usr_sys_times[0]);
    time[0] = clock();
    fulfill_file(input, output);
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

    close(input);
    close(output);

    return 0;
}