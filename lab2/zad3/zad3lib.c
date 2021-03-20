#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/times.h>

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

void fulfill_files(FILE* data, FILE* a, FILE* b, FILE* c){
    int even = 0;
    char* line = NULL;
    size_t length = 0;
    int number;
    char* number_str = (char*) calloc(12, sizeof(char));
    while (getline_lib(&line, &length, data) != 0){
        number = atoi(line);
        if (number%2 == 0) even += 1;
        if (get_tens(number) == 0 || get_tens(number) == 7) fwrite(line, sizeof(char), strlen(line), b);
        if (is_quarter(number) == 1) fwrite(line, sizeof(char), strlen(line), c);
    }
    sprintf(number_str, "%d", even);
    fwrite(number_str, sizeof(char), strlen(number_str), a);
    free(number_str);
}

int main(int argc, char** argv){
    char* result[3];
    for (int i = 0; i < 3; i += 1){
        result[i] = (char*) calloc(20, sizeof(char));
    }
    FILE* report = fopen("pomiar_zad_3_lib.txt", "w");
    struct tms usr_sys_times[2];
    clock_t time[2];
    FILE* data = fopen("dane.txt", "r");
    FILE* a = fopen("a.txt", "w");
    FILE* b = fopen("b.txt", "w");
    FILE* c = fopen("c.txt", "w");
    
    times(&usr_sys_times[0]);
    time[0] = clock();
    fulfill_files(data, a, b, c);
    times(&usr_sys_times[1]);
    time[1] = clock();
    sprintf(result[0],"%lfs",(double)(time[1]-time[0])/CLOCKS_PER_SEC);
    sprintf(result[1],"%lfs",(double)(usr_sys_times[1].tms_utime-usr_sys_times[0].tms_utime)/sysconf(_SC_CLK_TCK));
    sprintf(result[2],"%lfs",(double)(usr_sys_times[1].tms_stime-usr_sys_times[0].tms_stime)/sysconf(_SC_CLK_TCK));
    fwrite("----REAL----\n", sizeof(char), 13, report);
    fwrite(result[0], sizeof(char), strlen(result[0]), report);
    fwrite("\n----USER----\n", sizeof(char), 14, report);
    fwrite(result[1], sizeof(char), strlen(result[1]), report);
    fwrite("\n----SYS-----\n", sizeof(char), 14, report);
    fwrite(result[2], sizeof(char), strlen(result[2]), report);

    fclose(report);

    for (int i = 0; i < 3; i += 1){
        free(result[i]);
    }

    fclose(data);
    fclose(a);
    fclose(b);
    fclose(c);

    return 0;
}