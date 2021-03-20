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
    char* result[3];
    for (int i = 0; i < 3; i += 1){
        result[i] = (char*) calloc(20, sizeof(char));
    }
    FILE* report = fopen("pomiar_zad_4_lib.txt", "w");
    struct tms usr_sys_times[2];
    clock_t time[2];

    if (argc != 5){
        perror("Wrong number of arguments, must be 4!");
        return -1;
    }

    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");
    
    char* string1 = argv[3];
    char* string2 = argv[4];

    times(&usr_sys_times[0]);
    time[0] = clock();
    fulfill_file(input, output, string1, string2);
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

    fclose(input);
    fclose(output);

    return 0;
}