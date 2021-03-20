#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
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

    fclose(file);
}

int main(int argc, char** argv){
    char* result[3];
    for (int i = 0; i < 3; i += 1){
        result[i] = (char*) calloc(20, sizeof(char));
    }
    FILE* report = fopen("pomiar_zad_2_lib.txt", "w");
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

    return 0;
}