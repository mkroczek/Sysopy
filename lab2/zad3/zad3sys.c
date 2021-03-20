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
    char* result[3];
    for (int i = 0; i < 3; i += 1){
        result[i] = (char*) calloc(20, sizeof(char));
    }
    int report = open("pomiar_zad_3_sys.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
    struct tms usr_sys_times[2];
    clock_t time[2];

    int data = open("dane.txt", O_RDONLY);
    int a = open("a.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
    int b = open("b.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
    int c = open("c.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);

    times(&usr_sys_times[0]);
    time[0] = clock();
    fulfill_files(data, a, b, c);
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

    close(data);
    close(a);
    close(b);
    close(c);

    return 0;
}