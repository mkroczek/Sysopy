#include <stdlib.h>
#include <stdio.h>
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

void merge_and_print_lib(char* filename1, char* filename2){

    FILE* file1 = fopen(filename1, "r");
    if(file1 == NULL){
        perror("Couldn't open first file");
    }
    FILE* file2 = fopen(filename2, "r");
    if(file2 == NULL){
        perror("Couldn't open second file");
    }

    char* line1 = NULL;
    char* line2 = NULL;
    size_t line1_len = 0;
    size_t line2_len = 0;

    int l1 = getline_lib(&line1, &line1_len, file1);
    int l2 = getline_lib(&line2, &line2_len, file2);

    while (l1 != 0 || l2 != 0){
        if (l1 != 0){
            printf(line1);
            if(line1[strlen(line1)-1] != 10) printf("%c",10); //last line from file needs \n to add
            l1 = getline_lib(&line1, &line1_len, file1);
        }
        if (l2 != 0){
            printf(line2);
            if(line2[strlen(line2)-1] != 10) printf("%c",10);
            l2 = getline_lib(&line2, &line2_len, file2);
        }
    }

    free(line1);
    free(line2);
    fclose(file1);
    fclose(file2);

}

int main(int argc, char** argv){
    char* result[3];
    for (int i = 0; i < 3; i += 1){
        result[i] = (char*) calloc(20, sizeof(char));
    }
    FILE* report = fopen("pomiar_zad_1_lib.txt", "w");
    struct tms usr_sys_times[2];
    clock_t time[2];
    char* filename1 = (char*) calloc(300, sizeof(char));
    char* filename2 = (char*) calloc(300, sizeof(char));

    if (argc != 3){
        printf("Enter first file name: ");
        scanf("%s", filename1);
        printf("Enter second file name: ");
        scanf("%s", filename2);
    }

    else{
        filename1= argv[1];
        filename2= argv[2];
    }

    times(&usr_sys_times[0]);
    time[0] = clock();
    merge_and_print_lib(filename1, filename2);
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