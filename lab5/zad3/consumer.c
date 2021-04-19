#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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

void add_input(char* save_path, int row, char* string){
    FILE* tmp = fopen("temp.txt", "w");
    FILE* save_file = fopen(save_path, "r");
    int i = 0;

    char* line = NULL;
    size_t line_len = 0;

    int l = getline_lib(&line, &line_len, save_file); 
    while (l != 0){
        if (i != row){
            fputs(line, tmp);
        }
        else{
            if(line[strlen(line)-1] == '\n'){
                fwrite(line, sizeof(char), strlen(line)-1, tmp);
            }
            else
                fwrite(line, sizeof(char), strlen(line), tmp);
            fputs(string, tmp);
            fputc('\n', tmp);
        }
        i+=1;
        l = getline_lib(&line, &line_len, save_file); 
    }
    fclose(save_file);
    fclose(tmp);
    remove(save_path);
    rename("temp.txt", save_path);

}

int main(int argc, char** argv){
    char* pipe_path;
    char* save_path;
    int N;
    if (argc == 4){
        pipe_path = argv[1];
        save_path = argv[2];
        N = atoi(argv[3]);
    }
    else{
        exit(1);
    }

    FILE* pipe_file = fopen(pipe_path, "r");
    // FILE* save_file = fopen(save_path, "w");

    char* line = NULL;
    size_t line_len = 0;

    int l = getline_lib(&line, &line_len, pipe_file); 
    while (l != 0){
        char* pch;
        char* end;
        int index;
        pch = strtok_r(line, "#\n", &end);
        index = atoi(pch);
        pch = strtok_r(NULL, "#\n", &end);
        printf("index = %d input = %s\n", index,pch);
        add_input(save_path, index, pch);
        l = getline_lib(&line, &line_len, pipe_file);

    }
    fclose(pipe_file);
    // fclose(save_file);

    return 0;
}
