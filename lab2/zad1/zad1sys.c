#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
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

void merge_and_print_sys(char* filename1, char* filename2){

    int file1 = open(filename1, O_RDONLY);
    int file2 = open(filename2, O_RDONLY);

    char* line1 = NULL;
    char* line2 = NULL;
    size_t line1_len = 0;
    size_t line2_len = 0;

    int l1 = getline_sys(&line1, &line1_len, file1);
    int l2 = getline_sys(&line2, &line2_len, file2);

    while (l1 != 0 || l2 != 0){
        if (l1 != 0){
            printf(line1);
            if(line1[strlen(line1)-1] != 10) printf("%c",10); //last line from file needs \n to add
            l1 = getline_sys(&line1, &line1_len, file1);
        }
        if (l2 != 0){
            printf(line2);
            if(line2[strlen(line2)-1] != 10) printf("%c",10);
            l2 = getline_sys(&line2, &line2_len, file2);
        }
    }

    free(line1);
    free(line2);
    close(file1);
    close(file2);

}
int main(int argc, char** argv){
    char* result[3];
    for (int i = 0; i < 3; i += 1){
        result[i] = (char*) calloc(20, sizeof(char));
    }
    int report = open("pomiar_zad_1_sys.txt", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR|S_IWUSR);
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
    merge_and_print_sys(filename1, filename2);
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