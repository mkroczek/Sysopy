#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void merge_and_print(char* filename1, char* filename2){
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

    int l1 = getline(&line1, &line1_len, file1);
    int l2 = getline(&line2, &line2_len, file2);

    while (l1 != -1 || l2 != -1){
        if (l1 != -1){
            printf(line1);
            if(line1[strlen(line1)-1] != 10) printf("%c",10); //last line from file needs \n to add
            l1 = getline(&line1, &line1_len, file1);
        }
        if (l2 != -1){
            printf(line2);
            if(line2[strlen(line2)-1] != 10) printf("%c",10);
            l2 = getline(&line2, &line2_len, file2);
        }
    }

    free(line1);
    free(line2);
    fclose(file1);
    fclose(file2);

}

int main(int argc, char** argv){
    char* filename1;
    char* filename2;

    filename1= argv[1];
    filename2= argv[2];

    merge_and_print(filename1, filename2);

    return 0;

}